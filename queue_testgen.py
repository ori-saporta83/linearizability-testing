import os
from typing import Tuple
from typing import List
import jinja2
from jinja2 import Environment
import networkx as nx

queue_template_str = '''#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

{% for item in includes -%}
#include "{{item}}"
{% endfor %}
// {{desc}}
{{queue_type}} q;

{% for i in range(ops|length) -%}
atomic_int f_{{i}};
{% endfor -%}

{%- for op in ops %}
// {%if op.enq %}enq{% else %}deq{% endif %}({{(op.val+64)|chr}})
void *thread_{{loop.index0}}(void *arg)
{
    set_thread_num({{loop.index0}});
    {% for v in op.wait_for %}
    int val_{{v}} = 0;
    val_{{v}} = atomic_load_explicit(&f_{{v}}, memory_order_acquire);
    __VERIFIER_assume(val_{{v}} == 1);
    {% endfor -%}

    {% if op.enq %}
    int * val = (int *)malloc(sizeof(int));
    *val = {{op.val}};
    q_enqueue(&q, val);
    {% else %}
    int * res = NULL;
    bool succ = q_dequeue(&q, &res);
    {% if op.val == -1 -%}
    __VERIFIER_assume(!succ);
    {% else %}
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == {{op.val}});
    {% endif %}
    {% endif %}
    atomic_store_explicit(&f_{{loop.index0}}, 1, memory_order_release);

    return NULL;
}
{% endfor %}

int main()
{
    q_init_queue(&q, {{ops|length}});

    {% for i in participating -%}
    pthread_t t_{{i}};
    if (pthread_create(&t_{{i}}, NULL, thread_{{i}}, NULL))
        abort();
        
    {% endfor %}
    {% for i in range(noise) -%}
    {% set j = i+ops|length -%}
    pthread_t t_{{j}};
    if (pthread_create(&t_{{j}}, NULL, noise_enq_deq, create_args(&q, {{j}}, 1)))
        abort();
        
    {% endfor -%}
}
'''

queue_template_expected_str = '''#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

{% for item in includes -%}
#include "{{item}}"
{% endfor %}
// {{desc}}
{{queue_type}} q;

{% for i in range(ops|length) -%}
atomic_int f_{{i}};
{% endfor -%}

{%- for op in ops %}
// {%if op.enq %}enq{% else %}deq{% endif %}({{(op.val+64)|chr}})
void *thread_{{loop.index0}}(void *arg)
{
    set_thread_num({{loop.index0}});
    {% for v in op.wait_for %}
    int val_{{v}} = 0;
    val_{{v}} = atomic_load_explicit(&f_{{v}}, memory_order_acquire);
    __VERIFIER_assume(val_{{v}} == 1);
    {% endfor -%}

    {% if op.enq %}
    int * val = (int *)malloc(sizeof(int));
    *val = {{op.val}};
    q_enqueue(&q, val);
    {% else %}
    int * res = NULL;
    bool succ = q_dequeue(&q, &res, {{op.val}});
    {% if op.val == -1 -%}
    __VERIFIER_assume(!succ);
    {% else %}
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == {{op.val}});
    {% endif %}
    {% endif %}
    atomic_store_explicit(&f_{{loop.index0}}, 1, memory_order_release);

    return NULL;
}
{% endfor %}

int main()
{
    q_init_queue(&q, {{ops|length}});

    {% for i in participating -%}
    pthread_t t_{{i}};
    if (pthread_create(&t_{{i}}, NULL, thread_{{i}}, NULL))
        abort();
        
    {% endfor %}
    {% for i in range(noise) -%}
    {% set j = i+ops|length -%}
    pthread_t t_{{j}};
    if (pthread_create(&t_{{j}}, NULL, noise_enq_deq, create_args(&q, {{j}}, 1)))
        abort();
        
    {% endfor -%}
}
'''

def load_template(name):
    if (name == "queue"):
        return queue_template_str
    if (name == "lcrq"):
        return queue_template_expected_str
    
    return ""

class Op:
    def __init__(self, is_enq, v, wait_for):
        self.enq = is_enq
        self.val = v
        self.wait_for = wait_for

env = Environment(loader=jinja2.FunctionLoader(load_func=load_template))
env.filters["chr"] = chr

def parse_index(event_name, n, k):
    ret = 0
    if event_name[4] == "T":
        ret = n - k + ord(event_name[6]) - 97
    else:
        ret = (ord(event_name[4]) - 65) * 2
        if event_name.startswith("deq"): ret += 1
    return ret


def parse_trace(trace, n, k) -> Tuple[List[Op], int, List[int]]:
    ops = generate_ops(n, k)
    g = nx.DiGraph()
    g.add_nodes_from(i for i in range(n))
    participating = set()
    for event in trace:
        names = str(event)[1:-1].split("_")
        before_index = parse_index(names[0], n, k)
        after_index = parse_index(names[1], n, k)
        ops[after_index].wait_for.append(before_index)
        g.add_edge(before_index, after_index)
        participating.add(before_index)
        participating.add(after_index)
    width = len(max(list(nx.antichains(g)),key=len))
    return ops, width, list(participating)


def generate_ops(n, k):
    ops = []
    for i in range(n):
        is_enq = i < n-k and i % 2 == 0
        val = (int(i/2) if i < n-k else -1) + 1
        op = Op(is_enq, val, [])
        ops.append(op)
    return ops
   

def generate_test(trace, n, k):
    ops, width, participating = parse_trace(trace, n, k)
    includes = ["../../../wrappers/lcrq-wrapper-assume.h"]
    desc = str(trace)
    queue_type = "queue_t"
    template = env.get_template("lcrq")
    noise_threads = max(0, max_threads-(len(ops)-width)) if noise else 0
    participating = participating if noise else [i for i in range(len(ops))]
    return template.render(ops=ops, includes=includes, desc=desc, queue_type=queue_type, participating=participating, noise=noise_threads)

fname = "./docs/resultset2.txt"
outpath = "./tests/generated"
noise = True
max_threads = 4

def parse_params_line(line):
    parts = line.split(" ")
    return int(parts[1]), int(parts[3]), parts[5], int(parts[7])


def parse_trace_line(line):
    return line[line.index("[")+1:line.index("]")].split(", ")


def parse_testset(fname, outpath):
    f = open(fname, "r")
    lines = f.readlines()
    f.close()
    path = outpath
    n, k = 0, 0
    for i, line in enumerate(lines):
        if line.startswith("end"):
            break
        if line.startswith("start"):
            continue
        if line.startswith("n:"):
            n, k, q, N = parse_params_line(line)
            path = os.path.join(path, q+"_"+str(n)+"_"+str(k)+"_"+str(N))+"_"+str(noise)+"_"+str(max_threads)+"_lcrq"
            if not os.path.exists(path):
                os.mkdir(path)
            continue
        trace = parse_trace_line(line)
        test_data = generate_test(trace, n, k)
        f = open(os.path.join(path, "t"+str(i-1)+".c"), "w")
        f.write(test_data)
        f.close()


def main():
    parse_testset(fname, outpath)


if __name__ == "__main__":
    main()
