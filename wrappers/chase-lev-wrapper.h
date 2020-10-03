#include "../../genmc/tests/correct/data-structures/chase-lev/deque.h"
#include <assert.h>

typedef struct Deque queue_t;
int N = 10;

void enqueue(queue_t *q, unsigned int val) 
{
	int res = try_push(q, N, val);
    assert(res == 0);
}

bool dequeue(queue_t *q, unsigned int *retVal)
{
    int64_t val = 0;
    int res = try_steal(q, N, &val);
    *retVal = (unsigned int)val;
    return res == 0;
}

void init_queue(queue_t *q, int num_threads)
{
}
