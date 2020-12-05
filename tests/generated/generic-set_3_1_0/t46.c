#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../../../wrappers/set-wrappers.h"

// ["'remove(A)_add(B)'", "'remove(B)_in(A)-t-0'"]
set_t set;

atomic_int f_0;
atomic_int f_1;
atomic_int f_2;
atomic_int f_3;
atomic_int f_4;
atomic_int f_5;
atomic_int f_6;
atomic_int f_7;
atomic_int f_8;

// add(A)
void *thread_0(void *arg)
{
    set_thread_num(0);
    
    bool succ = w_add(&set, 0);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_0, 1, memory_order_release);

    return NULL;
}

// remove(A)
void *thread_1(void *arg)
{
    set_thread_num(1);
    
    bool succ = w_remove(&set, 0);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_1, 1, memory_order_release);

    return NULL;
}

// in(A)-t-0
void *thread_2(void *arg)
{
    set_thread_num(2);
    
    int val_4 = 0;
    val_4 = atomic_load_explicit(&f_4, memory_order_acquire);
    __VERIFIER_assume(val_4 == 1);
    
    bool succ = w_in(&set, 0);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_2, 1, memory_order_release);

    return NULL;
}

// add(B)
void *thread_3(void *arg)
{
    set_thread_num(3);
    
    int val_1 = 0;
    val_1 = atomic_load_explicit(&f_1, memory_order_acquire);
    __VERIFIER_assume(val_1 == 1);
    
    bool succ = w_add(&set, 1);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_3, 1, memory_order_release);

    return NULL;
}

// remove(B)
void *thread_4(void *arg)
{
    set_thread_num(4);
    
    bool succ = w_remove(&set, 1);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_4, 1, memory_order_release);

    return NULL;
}

// in(B)-t-0
void *thread_5(void *arg)
{
    set_thread_num(5);
    
    bool succ = w_in(&set, 1);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_5, 1, memory_order_release);

    return NULL;
}

// add(C)
void *thread_6(void *arg)
{
    set_thread_num(6);
    
    bool succ = w_add(&set, 2);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_6, 1, memory_order_release);

    return NULL;
}

// remove(C)
void *thread_7(void *arg)
{
    set_thread_num(7);
    
    bool succ = w_remove(&set, 2);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_7, 1, memory_order_release);

    return NULL;
}

// in(C)-t-0
void *thread_8(void *arg)
{
    set_thread_num(8);
    
    bool succ = w_in(&set, 2);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_8, 1, memory_order_release);

    return NULL;
}


int main()
{
    init_set(&set, 9);

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
        
    pthread_t t_5;
    if (pthread_create(&t_5, NULL, thread_5, NULL))
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
        
    
}