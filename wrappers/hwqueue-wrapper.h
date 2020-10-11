#include "../../genmc/include/genmc.h"
#include <stdatomic.h>

#ifndef MAX
# define MAX 42
#endif

typedef struct queue_t {
    atomic_int AR[MAX];
    atomic_int back;
} queue_t;

void enqueue(queue_t *q, unsigned int val) 
{
	int k = atomic_fetch_add_explicit(&(q->back), 1, memory_order_acq_rel);
	atomic_store_explicit(&(q->AR[k]), val, memory_order_release);
}

bool dequeue(queue_t *q, unsigned int *retVal)
{
	int lback = atomic_load_explicit(&(q->back), memory_order_acquire);
	int lan, k;

	for (lan = k = 0; lan == 0; ++k) {
		__VERIFIER_assume(k < lback);
		// lan = atomic_exchange_explicit(&(q->AR[k]), 0, memory_order_acq_rel);
		lan = atomic_exchange_explicit(&(q->AR[k]), 0, memory_order_acquire);
	}
    *retVal = lan;
	return true;
}

void init_queue(queue_t *q, int num_threads)
{
}
