#include <stdatomic.h>
#include "../../genmc/include/genmc.h"

#define MAX_NODES			63

typedef unsigned long long pointer;
typedef atomic_ullong pointer_t;

#define MAKE_POINTER(ptr, count)	((((pointer)count) << 32) | ptr)
#define PTR_MASK 0xffffffffLL
#define COUNT_MASK (0xffffffffLL << 32)

// static inline void set_count(pointer *p, unsigned int val) { *p = (*p & ~COUNT_MASK) | ((pointer)val << 32); }
static inline void set_ptr(pointer *p, unsigned int val) { *p = (*p & ~PTR_MASK) | val; }
static inline unsigned int get_count(pointer p) { return (p & COUNT_MASK) >> 32; }
static inline unsigned int get_ptr(pointer p) { return p & PTR_MASK; }

typedef int val_t;

typedef struct node {
	val_t * value;
	pointer_t next;
} node_t;

typedef struct {
	pointer_t head;
	pointer_t tail;
	node_t nodes[MAX_NODES + 1];
} queue_t;

void init_queue(queue_t *q, int num_threads);
int get_thread_num();

#ifndef MAX_THREADS
# define MAX_THREADS 32
#endif

#define MAX_FREELIST 4 /* Each thread can own up to MAX_FREELIST free nodes */
#define INITIAL_FREE 2 /* Each thread starts with INITIAL_FREE free nodes */

#define POISON_IDX 0x666

static unsigned int free_lists[MAX_THREADS][MAX_FREELIST];

/* Search this thread's free list for a "new" node */
static unsigned int new_node()
{
	int i;
	int t = get_thread_num();
	for (i = 0; i < MAX_FREELIST; i++) {
		unsigned int node = free_lists[t][i];
		if (node) {
			free_lists[t][i] = 0;
			return node;
		}
	}
	/* free_list is empty? */
	assert(0);
	return 0;
}

/* Place this node index back on this thread's free list */
static void reclaim(unsigned int node)
{
	int i;
	int t = get_thread_num();

	/* Don't reclaim NULL node */
	// if (!node) return;
	// assert(node);

	for (i = 0; i < MAX_FREELIST; i++) {
		/* Should never race with our own thread here */
		unsigned int idx = free_lists[t][i];

		/* Found empty spot in free list */
		if (idx == 0) {
			free_lists[t][i] = node;
			return;
		}
	}
	/* free list is full? */
	assert(0);
}

void init_queue(queue_t *q, int num_threads)
{
	int i, j;

	/* Initialize each thread's free list with INITIAL_FREE pointers */
	/* The actual nodes are initialized with poison indexes */
	for (i = 0; i < num_threads; i++) {
		for (j = 0; j < INITIAL_FREE; j++) {
			free_lists[i][j] = 2 + i * MAX_FREELIST + j;
			atomic_init(&q->nodes[free_lists[i][j]].next, MAKE_POINTER(POISON_IDX, 0));
		}
	}

	/* initialize queue */
	atomic_init(&q->head, MAKE_POINTER(1, 0));
	atomic_init(&q->tail, MAKE_POINTER(1, 0));
	atomic_init(&q->nodes[1].next, MAKE_POINTER(0, 0));
}

#define MAX_ITER 3

void enqueue(queue_t *q, val_t * val)
{
	int success = 0;
	unsigned int node;
	pointer tail;
	pointer next;
	pointer tmp;

	node = new_node();
	q->nodes[node].value = val;
	tmp = atomic_load_explicit(&q->nodes[node].next, memory_order_acquire);
	set_ptr(&tmp, 0); // NULL
	atomic_store_explicit(&q->nodes[node].next, tmp, memory_order_release);
	int cnt = 0;
	while (!success) {
		__VERIFIER_assume(++cnt < MAX_ITER);
		tail = atomic_load_explicit(&q->tail, memory_order_acquire);
		next = atomic_load_explicit(&q->nodes[get_ptr(tail)].next, memory_order_acquire);
		if (tail == atomic_load_explicit(&q->tail, memory_order_acquire)) {

			/* Check for uninitialized 'next' */
			assert(get_ptr(next) != POISON_IDX);

			if (get_ptr(next) == 0) { // == NULL
				pointer value = MAKE_POINTER(node, get_count(next) + 1);
				success = atomic_compare_exchange_strong_explicit(&q->nodes[get_ptr(tail)].next,
						&next, value, memory_order_acq_rel, memory_order_acquire);
			}
			if (!success) {
				unsigned int ptr = get_ptr(atomic_load_explicit(&q->nodes[get_ptr(tail)].next, memory_order_acquire));
				pointer value = MAKE_POINTER(ptr,
						get_count(tail) + 1);
				atomic_compare_exchange_strong_explicit(&q->tail,
						&tail, value,
						memory_order_acq_rel, memory_order_acquire);
//				thrd_yield();
			}
		}
	}
	atomic_compare_exchange_strong_explicit(&q->tail,
			&tail,
			MAKE_POINTER(node, get_count(tail) + 1),
			memory_order_acq_rel, memory_order_acquire);
}

bool dequeue(queue_t *q, val_t **retVal)
{
	int success = 0;
	pointer head;
	pointer tail;
	pointer next;

    int cnt = 0;
	while (!success) {
        __VERIFIER_assume(++cnt < MAX_ITER);
		head = atomic_load_explicit(&q->head, memory_order_acquire);
		tail = atomic_load_explicit(&q->tail, memory_order_acquire);
		next = atomic_load_explicit(&q->nodes[get_ptr(head)].next, memory_order_acquire);
		if (atomic_load_explicit(&q->head, memory_order_acquire) == head) {
			if (get_ptr(head) == get_ptr(tail)) {

				/* Check for uninitialized 'next' */
				assert(get_ptr(next) != POISON_IDX);

				if (get_ptr(next) == 0) { // NULL
					return false; // NULL
				}
				atomic_compare_exchange_strong_explicit(&q->tail,
						&tail,
						MAKE_POINTER(get_ptr(next), get_count(tail) + 1),
						memory_order_acq_rel, memory_order_acquire);
//				thrd_yield();
			} else {
				*retVal = q->nodes[get_ptr(next)].value;
				success = atomic_compare_exchange_strong_explicit(&q->head,
						&head,
						MAKE_POINTER(get_ptr(next), get_count(head) + 1),
						memory_order_acq_rel, memory_order_acquire);
				/* if (!success) */
				/* 	;//					thrd_yield(); */
			}
		}
	}
	reclaim(get_ptr(head));
	return true;
}

// typedef  queue_t;

void q_enqueue(queue_t *q, val_t * val) 
{
    enqueue(q, val);
}

bool q_dequeue(queue_t *q, val_t **retVal)
{
    return dequeue(q, retVal);
}

void q_init_queue(queue_t *q, int num_threads)
{
    init_queue(q, num_threads);
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
