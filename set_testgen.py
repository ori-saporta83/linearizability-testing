from typing import List, Tuple
import jinja2
from jinja2 import Template, Environment
import os
import networkx as nx

set_template_str = '''#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

{% for item in includes -%}
#include "{{item}}"
{% endfor %}
// {{desc}}
{{set_type}} set;

{% for i in range(ops|length) -%}
atomic_int f_{{i}};
{% endfor -%}

{%- for op in ops %}
// {{op.desc}}
void *thread_{{loop.index0}}(void *arg)
{
    set_thread_num({{loop.index0}});
    {% for v in op.wait_for %}
    int val_{{v}} = 0;
    val_{{v}} = atomic_load_explicit(&f_{{v}}, memory_order_acquire);
    __VERIFIER_assume(val_{{v}} == 1);
    {% endfor -%}

    {% if op.type == 0 %}
    bool succ = w_add(&set, {{op.val}} * 10);
    {% elif op.type == 1 %}
    bool succ = w_remove(&set, {{op.val}} * 10);
    {% else %}
    bool succ = w_in(&set, {{op.val}} * 10);
    {% endif %}
    {% if op.succ -%}
    __VERIFIER_assume(succ);
    {% else %}
    __VERIFIER_assume(!succ);
    {% endif %}
    atomic_store_explicit(&f_{{loop.index0}}, 1, memory_order_release);

    return NULL;
}
{% endfor %}

int main()
{
    init_set(&set, {{ops|length}});

    {% for i in participating -%}
    pthread_t t_{{i}};
    if (pthread_create(&t_{{i}}, NULL, thread_{{i}}, NULL))
        abort();
        
    {% endfor %}
    {% for i in range(noise) -%}
    {% set j = i+ops|length -%}
    pthread_t t_{{j}};
    if (pthread_create(&t_{{j}}, NULL, noise_gen, create_args(&set, {{j}}, 1, {{ops|length}})))
        abort();
        
    {% endfor %}
}
'''


def load_template(name):
    if (name == "set"):
        return set_template_str

    return ""


class Op:
    def __init__(self, opType, v, success, description, wait_for):
        self.type = opType
        self.val = v
        self.succ = success
        self.desc = description
        self.wait_for = wait_for


env = Environment(loader=jinja2.FunctionLoader(load_func=load_template))
env.filters["chr"] = chr


def parse_index(event_name, n, k, l):
    group_index = ord(event_name[event_name.index("(")+1]) - 65
    offset = 0
    if event_name.startswith("remove"):
        offset = 1
    elif event_name.startswith("in"):
        offset = 2 + int(event_name[8])
        if (event_name[6] == "f"):
            offset += k

    return group_index * (2 + k + l) + offset


def parse_trace(trace, n, k, l) -> Tuple[List[Op], int, List[int]]:
    ops = generate_ops(n, k, l)
    g = nx.DiGraph()
    g.add_nodes_from(i for i in range(len(ops)))
    participating = set()
    for event in trace:
        names = str(event)[1:-1].split("_")
        before_index = parse_index(names[0], n, k, l)
        after_index = parse_index(names[1], n, k, l)
        ops[after_index].wait_for.append(before_index)
        g.add_edge(before_index, after_index)
        participating.add(before_index)
        participating.add(after_index)
    width = len(max(list(nx.antichains(g)), key=len))
    return ops, width, list(participating)


def generate_ops(n, k, l):
    ops = []
    for i in range(n):
        v = i+1
        desc = "add("+chr(i+65)+")"
        op = Op(0, v, True, desc, [])
        ops.append(op)
        desc = "remove("+chr(i+65)+")"
        op = Op(1, v, True, desc, [])
        ops.append(op)
        for j in range(k):
            desc = "in("+chr(i+65)+")-t-"+str(j)
            op = Op(2, v, True, desc, [])
            ops.append(op)
        for j in range(l):
            desc = "in("+chr(i+65)+")-f-"+str(j)
            op = Op(3, v, False, desc, [])
            ops.append(op)
    return ops


def parse_params_line(line):
    return int(line[3]), int(line[8]), int(line[13])


def parse_trace_line(line):
    return line[line.index("[")+1:line.index("]")].split(", ")


def generate_test(trace, n, k, l):
    ops, width, participating = parse_trace(trace, n, k, l)
    includes = ["../../../wrappers/set-wrappers.h"]
    desc = str(trace)
    set_type = "set_t"
    template = env.get_template("set")
    noise_threads = max(0, max_threads-(len(ops)-width)) if noise else 0
    participating = participating if noise else [i for i in range(len(ops))]
    return template.render(ops=ops, includes=includes, desc=desc, set_type=set_type, participating=participating, noise=noise_threads)


test_set = "generic-set"
fname = "./docs/set-results2.txt"
outpath = "./tests/generated"
max_threads = 4
noise = True


def parse_testset(fname, outpath):
    f = open(fname, "r")
    lines = f.readlines()
    f.close()
    n, k, l = 0, 0, 0
    path = outpath
    for i, line in enumerate(lines):
        if line.startswith("end"):
            break
        if line.startswith("start"):
            continue
        if line.startswith("n:"):
            n, k, l = parse_params_line(line)
            path = os.path.join(path, test_set+"_" +
                                str(n)+"_"+str(k)+"_"+str(l)+"_"+str(noise)+"_"+str(max_threads))
            if not os.path.exists(path):
                os.mkdir(path)
            continue
        trace = parse_trace_line(line)
        test_data = generate_test(trace, n, k, l)
        f = open(os.path.join(path, "t"+str(i-1)+".c"), "w")
        f.write(test_data)
        f.close()


def main():
    parse_testset(fname, outpath)


if __name__ == "__main__":
    main()
