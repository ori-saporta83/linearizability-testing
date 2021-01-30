#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../../../wrappers/queue-wrappers.h"

// ["'deq(A)_enq(B)'", "'deq(B)_enq(E)'", "'deq(E)_enq(A)'"]
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
    
    int val_9 = 0;
    val_9 = atomic_load_explicit(&f_9, memory_order_acquire);
    __VERIFIER_assume(val_9 == 1);
    
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
    bool succ = q_dequeue(&q, &res);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == 1);
    
    
    atomic_store_explicit(&f_1, 1, memory_order_release);

    return NULL;
}

// enq(B)
void *thread_2(void *arg)
{
    set_thread_num(2);
    
    int val_1 = 0;
    val_1 = atomic_load_explicit(&f_1, memory_order_acquire);
    __VERIFIER_assume(val_1 == 1);
    
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
    bool succ = q_dequeue(&q, &res);
    
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
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res);
    
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
    bool succ = q_dequeue(&q, &res);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == 4);
    
    
    atomic_store_explicit(&f_7, 1, memory_order_release);

    return NULL;
}

// enq(E)
void *thread_8(void *arg)
{
    set_thread_num(8);
    
    int val_3 = 0;
    val_3 = atomic_load_explicit(&f_3, memory_order_acquire);
    __VERIFIER_assume(val_3 == 1);
    
    int * val = (int *)malloc(sizeof(int));
    *val = 5;
    q_enqueue(&q, val);
    
    atomic_store_explicit(&f_8, 1, memory_order_release);

    return NULL;
}

// deq(E)
void *thread_9(void *arg)
{
    set_thread_num(9);
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(*res == 5);
    
    
    atomic_store_explicit(&f_9, 1, memory_order_release);

    return NULL;
}


int main()
{
    q_init_queue(&q, 10);

    pthread_t t_0;
    if (pthread_create(&t_0, NULL, thread_0, NULL))
        abort();
        
    pthread_t t_1;
    if (pthread_create(&t_1, NULL, thread_1, NULL))
        abort();
        
    pthread_t t_2;
    if (pthread_create(&t_2, NULL, thread_2, NULL))
        abort();
        
    pthread_t t_3;
    if (pthread_create(&t_3, NULL, thread_3, NULL))
        abort();
        
    pthread_t t_8;
    if (pthread_create(&t_8, NULL, thread_8, NULL))
        abort();
        
    pthread_t t_9;
    if (pthread_create(&t_9, NULL, thread_9, NULL))
        abort();
        
    
    pthread_t t_10;
    if (pthread_create(&t_10, NULL, noise_enq_deq, create_args(&q, 10, 1)))
        abort();
        
    pthread_t t_11;
    if (pthread_create(&t_11, NULL, noise_enq_deq, create_args(&q, 11, 1)))
        abort();
        
    }