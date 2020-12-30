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
    init_set(&set, 3);

    pthread_t t0, t1, t2;

    if (pthread_create(&t0, NULL, noise_gen, create_args(&set, 0, 1, 9)))
        abort();

    if (pthread_create(&t1, NULL, noise_gen, create_args(&set, 1, 1, 9)))
        abort();

    // if (pthread_create(&t2, NULL, noise_gen, create_args(&set, 2, 1, 9)))
    //     abort();
}
