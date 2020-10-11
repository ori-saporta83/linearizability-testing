#include "../../genmc/tests/correct/data-structures/mpmc-queue/mpmc-queue.h"

typedef mpmc_boundq_1_alt queue_t;

void enqueue(queue_t *q, unsigned int val) 
{
	atomic_int_fast32_t *bin = write_prepare(q);
	atomic_store_explicit(bin, val, memory_order_relaxed);
	write_publish(q);
}

bool dequeue(queue_t *q, unsigned int *retVal)
{
    atomic_int_fast32_t *bin = read_fetch(q);
    if (bin != NULL) {
        *retVal = atomic_load_explicit(bin, memory_order_relaxed);
        // *retVal = load_32(bin);
    }
	read_consume(q);
	return true;
}

void init_queue(queue_t *q, int num_threads)
{
}
