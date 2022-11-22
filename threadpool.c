#include "global.h"

// Create threadpool
threadpool_t *create_pool(int num_threads) {
    // Pool
    threadpool_t *pool = (threadpool_t *) malloc(sizeof(threadpool_t));

    // Mutex & conditional variables
    pthread_mutex_init(&(pool->lock), NULL);
    pthread_cond_init(&(pool->queue_not_empty), NULL);
    pthread_cond_init(&(pool->queue_empty), NULL);

    // Threads
    pool->threads = (pthread_t *) malloc(sizeof(pthread_t) * num_threads);
    pool->num_threads = num_threads;

    // Tasks
    pool->task_queue = (chunk **) malloc(sizeof(chunk *) * (1024 * 1024 / 4));
    pool->queue_head = 0;
    pool->queue_tail = 0;
    pool->queue_size = 0;

    // Results
    pool->results = (chunk **) malloc(sizeof(chunk *) * (1024 * 1024 / 4));
    pool->results_size = 0;

    return pool;
}

// Thread
void *thread(void *threadpool) {
    threadpool_t *pool = (threadpool_t *) threadpool;

    while (1) {
        pthread_mutex_lock(&(pool->lock));

        // Wait until there is task to do
        while (pool->queue_size == 0) {
            pthread_cond_wait(&(pool->queue_not_empty), &(pool->lock));
        }

        // Get task from the head of task queue
        chunk *input = pool->task_queue[pool->queue_head];
        pool->queue_head++;
        pool->queue_size--;

        // Encode the input
        pthread_mutex_unlock(&(pool->lock));
        chunk *output = encode(input);
        pthread_mutex_lock(&(pool->lock));

        // Store the output
        pool->results[output->position] = output;
        pool->results_size++;

        // If all tasks are finished
        if (pool->results_size == pool->queue_tail) {
            pool->results[pool->results_size] = NULL;
            pthread_cond_signal(&(pool->queue_empty));
        }
        pthread_mutex_unlock(&(pool->lock));
    }

    pthread_exit(NULL);
}

int free_pool(threadpool_t *pool)
{
    int i = 0;
    while (pool->task_queue[i] != NULL)
    {
        free(pool->task_queue[i++]);
    }
    free(pool->task_queue);

    return 1;
}