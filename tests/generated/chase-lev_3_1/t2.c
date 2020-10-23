#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../../../wrappers/chase-lev-wrapper.h"
#include "../../genmc/include/genmc.h"

// ['enq(A)_deq(A)', 'enq(A)_deq(T-a)', 'deq(T-a)_deq(A)']
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

atomic_int f_0;
atomic_int f_1;
atomic_int f_2;

// enq(A)
void *thread_0(void *arg)
{
    set_thread_num(0);
    
    enqueue(&q, 1);
    
    atomic_store_explicit(&f_0, 1, memory_order_release);

    return NULL;
}

// deq(A)
void *thread_1(void *arg)
{
    set_thread_num(1);
    
    int val_0 = 0;
    val_0 = atomic_load_explicit(&f_0, memory_order_acquire);
    __VERIFIER_assume(val_0 == 1);
    
    int val_2 = 0;
    val_2 = atomic_load_explicit(&f_2, memory_order_acquire);
    __VERIFIER_assume(val_2 == 1);
    
    unsigned int res = 0;
    bool succ = dequeue(&q, &res);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(res == 1);
    
    
    atomic_store_explicit(&f_1, 1, memory_order_release);

    return NULL;
}

// deq(@)
void *thread_2(void *arg)
{
    set_thread_num(2);
    
    int val_0 = 0;
    val_0 = atomic_load_explicit(&f_0, memory_order_acquire);
    __VERIFIER_assume(val_0 == 1);
    
    unsigned int res = 0;
    bool succ = dequeue(&q, &res);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(res == 0);
    
    
    atomic_store_explicit(&f_2, 1, memory_order_release);

    return NULL;
}


int main()
{
    init_queue(&q, 3);

    pthread_t t_0;
    if (pthread_create(&t_0, NULL, thread_0, NULL))
        abort();
        
    pthread_t t_1;
    if (pthread_create(&t_1, NULL, thread_1, NULL))
        abort();
        
    pthread_t t_2;
    if (pthread_create(&t_2, NULL, thread_2, NULL))
        abort();
        
    
}