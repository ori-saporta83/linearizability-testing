// uniQ The Lock free Queue.
// See test.cpp for use case & README.md for details about the solution
// Released under LGPL 3.0
// https://github.com/bittnkr/uniq/blob/master/cpp/uniq.h

#pragma once
#include <assert.h>
#include "../../genmc/include/genmc.h"
#include <stdatomic.h>

#define MAX_SIZE 32

typedef struct queue_t
{
    atomic_uint buffer[MAX_SIZE];
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
    int in, out;
    // do
    // {
    //     i = q->in;
    //     if (i - q->out > q->mask)
    //         return;
    // } while (q->isUsed[i & q->mask] || !atomic_compare_exchange_weak(&(q->in), &i, i + 1));
    in = atomic_load_explicit(&q->in, memory_order_relaxed);
    out = atomic_load_explicit(&q->out, memory_order_relaxed);
    if (in - out > q->mask)
        return;
    bool used = atomic_load_explicit(&q->isUsed[in & q->mask], memory_order_relaxed);
    __VERIFIER_assume(!used);
    __VERIFIER_assume(atomic_compare_exchange_weak_explicit(&(q->in), &in, in + 1, memory_order_relaxed, memory_order_relaxed));

    atomic_store_explicit(&q->buffer[in & q->mask], val, memory_order_relaxed);
    atomic_store_explicit(&q->isUsed[in & q->mask], true, memory_order_relaxed);
    return;
}

bool q_dequeue(queue_t *q, unsigned int *retVal)
{
    int out, in;
    // do
    // {
    //     o = q->out;
    //     if (o == q->in)
    //         return -1;
    // } while (!q->isUsed[o & q->mask] || !atomic_compare_exchange_weak(&(q->out), &o, o + 1));
    out = atomic_load_explicit(&q->out, memory_order_relaxed);
    in = atomic_load_explicit(&q->in, memory_order_relaxed);
    if (out == in)
        return -1;
    bool used = atomic_load_explicit(&q->isUsed[out & q->mask], memory_order_relaxed);
    __VERIFIER_assume(used);
    __VERIFIER_assume(atomic_compare_exchange_weak_explicit(&(q->out), &out, out + 1, memory_order_relaxed, memory_order_relaxed));

    unsigned int r = atomic_load_explicit(&q->buffer[out &= q->mask], memory_order_relaxed);
    atomic_store_explicit(&q->isUsed[out], false, memory_order_relaxed);
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