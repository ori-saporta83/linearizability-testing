#include "../../genmc/tests/correct/data-structures/mpmc-queue/mpmc-queue.h"

typedef mpmc_boundq_1_alt queue_t;

void q_enqueue(queue_t *q, unsigned int val) 
{
	atomic_int_fast32_t *bin = write_prepare(q);
	atomic_store_explicit(bin, val, memory_order_relaxed);
	write_publish(q);
}

bool q_dequeue(queue_t *q, unsigned int *retVal)
{
    atomic_int_fast32_t *bin = read_fetch(q);
    if (bin != NULL) {
        *retVal = atomic_load_explicit(bin, memory_order_relaxed);
    }
	return true;
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
