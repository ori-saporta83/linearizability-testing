#include <unistd.h>
#include <inttypes.h>
#include <stdatomic.h>
#include "../../genmc/include/genmc.h"

#define t_size sizeof(int32_t)

typedef int32_t val_t;


typedef struct mpmc_boundq_1_alt
{

	// elements should generally be cache-line-size padded :
	val_t *	m_array[t_size];

	// rdwr counts the reads & writes that have started
	atomic_uint    	m_rdwr;
	// "read" and "written" count the number completed
	atomic_uint    	m_read;
	atomic_uint    	m_written;
} mpmc_boundq_1_alt;
// typedef struct mpmc_boundq_1_alt mpmc_boundq_1_alt;

#define MAX_ITER 3

bool deq(mpmc_boundq_1_alt *q, val_t ** retVal)
{
	unsigned int rdwr = atomic_load_explicit(&q->m_rdwr, memory_order_acquire);
	unsigned int rd, wr; 

	int cnt = 0;
	while(true) {
		__VERIFIER_assume(++cnt < MAX_ITER);

		rd = (rdwr >> 16) & 0xFFFF;
		wr = rdwr & 0xFFFF;

		if (wr == rd) // empty
			return false;

		if (atomic_compare_exchange_weak_explicit(&q->m_rdwr, &rdwr, rdwr + (1 << 16), memory_order_acq_rel, memory_order_acquire))
			break;
	}

	__VERIFIER_assume((atomic_load_explicit(&q->m_written, memory_order_acquire) & 0xFFFF) == wr);

	*retVal = (q->m_array[rd % t_size]);
	
	atomic_fetch_add_explicit(&q->m_read, 1, memory_order_release);

	return true;
}

void enq(mpmc_boundq_1_alt *q, val_t * val)
{
	unsigned int rdwr = atomic_load_explicit(&q->m_rdwr, memory_order_acquire);
	unsigned int rd,wr;

	int cnt = 0;
	while(true) {
		__VERIFIER_assume(++cnt < MAX_ITER);

		rd = (rdwr>>16) & 0xFFFF;
		wr = rdwr & 0xFFFF;

		if (wr == ((rd + t_size) & 0xFFFF)) // full
			break;

		if (atomic_compare_exchange_weak_explicit(&q->m_rdwr, &rdwr, (rd << 16) | ((wr + 1) & 0xFFFF),
							  memory_order_acq_rel, memory_order_acq_rel))
			break;
	}

	__VERIFIER_assume((atomic_load_explicit(&q->m_read, memory_order_acquire) & 0xFFFF) == rd);

	(q->m_array[wr % t_size]) = val;

	atomic_fetch_add_explicit(&q->m_written, 1, memory_order_release);
}

typedef mpmc_boundq_1_alt queue_t;

void q_enqueue(queue_t *q, val_t * val) 
{
	enq(q, val);
}

bool q_dequeue(queue_t *q, val_t **retVal)
{
    return deq(q, retVal);
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
