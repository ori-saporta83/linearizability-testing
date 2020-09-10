#%%
from jinja2 import Template, Environment
template = Template('Hello {{ name }}!')
template.render(name='John Doe')

#%%
import jinja2
from jinja2 import Template, Environment
t = '''
{% for item in l -%}
{{item}}
{{loop.index0}}
{% endfor %}
{% for i in range(l|length) -%}
{{(i+65)|chr}}
{% endfor %}
'''
def load_template(name):
    return t

env = Environment(loader=jinja2.FunctionLoader(load_func=load_template))
env.filters['chr'] = chr
template = env.get_template("")
print(template.render(l=['a', 'b', 'c', 'd']))

#%%
import jinja2
from jinja2 import Template, Environment
template_str = '''#include <stdlib.h>
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
    val_{{v}} = atomic_load_explicit(&f_{{v}}, memory_order_seq_cst);
    __VERIFIER_assume(val_{{v}} == 1);
    {% endfor -%}

    {% if op.enq %}
    enqueue(&q, {{op.val}});
    {% else %}
    unsigned int res = 0;
    bool succ = dequeue(&q, &res);
    __VERIFIER_assume(succ);
    __VERIFIER_assume(res == {{op.val}});
    {% endif %}
    atomic_store_explicit(&f_{{loop.index0}}, 1, memory_order_seq_cst);

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
    return template_str

class Op:
    def __init__(self, is_enq, v, wait_for):
        self.enq = is_enq
        self.val = v
        self.wait_for = wait_for

ops = [Op(True, 1, [2]), Op(False, 1, []), Op(True, 2, []), Op(False, 2, [1])]
includes = ["../../genmc/tests/correct/data-structures/ms-queue/my_queue.c","../../genmc/include/genmc.h"]
desc = "['deq(A)_deq(B)', 'enq(B)_enq(A)']"
queue_type = "queue_t"
env = Environment(loader=jinja2.FunctionLoader(load_func=load_template))
env.filters["chr"] = chr
template = env.get_template("")
print(template.render(ops=ops, includes=includes, desc=desc, queue_type=queue_type))