#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <assert.h>
#include <stdatomic.h>

#include "../wrappers/set-wrappers.h"

#include "../../genmc/include/genmc.h"

set_t set;

int main()
{
    init_set(&set, 6);

    pthread_t t0, t1, t2, t3, t4, t5;

    if (pthread_create(&t0, NULL, noise_add, create_args(&set, 0, 1, 0)))
        abort();

    if (pthread_create(&t1, NULL, noise_remove, create_args(&set, 1, 1, 0)))
        abort();

    #ifdef ADD
    if (pthread_create(&t2, NULL, noise_add, create_args(&set, 2, 1, 0)))
        abort();
    #endif
    #ifdef REM
    if (pthread_create(&t3, NULL, noise_remove, create_args(&set, 3, 1, 0)))
        abort();
    #endif
    #ifdef IN
    if (pthread_create(&t4, NULL, noise_in, create_args(&set, 4, 1, 0)))
        abort();
    if (pthread_create(&t5, NULL, noise_in, create_args(&set, 5, 1, 0)))
        abort();
    #endif
}
