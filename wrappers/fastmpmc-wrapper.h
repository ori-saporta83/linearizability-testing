//https://github.com/spectre1989/fast_mpmc_queue/blob/master/MPMCQueue.h
#pragma once

#include <stdatomic.h>
#include <stdalign.h>
#include <stdlib.h>
#include <stdint.h>
#include "../../genmc/include/genmc.h"

#define MAX_SIZE 32

typedef int val_t;

typedef struct Item
{
    atomic_uint_fast64_t version;
    val_t * value;
} Item;

typedef struct queue_t
{
    Item *m_items;
    size_t m_capacity;

    // Make sure each index is on a different cache line
    atomic_uint_fast64_t m_head, m_tail;
} queue_t;

void q_init_queue(queue_t *q, int num_threads)
{
    q->m_capacity = MAX_SIZE;
    q->m_items = (Item *)malloc(sizeof(Item) * q->m_capacity);
    q->m_head = q->m_tail = 0;
    for (size_t i = 0; i < q->m_capacity; ++i)
    {
        q->m_items[i].version = i;
    }
}

#define MAX_ITER 2

void q_enqueue(queue_t *q, val_t * val)
{
    int cnt = 0;
    uint64_t tail, t;
    while (true) {
        __VERIFIER_assume(++cnt < MAX_ITER);

        tail = atomic_load_explicit(&(q->m_tail), memory_order_relaxed);
        t = atomic_load_explicit(&(q->m_items[tail % q->m_capacity].version), memory_order_relaxed);
        if (t != tail) continue;

        if (!atomic_compare_exchange_weak_explicit(&(q->m_tail), &tail, tail + 1, memory_order_relaxed, memory_order_relaxed)) continue;

        break;
    }

    q->m_items[tail % q->m_capacity].value = val;

    // Release operation, all reads/writes before this store cannot be reordered past it
    // Writing version to tail + 1 signals reader threads when to read payload
    atomic_store_explicit(&(q->m_items[tail % q->m_capacity].version), tail + 1, memory_order_release);
}

bool q_dequeue(queue_t *q, val_t **retVal)
{
    int cnt = 0;
    uint64_t head, h;
    while (true) {
        __VERIFIER_assume(++cnt < MAX_ITER);

        head = atomic_load_explicit(&(q->m_head), memory_order_relaxed);
        uint64_t tail = atomic_load_explicit(&(q->m_tail), memory_order_relaxed);
        if (head == tail) return false;

        // Acquire here makes sure read of m_data[head].value is not reordered before this
        // Also makes sure side effects in try_enqueue are visible here
        h = atomic_load_explicit(&(q->m_items[head % q->m_capacity].version), memory_order_acquire);
        if (h != head + 1) continue;

        if (!atomic_compare_exchange_weak_explicit(&(q->m_head), &head, head + 1, memory_order_relaxed, memory_order_relaxed)) continue;

        break;
    }

    *retVal = q->m_items[head % q->m_capacity].value;

    // This signals to writer threads that they can now write something to this index
    atomic_store_explicit(&(q->m_items[head % q->m_capacity].version), head + q->m_capacity, memory_order_relaxed);

    return true;
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