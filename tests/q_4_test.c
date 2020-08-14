#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../../genmc/tests/correct/data-structures/ms-queue/my_queue.c"
#include "../../genmc/include/genmc.h"

// ['deq(A)_deq(B)', 'enq(B)_enq(A)']
queue_t q;

atomic_int f_1;
atomic_int f_2;
atomic_int f_3;
atomic_int f_4;

// enq(A)
void *thread_1(void *arg)
{
    int val = 0;
    do
    {
        val = atomic_load_explicit(&f_3, memory_order_seq_cst);
    } while (val != 1);

    enqueue(&q, 1);
    atomic_store_explicit(&f_1, 1, memory_order_seq_cst);

    return NULL;
}

// deq(A)
void *thread_2(void *arg)
{
    unsigned int res = 0;
    dequeue(&q, &res);
    __VERIFIER_assume(res == 1);
    atomic_store_explicit(&f_2, 1, memory_order_seq_cst);

    return NULL;
}

// enq(B)
void *thread_3(void *arg)
{
    enqueue(&q, 2);
    atomic_store_explicit(&f_3, 1, memory_order_seq_cst);

    return NULL;
}

// deq(B)
void *thread_4(void *arg)
{
    int val = 0;
    do
    {
        val = atomic_load_explicit(&f_2, memory_order_seq_cst);
    } while (val != 1);

    unsigned int res = 0;
    dequeue(&q, &res);
    __VERIFIER_assume(res == 2);
    atomic_store_explicit(&f_4, 1, memory_order_seq_cst);

    return NULL;
}

int main()
{
    init_queue(&q, 4);

    pthread_t t1, t2, t3, t4;
    if (pthread_create(&t1, NULL, thread_1, NULL))
        abort();

    if (pthread_create(&t1, NULL, thread_2, NULL))
        abort();

    if (pthread_create(&t1, NULL, thread_3, NULL))
        abort();

    if (pthread_create(&t1, NULL, thread_4, NULL))
        abort();
}

int get_thread_num()
{
	return 4;
}
