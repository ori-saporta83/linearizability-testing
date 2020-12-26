#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../../../wrappers/queue-wrappers.h"

// ["'deq(@)'"]
queue_t q;

int main()
{
    q_init_queue(&q, 10);
  
    pthread_t t_10;
    if (pthread_create(&t_10, NULL, noise_deq, create_args(&q, 1, 1)))
        abort();    
}