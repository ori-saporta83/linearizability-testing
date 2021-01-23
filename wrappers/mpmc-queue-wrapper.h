#include <unistd.h>
#include <inttypes.h>
#include <stdatomic.h>
#include "../../genmc/include/genmc.h"

#define t_size sizeof(int32_t)

typedef int32_t val_t;


typedef struct mpmc_boundq_1_alt
{

	// elements should generally be cache-line-size padded :
	val_t *	_Atomic	m_array[t_size];

	// rdwr counts the reads & writes that have started
	atomic_uint    	m_rdwr;
	// "read" and "written" count the number completed
	atomic_uint    	m_read;
	atomic_uint    	m_written;
} mpmc_boundq_1_alt;
// typedef struct mpmc_boundq_1_alt mpmc_boundq_1_alt;

#define MAX_ITER 3

val_t * _Atomic *read_fetch(mpmc_boundq_1_alt *q)
{
	unsigned int rdwr = atomic_load_explicit(&q->m_rdwr, memory_order_acquire);
	unsigned int rd,wr;

	int cnt = 0;
	for(;;) {
		__VERIFIER_assume(++cnt < MAX_ITER);

		rd = (rdwr >> 16) & 0xFFFF;
		wr = rdwr & 0xFFFF;

		if (wr == rd) // empty
			return NULL;

		if (atomic_compare_exchange_weak_explicit(&q->m_rdwr, &rdwr, rdwr + (1 << 16), memory_order_acq_rel, memory_order_acquire))
			break;
	}

	cnt = 0;
	while((atomic_load_explicit(&q->m_written, memory_order_acquire) & 0xFFFF) == wr) {
		__VERIFIER_assume(++cnt < MAX_ITER);
	};
	// while ((atomic_load_explicit(&q->m_written, mo_acquire) & 0xFFFF) != wr)
		; // thrd_yield();

	return &(q->m_array[rd % t_size]);
}

void read_consume(mpmc_boundq_1_alt *q)
{
	atomic_fetch_add_explicit(&q->m_read, 1, memory_order_acquire);
}

val_t * _Atomic *write_prepare(mpmc_boundq_1_alt *q)
{
	unsigned int rdwr = atomic_load_explicit(&q->m_rdwr, memory_order_acquire);
	unsigned int rd,wr;

	int cnt = 0;
	for(;;) {
		__VERIFIER_assume(++cnt < MAX_ITER);

		rd = (rdwr>>16) & 0xFFFF;
		wr = rdwr & 0xFFFF;

		if (wr == ((rd + t_size) & 0xFFFF)) // full
			return NULL;

		if (atomic_compare_exchange_weak_explicit(&q->m_rdwr, &rdwr, (rd << 16) | ((wr + 1) & 0xFFFF),
							  memory_order_acq_rel, memory_order_acq_rel))
			break;
	}

	cnt = 0;
	if ((atomic_load_explicit(&q->m_read, memory_order_acquire) & 0xFFFF) == rd) {
		__VERIFIER_assume(++cnt < MAX_ITER);
	};
	// while ((atomic_load_explicit(&q->m_read, mo_acquire) & 0xFFFF) != rd )
		; // thrd_yield()

	return &(q->m_array[wr % t_size]);
}

void write_publish(mpmc_boundq_1_alt *q)
{
	atomic_fetch_add_explicit(&q->m_written, 1, memory_order_release);
}

typedef mpmc_boundq_1_alt queue_t;

void q_enqueue(queue_t *q, val_t * val) 
{
	val_t * _Atomic * bin = write_prepare(q);
	assert(bin != NULL);
	atomic_store_explicit(bin, val, memory_order_relaxed);
	write_publish(q);
}

bool q_dequeue(queue_t *q, val_t **retVal)
{
    val_t * _Atomic * bin = read_fetch(q);
    if (bin != NULL) {
        *retVal = atomic_load_explicit(bin, memory_order_relaxed);
		read_consume(q);
		return true;
    }
	return false;
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
