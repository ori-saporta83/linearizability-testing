#ifdef CHASE
#include "chase-lev-wrapper.h" 
#elif FASTMPMC
#include "fastmpmc-wrapper.h"
#elif HW
#include "hwqueue-wrapper.h" 
#elif LCRQ
#include "lcrq-wrapper.h"
#elif LF
#include "lfqueue-wrapper.h"
#elif LFRING
#include "lfring-wrapper.h"
#elif MPMC
#include "mpmc-queue-wrapper.h" 
#elif MS
#include "ms-queue-wrapper.h" // failed
#elif QU
#include "qu-wrapper.h"
#elif UNIQ
#include "uniq-wrapper.h"
#endif
#include "../../genmc/include/genmc.h"

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

void *noise_enq(void *arg)
{
    noise_args *args = (noise_args *)arg;
    set_thread_num(args->tid);

    for (int i = 0; i < args->count; ++i)
    {
        int * val = (int *)malloc(sizeof(int));
        *val = (args->tid * 100) + i;
        q_enqueue(args->q, val);
    }

    free(args);

    return NULL;
}

void *noise_deq(void *arg)
{
    noise_args *args = (noise_args *)arg;
    set_thread_num(args->tid);

    for (int i = 0; i < args->count; ++i)
    {
        int * res = NULL;
        bool succ = q_dequeue(args->q, &res);
        if (succ) {
            assert(*res != -1);
        }
    }

    free(args);

    return NULL;
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
            bool succ = q_dequeue(args->q, &res);
            if (succ) {
                // assert(*res != -1);
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
            bool succ = q_dequeue(args->q, &res);
            if (succ) {
                // assert(*res != -1);
            }
        }
    }

    free(args);

    return NULL;
}
