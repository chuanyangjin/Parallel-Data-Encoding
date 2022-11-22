// Not Your Usual ENCoder
// Data compression is the process of encoding information using fewer bits than the original representation. 
// Run-length encoding (RLE) is a simple yet effective compression algorithm: repeated data are stored as a single data and the count. 
// In this lab, I build a parallel run-length encoder called Not Your Usual ENCoder, or nyuenc for short. 
// If multiple files are passed to nyuenc, they will be concatenated and encoded into a single compressed output.
// It takes an optional command-line option -j jobs, which specifies the number of worker threads.
// (If no such option is provided, it runs sequentially.) 

#include "global.h"

int main(int argc, char *argv[]) {   
    // Parse command-line options
    int opt;
    int num_threads = -1;
    while((opt = getopt(argc, argv, "j:")) != -1) 
    { 
        switch(opt) 
        { 
            case 'j':;
                num_threads = atoi(optarg);
                break; 
        } 
    }

    // Run-length encoding for seperate files / chunks
    struct stat sb;
    int fd;
    unsigned char* addr;
    chunk** results = (chunk **) malloc(sizeof(chunk *) * (1024 * 1024 / 4));
    if (num_threads == -1) {
        // Sequential RLE
        for (int i = 0; i < argc - 1; i++){
            // Open file, get size, map address
            fd = open(argv[i + 1], O_RDONLY);
            stat(argv[i + 1], &sb);
            addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

            // Run-length encoding
            chunk* input = (chunk *) malloc(sizeof(chunk));;
            input -> string = addr;
            input -> size = sb.st_size;
            input -> position = i;
            results[i] = encode(input);
        }
    }
    else {
        // Parallel RLE
        threadpool_t *pool = create_pool(num_threads);
        pthread_mutex_lock(&(pool->lock));

        for (int i = 0; i < argc - 3; i++){
            // Open file, get size, map address
            fd = open(argv[i + 3], O_RDONLY);
            stat(argv[i + 3], &sb);
            addr = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

            for (int j = 0; j < sb.st_size; j += 4096){
                // Run-length encoding
                pool->task_queue[pool->queue_tail] = (chunk *) malloc(sizeof(chunk));
                pool->task_queue[pool->queue_tail]->string = addr + j;
                pool->task_queue[pool->queue_tail]->size = (sb.st_size - j < 4096) ? sb.st_size - j : 4096;
                pool->task_queue[pool->queue_tail]->position = pool->queue_size;
                pool->queue_size++;
                pool->queue_tail++;
            }
        }

        pool->task_queue[pool->queue_tail] = NULL;
        pthread_cond_signal(&(pool->queue_not_empty));

        // Create threads
        for (int i = 0; i < num_threads; i++)
        {
            pthread_create(&(pool->threads[i]), NULL, thread, (void *) pool);
        }

        // Wait until all tasks are finished
        while (pool -> queue_size != 0)
        {
            pthread_cond_wait(&(pool->queue_empty), &(pool->lock));
        }

        // Store results
        results = pool->results;

        // Release the mutex
        pthread_mutex_unlock(&(pool->lock));

        // Free the threadpool
        free_pool(pool);
    }
    
    // Concatenate and write the results
    int i = 0;
    while (results[i] != NULL) {
        if (results[i + 1] != NULL) {
            // Not last chunk
            write(1, results[i] -> string, results[i] -> size - 2);
            unsigned char last_char = results[i] -> string[results[i] -> size - 2];
            unsigned char first_char = results[i+1] -> string[0];

            if (last_char == first_char) {
                // Concatenate the same chars
                // first_count += last_count
                results[i + 1] -> string[1] += results[i] -> string[results[i] -> size - 1];
            }
            else {
                // Write the different chars
                write(1, results[i] -> string + results[i] -> size - 2, 2);
            }
        }
        else {
            // Last chunk
            write(1, results[i] -> string, results[i] -> size);
        }
        i++;
    }
}