#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../wrappers/mpmc-queue-wrapper.h"
#include "../../genmc/include/genmc.h"

// ['enq(A)_deq(A)', 'enq(A)_enq(B)', 'deq(A)_enq(B)', 'deq(D)_enq(D)']
queue_t q;

atomic_int f_0;
atomic_int f_1;
atomic_int f_2;
atomic_int f_3;
atomic_int f_4;
atomic_int f_5;
atomic_int f_6;
atomic_int f_7;

// enq(A)
void *thread_0(void *arg)
{
    set_thread_num(0);
    
    enqueue(&q, 1);
    
    atomic_store_explicit(&f_0, 1, memory_order_release);

    unsigned int res = 0;
    bool succ = dequeue(&q, &res);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(res == 1);
      
    atomic_store_explicit(&f_1, 1, memory_order_release);

    enqueue(&q, 2);
    
    atomic_store_explicit(&f_2, 1, memory_order_release);

    return NULL;
}

// deq(B)
void *thread_1(void *arg)
{
    set_thread_num(1);
    
    unsigned int res = 0;
    bool succ = dequeue(&q, &res);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(res == 2);
    
    
    atomic_store_explicit(&f_3, 1, memory_order_release);

    return NULL;
}

// enq(C)
void *thread_2(void *arg)
{
    set_thread_num(2);
    
    enqueue(&q, 3);
    
    atomic_store_explicit(&f_4, 1, memory_order_release);

    return NULL;
}

// deq(C)
void *thread_3(void *arg)
{
    set_thread_num(3);
    
    unsigned int res = 0;
    bool succ = dequeue(&q, &res);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(res == 3);
    
    
    atomic_store_explicit(&f_5, 1, memory_order_release);

    return NULL;
}

// enq(D)
void *thread_4(void *arg)
{
    set_thread_num(4);

    unsigned int res = 0;
    bool succ = dequeue(&q, &res);

    atomic_store_explicit(&f_6, 1, memory_order_release);

    __VERIFIER_assume(succ);
    __VERIFIER_assume(res == 4);
    
    enqueue(&q, 4);

    atomic_store_explicit(&f_7, 1, memory_order_release);    

    return NULL;
}

int main()
{
    init_queue(&q, 5);

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
        
    pthread_t t_4;
    if (pthread_create(&t_4, NULL, thread_4, NULL))
        abort();       
    
}