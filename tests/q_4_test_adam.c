#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

// #define WFQUEUE
// #include "../../fast-wait-free-queue/wfqueue.c"
// #define CCQUEUE
// #include "../../fast-wait-free-queue/ccqueue.c"
// #define MSQUEUE
// #include "../../fast-wait-free-queue/msqueue.c"
#define LCRQ
#include "../../fast-wait-free-queue/lcrq.c"

#include "../../genmc/include/genmc.h"

// ['deq(A)_deq(B)', 'enq(B)_enq(A)']
queue_t q;

int __thread tid;

void set_thread_num(int i)
{
    tid = i;
}

int get_thread_num()
{
    return tid;
}

atomic_int f_1;
atomic_int f_2;
atomic_int f_3;
atomic_int f_4;

// enq(A)
void *thread_0(void *arg)
{
    set_thread_num(0);
    handle_t *h = align_malloc(PAGE_SIZE, sizeof(handle_t));
    queue_register(&q, h, 0);

    int val = 0;
    val = atomic_load_explicit(&f_3, memory_order_acquire);
    __VERIFIER_assume(val == 1);

    enqueue(&q, h, (void *)1);
    atomic_store_explicit(&f_1, 1, memory_order_release);

    return NULL;
}

// deq(A)
void *thread_1(void *arg)
{
    set_thread_num(1);
    handle_t *h = align_malloc(PAGE_SIZE, sizeof(handle_t));
    queue_register(&q, h, 1);

    unsigned int res = (unsigned int)dequeue(&q, h);
    __VERIFIER_assume(res == 1);
    atomic_store_explicit(&f_2, 1, memory_order_seq_cst);

    return NULL;
}

// enq(B)
void *thread_2(void *arg)
{
    set_thread_num(2);
    handle_t *h = align_malloc(PAGE_SIZE, sizeof(handle_t));
    queue_register(&q, h, 2);

    enqueue(&q, h, (void *)2);

    atomic_store_explicit(&f_3, 1, memory_order_seq_cst);

    return NULL;
}

// deq(B)
void *thread_3(void *arg)
{
    set_thread_num(3);
    handle_t *h = align_malloc(PAGE_SIZE, sizeof(handle_t));
    queue_register(&q, h, 3);

    int val = 0;
    val = atomic_load_explicit(&f_2, memory_order_seq_cst);
    __VERIFIER_assume(val == 1);

    unsigned int res = (unsigned int)dequeue(&q, h);
    __VERIFIER_assume(res == 2);
    atomic_store_explicit(&f_4, 1, memory_order_seq_cst);

    return NULL;
}

int main()
{
    queue_init(&q, 4);

    pthread_t t0, t1, t2, t3;
    if (pthread_create(&t0, NULL, thread_0, NULL))
        abort();

    if (pthread_create(&t1, NULL, thread_1, NULL))
        abort();

    if (pthread_create(&t2, NULL, thread_2, NULL))
        abort();

    if (pthread_create(&t3, NULL, thread_3, NULL))
        abort();
}
