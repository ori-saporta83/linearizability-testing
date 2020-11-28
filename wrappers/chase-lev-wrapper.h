#include "../../genmc/tests/correct/data-structures/chase-lev/deque.h"
#include <assert.h>

typedef struct Deque queue_t;
int N = 10;

void q_enqueue(queue_t *q, unsigned int val) 
{
	int res = try_push(q, N, val);
    assert(res == 0);
}

bool q_dequeue(queue_t *q, unsigned int *retVal)
{
    int64_t val = 0;
    int res = try_steal(q, N, &val);
    *retVal = (unsigned int)val;
    return res == 0;
}

void q_init_queue(queue_t *q, int num_threads)
{
}

int __thread tid;

void set_thread_num(int i)
{
	tid = i;
}

int get_thread_num()
{
	return tid;
}