// #include "chase-lev-wrapper.h"
#include "hwqueue-wrapper.h"
// #include "lcrq-wrapper.h"
// #include "lfring-wrapper.h"
// #include "mpmc-queue-wrapper.h"
// #include "ms-queue-wrapper.h"
// #include "qu-wrapper.h"
// #include "uniq-wrapper.h"
// #include "lfqueue-wrapper.h"
// #include "fastmpmc-wrapper.h"
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
        q_enqueue(args->q, (args->tid * 100) + i);
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
        unsigned int res = 0;
        bool succ = q_dequeue(args->q, &res);
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
            unsigned int res = 0;
            bool succ = q_dequeue(args->q, &res);
        }
    }
    else
    {
        for (int i = 0; i < args->count; ++i)
        {
            q_enqueue(args->q, (args->tid * 100) + i);
        }

        for (int i = 0; i < args->count; ++i)
        {
            unsigned int res = 0;
            bool succ = q_dequeue(args->q, &res);
        }
    }

    free(args);

    return NULL;
}
