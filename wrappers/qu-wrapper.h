#include <assert.h>
#include "../../genmc/include/genmc.h"

int get_thread_num();
struct queue_node *new_node();


/***********************************************************
 * Queue implementation (utilizes malloc infrastructure)
 ***********************************************************/

typedef int val_t;

typedef struct queue_node {
	val_t * data;
	struct queue_node * _Atomic next;
} queue_node;

typedef struct queue {
	queue_node init;
	queue_node * _Atomic head;
	queue_node * _Atomic tail;
	atomic_bool is_initialized;
} queue;

void queue_init(struct queue *q)
{
	atomic_store_explicit(&q->init.next, NULL, memory_order_relaxed);
	atomic_store_explicit(&q->head, &q->init, memory_order_relaxed);
	atomic_store_explicit(&q->tail, &q->init, memory_order_relaxed);
	atomic_store_explicit(&q->is_initialized, true, memory_order_relaxed);
}

bool queue_is_initialized(struct queue *q)
{
	return atomic_load_explicit(&q->is_initialized, memory_order_relaxed);
}

queue_node *queue_find_tail(queue *q)
{
	queue_node *node = atomic_load_explicit(&q->tail, memory_order_relaxed);
	queue_node *next = atomic_load_explicit(&node->next, memory_order_relaxed);

	if (next == NULL)
		return node;

	atomic_store_explicit(&q->tail, next, memory_order_relaxed);
	return NULL;
}

#define MAX_ITER 3

int queue_try_enq(queue *q, val_t * data)
{
	queue_node *node = new_node();

	if (node == NULL) {
		return -3; /* OOM */
	}

	node->data = data;
	atomic_store_explicit(&node->next, NULL, memory_order_relaxed);

	queue_node *tail = NULL;
    int cnt = 0;
	do {
        __VERIFIER_assume(++cnt < MAX_ITER);
		tail = queue_find_tail(q);
	} while (tail == NULL);

	queue_node *v = NULL;
	if (atomic_compare_exchange_strong_explicit(&tail->next, &v, node,
						    memory_order_release,
						    memory_order_acquire)) {
		atomic_store_explicit(&q->tail, node, memory_order_relaxed);
		return 0;
	}

	return -2; /* CAVEAT: memory leak */
}

int queue_try_deq(queue *q, val_t  **ret_data)
{
	queue_node *head = atomic_load_explicit(&q->head, memory_order_relaxed);
	queue_node *node = atomic_load_explicit(&head->next, memory_order_acquire);

	if (node == NULL)
		return -1;

	if (atomic_compare_exchange_strong_explicit(&q->head, &head, node,
						    memory_order_relaxed,
						    memory_order_relaxed)) {
		*ret_data = node->data;
		return 0; /* CAVEAT: memory leak */
	}

	return -2;
}

typedef queue queue_t;

void q_enqueue(queue_t *q, val_t * val) 
{
    int ret = queue_try_enq(q, val);
    // assert(ret == 0);
}

bool q_dequeue(queue_t *q, val_t **retVal)
{
    int ret = queue_try_deq(q, retVal);
    return ret == 0;
}

void q_init_queue(queue_t *q, int num_threads)
{
    queue_init(q);
}

#ifndef MAX_THREADS
# define MAX_THREADS 32
#endif
#ifndef MAX_FREELIST
# define MAX_FREELIST 32 /* Each thread can own up to MAX_FREELIST free nodes */
#endif
unsigned int idx[MAX_THREADS + 1];
queue_node free_lists[MAX_THREADS + 1][MAX_FREELIST];
unsigned int free_index[MAX_THREADS + 1];
atomic_uint debug;

queue_node *new_node()
{
	int t = get_thread_num();
    unsigned int free_val = free_index[t];

    atomic_store_explicit(&debug, free_val, memory_order_release);
	assert(free_val < MAX_FREELIST);
	return &free_lists[t][free_index[t]++];
}

int __thread tid;

void set_thread_num(int i)
{
	tid = i;
}

int get_thread_num()
{
	return tid;
}