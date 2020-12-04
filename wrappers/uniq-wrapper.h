// uniQ The Lock free Queue.
// See test.cpp for use case & README.md for details about the solution
// Released under LGPL 3.0

#pragma once
#include <assert.h>
#include "../../genmc/include/genmc.h"
#include <stdatomic.h>

#define MAX_SIZE 32

typedef struct queue_t
{
    unsigned int buffer[MAX_SIZE];
    int mask;
    atomic_int in, out;
    atomic_bool isUsed[MAX_SIZE];

} queue_t;

void q_init_queue(queue_t *q, int num_threads)
{
    q->mask = MAX_SIZE;
    q->mask--;

    q->out = q->in = 0;
}

void q_enqueue(queue_t *q, unsigned int val)
{
    int i;
    // do
    // {
    //     i = q->in;
    //     if (i - q->out > q->mask)
    //         return;
    // } while (q->isUsed[i & q->mask] || !atomic_compare_exchange_weak(&(q->in), &i, i + 1));
    i = q->in;
    if (i - q->out > q->mask)
        return;
    __VERIFIER_assume(!q->isUsed[i & q->mask]);
    __VERIFIER_assume(atomic_compare_exchange_weak(&(q->in), &i, i + 1));

    q->buffer[i & q->mask] = val;
    q->isUsed[i & q->mask] = true;
    return;
}

bool q_dequeue(queue_t *q, unsigned int *retVal)
{
    int o;
    // do
    // {
    //     o = q->out;
    //     if (o == q->in)
    //         return -1;
    // } while (!q->isUsed[o & q->mask] || !atomic_compare_exchange_weak(&(q->out), &o, o + 1));
    o = q->out;
    if (o == q->in)
        return -1;
    __VERIFIER_assume(q->isUsed[o & q->mask]);
    __VERIFIER_assume(atomic_compare_exchange_weak(&(q->out), &o, o + 1));

    unsigned int r = q->buffer[o &= q->mask];
    q->isUsed[o] = false;
    return r;
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