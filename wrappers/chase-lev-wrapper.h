#include <assert.h>
#include <stdint.h>
#include <stdatomic.h>

#define MAX_SIZE 10

typedef int val_t;

struct Deque {
	atomic_uint_fast64_t bottom;
	atomic_uint_fast64_t top;
	val_t * _Atomic buffer[MAX_SIZE]; // in fact, it should be marked as atomic
	//due to the race between push and
	// steal.
};

int64_t try_push(struct Deque *deq, val_t * data)
{
	uint64_t b = atomic_load_explicit(&deq->bottom, memory_order_acquire);
	uint64_t t = atomic_load_explicit(&deq->top, memory_order_relaxed);

	int64_t len = (int64_t) (b - t);
	if (len >= MAX_SIZE) {
		return -1; // full
	}

	atomic_store_explicit(&deq->buffer[b % MAX_SIZE], data, memory_order_release);

	atomic_store_explicit(&deq->bottom, b + 1, memory_order_release);
	return 0;
}

int64_t try_steal(struct Deque *deq, val_t **data)
{
	uint64_t t = atomic_load_explicit(&deq->top, memory_order_relaxed);

	uint64_t b = atomic_load_explicit(&deq->bottom, memory_order_acquire);
	int64_t len = (int64_t) (b - t);

	if (len <= 0) {
		return -1; // empty
	}

	*data = atomic_load_explicit(&deq->buffer[t % MAX_SIZE], memory_order_acquire);

	bool is_successful = atomic_compare_exchange_strong_explicit(&deq->top, &t, t + 1,
							    memory_order_release,
							    memory_order_acquire);
	return (is_successful ? 0 : -2); // success or lost
}

typedef struct Deque queue_t;

void q_enqueue(queue_t *q, val_t *val) 
{
	int res = try_push(q, val);
    assert(res == 0);
}

bool q_dequeue(queue_t *q, val_t **retVal)
{
    int res = try_steal(q, retVal);
    return res == 0;
}

void q_init_queue(queue_t *q, int num_threads)
{
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