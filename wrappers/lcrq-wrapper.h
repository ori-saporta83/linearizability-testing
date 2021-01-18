#define	EXIT_FAILURE	1

#include "lcrq.c"

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

void q_enqueue(queue_t *q, unsigned int val)
{    
    enqueue(val, get_thread_num());
}

bool q_dequeue(queue_t *q, unsigned int *retVal)
{
    *retVal = dequeue(get_thread_num());
    return *retVal != -1;
}

void q_init_queue(queue_t *q, int num_threads)
{
    SHARED_OBJECT_INIT();
}
