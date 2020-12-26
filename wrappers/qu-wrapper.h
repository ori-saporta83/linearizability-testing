#include <assert.h>
#include "../../genmc/tests/correct/data-structures/qu/qu.c"


typedef struct queue queue_t;

void q_enqueue(queue_t *q, unsigned int val) 
{
    int ret = queue_try_enq(q, val);
    // assert(ret == 0);
}

bool q_dequeue(queue_t *q, unsigned int *retVal)
{
    int ret = queue_try_deq(q, (int *)retVal);
    return ret == 0;
}

void q_init_queue(queue_t *q, int num_threads)
{
    queue_init(q);
}

#ifndef MAX_THREADS
# define MAX_THREADS 32
#endif
#ifndef MAX_FREELIST
# define MAX_FREELIST 32 /* Each thread can own up to MAX_FREELIST free nodes */
#endif
unsigned int idx[MAX_THREADS + 1];
struct queue_node free_lists[MAX_THREADS + 1][MAX_FREELIST];
unsigned int free_index[MAX_THREADS + 1];
atomic_uint debug;

struct queue_node *new_node()
{
	int t = get_thread_num();
    unsigned int free_val = free_index[t];

    atomic_store_explicit(&debug, free_val, memory_order_relaxed);
	assert(free_val < MAX_FREELIST);
	return &free_lists[t][free_index[t]++];
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