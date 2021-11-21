#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../../../wrappers/lcrq-wrapper-assume.h"

// ["'enq(C)_deq(C)'", "'enq(C)_deq(T-b)'", "'deq(T-b)_deq(C)'"]
queue_t q;

atomic_int f_0;
atomic_int f_1;
atomic_int f_2;
atomic_int f_3;
atomic_int f_4;
atomic_int f_5;
atomic_int f_6;
atomic_int f_7;
atomic_int f_8;
atomic_int f_9;

// enq(A)
void *thread_0(void *arg)
{
    set_thread_num(0);
    
    int * val = (int *)malloc(sizeof(int));
    *val = 1;
    q_enqueue(&q, val);
    
    atomic_store_explicit(&f_0, 1, memory_order_release);

    return NULL;
}

// deq(A)
void *thread_1(void *arg)
{
    set_thread_num(1);
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res, 1);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == 1);
    
    
    atomic_store_explicit(&f_1, 1, memory_order_release);

    return NULL;
}

// enq(B)
void *thread_2(void *arg)
{
    set_thread_num(2);
    
    int * val = (int *)malloc(sizeof(int));
    *val = 2;
    q_enqueue(&q, val);
    
    atomic_store_explicit(&f_2, 1, memory_order_release);

    return NULL;
}

// deq(B)
void *thread_3(void *arg)
{
    set_thread_num(3);
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res, 2);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == 2);
    
    
    atomic_store_explicit(&f_3, 1, memory_order_release);

    return NULL;
}

// enq(C)
void *thread_4(void *arg)
{
    set_thread_num(4);
    
    int * val = (int *)malloc(sizeof(int));
    *val = 3;
    q_enqueue(&q, val);
    
    atomic_store_explicit(&f_4, 1, memory_order_release);

    return NULL;
}

// deq(C)
void *thread_5(void *arg)
{
    set_thread_num(5);
    
    int val_4 = 0;
    val_4 = atomic_load_explicit(&f_4, memory_order_acquire);
    __VERIFIER_assume(val_4 == 1);
    
    int val_9 = 0;
    val_9 = atomic_load_explicit(&f_9, memory_order_acquire);
    __VERIFIER_assume(val_9 == 1);
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res, 3);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == 3);
    
    
    atomic_store_explicit(&f_5, 1, memory_order_release);

    return NULL;
}

// enq(D)
void *thread_6(void *arg)
{
    set_thread_num(6);
    
    int * val = (int *)malloc(sizeof(int));
    *val = 4;
    q_enqueue(&q, val);
    
    atomic_store_explicit(&f_6, 1, memory_order_release);

    return NULL;
}

// deq(D)
void *thread_7(void *arg)
{
    set_thread_num(7);
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res, 4);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == 4);
    
    
    atomic_store_explicit(&f_7, 1, memory_order_release);

    return NULL;
}

// deq(@)
void *thread_8(void *arg)
{
    set_thread_num(8);
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res, 0);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == 0);
    
    
    atomic_store_explicit(&f_8, 1, memory_order_release);

    return NULL;
}

// deq(@)
void *thread_9(void *arg)
{
    set_thread_num(9);
    
    int val_4 = 0;
    val_4 = atomic_load_explicit(&f_4, memory_order_acquire);
    __VERIFIER_assume(val_4 == 1);
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res, 0);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == 0);
    
    
    atomic_store_explicit(&f_9, 1, memory_order_release);

    return NULL;
}


int main()
{
    q_init_queue(&q, 10);

    pthread_t t_9;
    if (pthread_create(&t_9, NULL, thread_9, NULL))
        abort();
        
    pthread_t t_4;
    if (pthread_create(&t_4, NULL, thread_4, NULL))
        abort();
        
    pthread_t t_5;
    if (pthread_create(&t_5, NULL, thread_5, NULL))
        abort();
        
    
    pthread_t t_10;
    if (pthread_create(&t_10, NULL, noise_enq_deq, create_args(&q, 10, 1)))
        abort();
        
    pthread_t t_11;
    if (pthread_create(&t_11, NULL, noise_enq_deq, create_args(&q, 11, 1)))
        abort();
        
    }