#define SCQ
#include "../../lfqueue/lfring_cas1.h"

#define SCQ_ORDER 15
#define EMPTY (void *)LFRING_EMPTY
#define PAGE_SIZE 4096
#define CACHE_LINE_SIZE 64
#define CACHE_ALIGNED __attribute__((aligned(CACHE_LINE_SIZE)))
#define DOUBLE_CACHE_ALIGNED __attribute__((aligned(2 * CACHE_LINE_SIZE)))

typedef struct _queue_t
{
    char ring[LFRING_SIZE(SCQ_ORDER)];
} queue_t DOUBLE_CACHE_ALIGNED;

void enqueue(queue_t *q, unsigned int val)
{
    size_t eidx = (size_t)val;
    lfring_enqueue((struct lfring *)q->ring, SCQ_ORDER, eidx, false);
}

bool dequeue(queue_t *q, unsigned int *retVal)
{
    *retVal = lfring_dequeue((struct lfring *) q->ring, SCQ_ORDER, false);
    return true;
}

void init_queue(queue_t *q, int num_threads)
{
    lfring_init_empty((struct lfring *)q->ring, SCQ_ORDER);
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
