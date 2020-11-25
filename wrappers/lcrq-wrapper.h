#define	EXIT_FAILURE	1

#include "../../synch1.0.1/lcrq_headless.c"

typedef RingQueue queue_t;

int __thread tid;

void set_thread_num(int i)
{
	tid = i;
}

int get_thread_num()
{
	return tid;
}

void enqueue(queue_t *q, unsigned int val)
{    
    enqueue_t(val, get_thread_num());
}

bool dequeue(queue_t *q, unsigned int *retVal)
{
    *retVal = dequeue_t(get_thread_num());
    return *retVal != -1;
}

void init_queue(queue_t *q, int num_threads)
{
    SHARED_OBJECT_INIT();
}
