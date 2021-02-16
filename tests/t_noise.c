#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../wrappers/queue-wrappers.h"

#include "../../genmc/include/genmc.h"

queue_t q;

int main()
{
    q_init_queue(&q, 5);

    pthread_t t0, t1, t2, t3, t4;

    if (pthread_create(&t0, NULL, noise_deq, create_args(&q, 0, 1)))
        abort();

    if (pthread_create(&t1, NULL, noise_enq, create_args(&q, 1, 1)))
        abort();

    if (pthread_create(&t2, NULL, noise_deq, create_args(&q, 2, 1)))
        abort();

    if (pthread_create(&t3, NULL, noise_enq, create_args(&q, 3, 1)))
        abort();

    // if (pthread_create(&t4, NULL, noise_deq, create_args(&q, 4, 1)))
    //     abort();
}
