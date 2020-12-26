#include <assert.h>
#include <pthread.h>
#include <stdatomic.h>

#ifndef CORE_NUM
# define CORE_NUM 8
#endif
#ifndef DEFAULT
# define DEFAULT
#endif
#ifndef _SC_NPROCESSORS_ONLN
# define _SC_NPROCESSORS_ONLN 8
#endif

// #  define CAS_PTR(a,b,c) atomic_compare_exchange_weak(a,b,c)
#  define CAS_PTR(a,b,c) __sync_val_compare_and_swap(a,b,c)

#include "../../ASCYLIB/src/bst-howley/bst_howley.c"

int __thread tid;
void set_thread_num(int i)
{
	tid = i;
}

int get_thread_num()
{
	return tid;
}

typedef struct node_t set_t;

void init_set(set_t *set, int num_threads)
{
    set = bst_initialize();
}

bool w_add(set_t *set, unsigned int val) 
{
	return bst_add((intptr_t) val, (intptr_t) val, set);
}

bool w_remove(set_t *set, unsigned int val)
{
    return bst_remove((intptr_t) val, set);
}

bool w_in(set_t *set, unsigned int val)
{
    return bst_contains((intptr_t) val, set);
}