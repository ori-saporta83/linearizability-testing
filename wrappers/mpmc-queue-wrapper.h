#include "../../genmc/tests/correct/data-structures/mpmc-queue/mpmc-queue.h"

typedef mpmc_boundq_1_alt queue_t;

void q_enqueue(queue_t *q, unsigned int val) 
{
	int32_t *bin = write_prepare(q);
	*bin = val;
	write_publish(q);
}

bool q_dequeue(queue_t *q, unsigned int *retVal)
{
    int32_t *bin = read_fetch(q);
    if (bin != NULL) {
        *retVal = *bin;
    }
	read_consume(q);
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
