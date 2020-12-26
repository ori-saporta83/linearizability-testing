#include "coarse-set-wrapper.h"
// #include "fine-set-wrapper.h"
// #include "howley-wrapper.h"
#include "../../genmc/include/genmc.h"

typedef struct noise_args {
    set_t *s;
    int tid;
    int count;
    int ops;
} noise_args;

noise_args * create_args(set_t *s, int tid, int count, int ops) {
    noise_args *args = malloc(sizeof(noise_args));
    args->s = s;
    args->tid = tid;
    args->count = count;
    args->ops = ops;
    return args;
}

void *noise_gen(void *arg)
{
    noise_args* args = (noise_args*) arg;
    set_thread_num(args->tid);
    
    int base_val = (1 + args->tid - args->ops) * 11;

    for (int i = 0; i < args->count; ++i) {
        w_add(args->s, base_val + i);
    }

    for (int i = 0; i < args->count; ++i) {
        w_remove(args->s, base_val + i);
    }
    
    free(args);
    
    return NULL;
}
