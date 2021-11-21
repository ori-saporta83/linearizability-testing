#define	EXIT_FAILURE	1

#include "lcrq/lcrq-assume.c"

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

void q_enqueue(queue_t *q, int * val)
{    
    enqueue(*val, get_thread_num());
}

bool q_dequeue(queue_t *q, int **retVal, int expected)
{
    *retVal = (int *)malloc(sizeof(int));
    **retVal = dequeue(get_thread_num(), expected);
    return **retVal != -1;
}

void q_init_queue(queue_t *q, int num_threads)
{
    SHARED_OBJECT_INIT();
}

typedef struct noise_args
{
    queue_t *q;
    int tid;
    int count;
} noise_args;

noise_args *create_args(queue_t *q, int tid, int count)
{
    noise_args *args = malloc(sizeof(noise_args));
    args->q = q;
    args->tid = tid;
    args->count = count;
    return args;
}

void *noise_enq_deq(void *arg)
{
    noise_args *args = (noise_args *)arg;
    set_thread_num(args->tid);

    if ((args->tid % 2) == 0)
    {
        for (int i = 0; i < args->count; ++i)
        {
            int * res = NULL;
            bool succ = q_dequeue(args->q, &res, 0);
            if (succ) {
                assert(*res != -1);
            }
        }
    }
    else
    {
        for (int i = 0; i < args->count; ++i)
        {
            int * val = (int *)malloc(sizeof(int));
            *val = (args->tid * 100) + i;
            q_enqueue(args->q, val);
        }

        for (int i = 0; i < args->count; ++i)
        {
            int * res = NULL;
            bool succ = q_dequeue(args->q, &res, 0);
            if (succ) {
                assert(*res != -1);
            }
        }
    }

    free(args);

    return NULL;
}