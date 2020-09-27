import jinja2
from jinja2 import Template, Environment

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

int __thread tid;

void set_thread_num(int i)
{
	tid = i;
}

int get_thread_num()
{
	return tid;
}

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
    enqueue(&q, {{op.val}});
    {% else %}
    unsigned int res = 0;
    bool succ = dequeue(&q, &res);
    {% if op.val == -1 -%}
    __VERIFIER_assume(!succ);
    {% else %}
    __VERIFIER_assume(succ);
    __VERIFIER_assume(res == {{op.val}});
    {% endif %}
    {% endif %}
    atomic_store_explicit(&f_{{loop.index0}}, 1, memory_order_release);

    return NULL;
}
{% endfor %}

int main()
{
    init_queue(&q, {{ops|length}});

    {% for i in range(ops|length) -%}
    pthread_t t_{{i}};
    if (pthread_create(&t_{{i}}, NULL, thread_{{i}}, NULL))
        abort();
        
    {% endfor %}
}
'''
def load_template(name):
    if (name == "queue"):
        return queue_template_str
    
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


def parse_trace(trace, n, k):
    ops = generate_ops(n, k)
    for event in trace:
        names = str(event)[1:-1].split("_")
        before_index = parse_index(names[0], n, k)
        after_index = parse_index(names[1], n, k)
        ops[after_index].wait_for.append(before_index)
    return ops


def generate_ops(n, k):
    ops = []
    for i in range(n):
        is_enq = i < n-k and i % 2 == 0
        val = (int(i/2) if i < n-k else -1) + 1
        op = Op(is_enq, val, [])
        ops.append(op)
    return ops

queue_import_map = {
    "ms-queue": "../../genmc/tests/correct/data-structures/ms-queue/my_queue.c",
    "qu": "../../wrappers/qu-wrapper.h", 
    "mpmc": "../../wrappers/mpmc-queue-wrapper.h"
}
    

def generate_test(trace, n, k, q):
    ops = parse_trace(trace, n, k)
    includes = [queue_import_map[q], "../../genmc/include/genmc.h"]
    desc = str(trace)
    queue_type = "queue_t"
    template = env.get_template("queue")
    return template.render(ops=ops, includes=includes, desc=desc, queue_type=queue_type)