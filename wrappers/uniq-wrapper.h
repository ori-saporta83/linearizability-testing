// uniQ The Lock free Queue.
// See test.cpp for use case & README.md for details about the solution
// Released under LGPL 3.0
// https://github.com/bittnkr/uniq/blob/master/cpp/uniq.h

#pragma once
#include <assert.h>
#include "../../genmc/include/genmc.h"
#include <stdatomic.h>

#define MAX_SIZE 32

typedef int val_t;

typedef struct queue_t
{
    val_t* buffer[MAX_SIZE];
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

#define MAX_ITER 2

void enqueue(queue_t *q, val_t * val)
{
    int in, out;
    bool used;
    // do
    // {
    //     i = q->in;
    //     if (i - q->out > q->mask)
    //         return;
    // } while (q->isUsed[i & q->mask] || !atomic_compare_exchange_weak(&(q->in), &i, i + 1));
    int cnt = 0;
    do 
    {
        __VERIFIER_assume(++cnt < MAX_ITER);
        in = atomic_load_explicit(&q->in, memory_order_relaxed);
        out = atomic_load_explicit(&q->out, memory_order_relaxed);
        if (in - out > q->mask)
            return;
    } while (!atomic_compare_exchange_weak_explicit(&(q->in), &in, in + 1, memory_order_relaxed, memory_order_relaxed));

    q->buffer[in & q->mask] = val;
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&q->isUsed[in & q->mask], true, memory_order_relaxed);
    return;
}

void q_enqueue(queue_t *q, int * val) {
    enqueue(q, val);
}

bool dequeue(queue_t *q, val_t **retVal)
{
    int out, in;
    bool used;
    // do
    // {
    //     o = q->out;
    //     if (o == q->in)
    //         return -1;
    // } while (!q->isUsed[o & q->mask] || !atomic_compare_exchange_weak(&(q->out), &o, o + 1));
    int cnt = 0;
    do 
    {
        __VERIFIER_assume(++cnt < MAX_ITER);
        out = atomic_load_explicit(&q->out, memory_order_relaxed);
        in = atomic_load_explicit(&q->in, memory_order_relaxed);
        if (out == in)
            return false;

        used = atomic_load_explicit(&q->isUsed[out & q->mask], memory_order_relaxed);
        
    } while (!used || !atomic_compare_exchange_weak_explicit(&(q->out), &out, out + 1, memory_order_relaxed, memory_order_relaxed));

    atomic_thread_fence(memory_order_acquire);
    *retVal = q->buffer[out &= q->mask];
    atomic_store_explicit(&q->isUsed[out], false, memory_order_relaxed);
    return true;
}

bool q_dequeue(queue_t *q, int **retVal) {
    return dequeue(q, retVal);
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