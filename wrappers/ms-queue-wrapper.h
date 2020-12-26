#include "../../genmc/tests/correct/data-structures/ms-queue/my_queue.c"

// typedef  queue_t;

void q_enqueue(queue_t *q, unsigned int val) 
{
    enqueue(q, val);
}

bool q_dequeue(queue_t *q, unsigned int *retVal)
{
    return dequeue(q, retVal);
}

void q_init_queue(queue_t *q, int num_threads)
{
    init_queue(q, num_threads);
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
