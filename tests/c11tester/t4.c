#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "hwqueue-wrapper.h"

// ["'enq(A)_enq(B)'", "'deq(B)_enq(D)'", "'deq(D)_deq(E)'", "'enq(E)_deq(A)'"]
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
    
    int val_8 = 0;
    while (val_8 != 1) {
        val_8 = atomic_load_explicit(&f_8, memory_order_acquire);
    }
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res);
    
    // assert(succ);
    
    atomic_store_explicit(&f_1, 1, memory_order_release);

    return (void *) res;
}

// enq(B)
void *thread_2(void *arg)
{
    set_thread_num(2);
    
    int val_0 = 0;
    while (val_0 != 1) {
        val_0 = atomic_load_explicit(&f_0, memory_order_acquire);
    }
    
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
    
    // assert(succ);
    
    atomic_store_explicit(&f_3, 1, memory_order_release);

    return (void *) res;
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
    
    // assert(succ);  
    
    atomic_store_explicit(&f_5, 1, memory_order_release);

    return (void *) res;
}

// enq(D)
void *thread_6(void *arg)
{
    set_thread_num(6);
    
    int val_3 = 0;
    while (val_3 != 1) {
        val_3 = atomic_load_explicit(&f_3, memory_order_acquire);
    }
    
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
    
    // assert(succ);
    
    atomic_store_explicit(&f_7, 1, memory_order_release);

    return (void *) res;
}

// enq(E)
void *thread_8(void *arg)
{
    set_thread_num(8);
    
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
    
    int val_7 = 0;
    while (val_7 != 1) {
        val_7 = atomic_load_explicit(&f_7, memory_order_acquire);
    }
    
    int * res = NULL;
    bool succ = q_dequeue(&q, &res);
    
    // assert(succ);
    
    atomic_store_explicit(&f_9, 1, memory_order_release);

    return (void *) res;
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
        
    pthread_t t_6;
    if (pthread_create(&t_6, NULL, thread_6, NULL))
        abort();
        
    pthread_t t_7;
    if (pthread_create(&t_7, NULL, thread_7, NULL))
        abort();
        
    pthread_t t_8;
    if (pthread_create(&t_8, NULL, thread_8, NULL))
        abort();
        
    pthread_t t_9;
    if (pthread_create(&t_9, NULL, thread_9, NULL))
        abort();
        
    int *rv1, *rv3, *rv7, *rv9 = 0;
    pthread_join(t_0, NULL);
    pthread_join(t_1, (void *) &rv1);
    pthread_join(t_2, NULL);
    pthread_join(t_3, (void *) &rv3);
    pthread_join(t_6, NULL);
    pthread_join(t_7, (void *) &rv7);
    pthread_join(t_8, NULL);
    pthread_join(t_9, (void *) &rv9);

    printf("rv1: %d, rv3: %d, rv7: %d, rv9: %d\n", rv1 != NULL ? *rv1 : 0, rv3 != NULL ? *rv3 : 0, rv7 != NULL ? *rv7 : 0, rv9 != NULL ? *rv9 : 0);
    // assert((*rv1 == 1) && (*rv3 == 2) && (*rv7 == 4) && (*rv9 == 5));
}