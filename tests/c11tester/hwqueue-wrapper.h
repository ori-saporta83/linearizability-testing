#include <stdatomic.h>

#ifndef MAX
# define MAX 42
#endif

typedef int val_t;

typedef struct queue_t {
    val_t * _Atomic AR[MAX];
    atomic_int back;
} queue_t;

void q_enqueue(queue_t *q, val_t * val) 
{
	int k = atomic_fetch_add_explicit(&(q->back), 1, memory_order_release);
	// int k = atomic_fetch_add_explicit(&(q->back), 1, memory_order_acq_rel);
	atomic_store_explicit(&(q->AR[k]), val, memory_order_relaxed);
}

bool q_dequeue(queue_t *q, val_t **retVal)
{

	int lback = atomic_load_explicit(&(q->back), memory_order_acquire);
	val_t * lan = NULL;


	if (lback > 0) 
	{
		for (int k = 0; lan == NULL && k < lback; ++k) {
			lan = atomic_exchange_explicit(&(q->AR[k]), 0, memory_order_relaxed);
		}
	}
    *retVal = lan;
	return lan != NULL;
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