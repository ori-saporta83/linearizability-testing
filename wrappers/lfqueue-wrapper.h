#include "../../lfqueue/lfqueue.c"

typedef lfqueue_t queue_t;

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
    unsigned int* pdata = (unsigned int*) malloc(sizeof(unsigned int));
    *pdata = val;
    lfqueue_enq(q, pdata);
}

bool q_dequeue(queue_t *q, unsigned int *retVal)
{
    unsigned int* pdata = (unsigned int *) lfqueue_deq(q);
    *retVal = *pdata;
    return true;
}

void q_init_queue(queue_t *q, int num_threads)
{
    lfqueue_init(q);
}