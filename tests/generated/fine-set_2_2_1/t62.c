#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../../../wrappers/fine-set-wrapper.h"
#include "../../genmc/include/genmc.h"

// ["'add(A)_in(A)-f-0'", "'add(A)_in(B)-t-0'", "'in(A)-t-1_remove(A)'", "'in(A)-t-1_in(B)-f-0'", "'in(A)-f-0_in(B)-t-0'", "'in(B)-t-1_add(A)'", "'in(B)-t-1_in(A)-f-0'", "'in(B)-t-1_in(B)-t-0'", "'in(B)-f-0_remove(A)'"]
set_t set;

int __thread tid;

void set_thread_num(int i)
{
	tid = i;
}

int get_thread_num()
{
	return tid;
}

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

// add(A)
void *thread_0(void *arg)
{
    set_thread_num(0);
    
    int val_8 = 0;
    val_8 = atomic_load_explicit(&f_8, memory_order_acquire);
    __VERIFIER_assume(val_8 == 1);
    
    bool succ = w_add(&set, 0);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_0, 1, memory_order_release);

    return NULL;
}

// remove(A)
void *thread_1(void *arg)
{
    set_thread_num(1);
    
    int val_3 = 0;
    val_3 = atomic_load_explicit(&f_3, memory_order_acquire);
    __VERIFIER_assume(val_3 == 1);
    
    int val_9 = 0;
    val_9 = atomic_load_explicit(&f_9, memory_order_acquire);
    __VERIFIER_assume(val_9 == 1);
    
    bool succ = w_remove(&set, 0);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_1, 1, memory_order_release);

    return NULL;
}

// in(A)-t-0
void *thread_2(void *arg)
{
    set_thread_num(2);
    
    bool succ = w_in(&set, 0);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_2, 1, memory_order_release);

    return NULL;
}

// in(A)-t-1
void *thread_3(void *arg)
{
    set_thread_num(3);
    
    bool succ = w_in(&set, 0);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_3, 1, memory_order_release);

    return NULL;
}

// in(A)-f-0
void *thread_4(void *arg)
{
    set_thread_num(4);
    
    int val_0 = 0;
    val_0 = atomic_load_explicit(&f_0, memory_order_acquire);
    __VERIFIER_assume(val_0 == 1);
    
    int val_8 = 0;
    val_8 = atomic_load_explicit(&f_8, memory_order_acquire);
    __VERIFIER_assume(val_8 == 1);
    
    bool succ = w_in(&set, 0);
    
    
    __VERIFIER_assume(!succ);
    
    atomic_store_explicit(&f_4, 1, memory_order_release);

    return NULL;
}

// add(B)
void *thread_5(void *arg)
{
    set_thread_num(5);
    
    bool succ = w_add(&set, 1);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_5, 1, memory_order_release);

    return NULL;
}

// remove(B)
void *thread_6(void *arg)
{
    set_thread_num(6);
    
    bool succ = w_remove(&set, 1);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_6, 1, memory_order_release);

    return NULL;
}

// in(B)-t-0
void *thread_7(void *arg)
{
    set_thread_num(7);
    
    int val_0 = 0;
    val_0 = atomic_load_explicit(&f_0, memory_order_acquire);
    __VERIFIER_assume(val_0 == 1);
    
    int val_4 = 0;
    val_4 = atomic_load_explicit(&f_4, memory_order_acquire);
    __VERIFIER_assume(val_4 == 1);
    
    int val_8 = 0;
    val_8 = atomic_load_explicit(&f_8, memory_order_acquire);
    __VERIFIER_assume(val_8 == 1);
    
    bool succ = w_in(&set, 1);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_7, 1, memory_order_release);

    return NULL;
}

// in(B)-t-1
void *thread_8(void *arg)
{
    set_thread_num(8);
    
    bool succ = w_in(&set, 1);
    
    __VERIFIER_assume(succ);
    
    atomic_store_explicit(&f_8, 1, memory_order_release);

    return NULL;
}

// in(B)-f-0
void *thread_9(void *arg)
{
    set_thread_num(9);
    
    int val_3 = 0;
    val_3 = atomic_load_explicit(&f_3, memory_order_acquire);
    __VERIFIER_assume(val_3 == 1);
    
    bool succ = w_in(&set, 1);
    
    
    __VERIFIER_assume(!succ);
    
    atomic_store_explicit(&f_9, 1, memory_order_release);

    return NULL;
}


int main()
{
    init_set(&set, 10);

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
        
    pthread_t t_9;
    if (pthread_create(&t_9, NULL, thread_9, NULL))
        abort();
        
    
}