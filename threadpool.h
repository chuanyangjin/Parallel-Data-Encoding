#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "global.h"

// Threadpool
typedef struct {
    // Mutex & conditional variables
    pthread_mutex_t lock;
    pthread_cond_t queue_not_empty;
    pthread_cond_t queue_empty;

    // Threads
    pthread_t *threads;
    int num_threads;

    // Tasks
    chunk **task_queue;
    int queue_head;
    int queue_tail;
    int queue_size;

    // Results
    chunk **results;
    int results_size;
} threadpool_t;

threadpool_t *create_pool(int num_threads);
void *thread(void *pool);
int free_pool(threadpool_t *pool);

#endif