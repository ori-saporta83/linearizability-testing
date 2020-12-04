#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../../../wrappers/queue-wrappers.h"

// ['deq(A)_enq(A)']
queue_t q;

atomic_int f_0;
atomic_int f_1;

// enq(A)
void *thread_0(void *arg)
{
    set_thread_num(0);
    
    int val_1 = 0;
    val_1 = atomic_load_explicit(&f_1, memory_order_acquire);
    __VERIFIER_assume(val_1 == 1);
    
    q_enqueue(&q, 1);
    
    atomic_store_explicit(&f_0, 1, memory_order_release);

    return NULL;
}

// deq(A)
void *thread_1(void *arg)
{
    set_thread_num(1);
    
    unsigned int res = 0;
    bool succ = q_dequeue(&q, &res);
    
    __VERIFIER_assume(succ);
    __VERIFIER_assume(res == 1);
    
    
    atomic_store_explicit(&f_1, 1, memory_order_release);

    return NULL;
}


int main()
{
    q_init_queue(&q, 2);

    pthread_t t_0;
    if (pthread_create(&t_0, NULL, thread_0, NULL))
        abort();
        
    pthread_t t_1;
    if (pthread_create(&t_1, NULL, thread_1, NULL))
        abort();
        
    
}