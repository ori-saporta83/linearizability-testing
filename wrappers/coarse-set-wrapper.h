#include <assert.h>
#include <pthread.h>

#ifndef MAX_THREADS
# define MAX_THREADS 32
#endif
#ifndef MAX_FREELIST
# define MAX_FREELIST 32 /* Each thread can own up to MAX_FREELIST free nodes */
#endif

int __thread tid;

void set_thread_num(int i)
{
	tid = i;
}

int get_thread_num()
{
	return tid;
}

#include "../../genmc/tests/correct/lapor/coarse-set/coarse-set.c"

typedef struct set_head set_t;

void init_set(set_t *set, int num_threads)
{
    set_init(set);
}

bool w_add(set_t *set, unsigned int val) 
{
	return add(set, val);
}

bool w_remove(set_t *set, unsigned int val)
{
    return remove(set, val);
}

bool w_in(set_t *set, unsigned int val)
{
    return contains(set, val);
}

pthread_t threads[MAX_THREADS + 1];
int param[MAX_THREADS + 1];
struct set_node free_lists[MAX_THREADS + 1][MAX_FREELIST];
unsigned int free_index[MAX_THREADS + 1];

struct set_node *new_node(int key, int elem)
{
	int t = get_thread_num();

	assert(free_index[t] < MAX_FREELIST);
	free_lists[t][free_index[t]].key = key;
	free_lists[t][free_index[t]].val = elem;
	return &free_lists[t][free_index[t]++];
}