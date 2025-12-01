#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"

#define BSIZE 10
#define MAX 10
#define NULL 0

typedef struct {
    int buf[BSIZE];
    int nextin;
    int nextout;
    int num_produced;
    int num_consumed;
    int total;
} buffer_t;

buffer_t *buffer;
sem_t empty;
sem_t full;
sem_t mutex;

void producer()
{
    int i;
    for (i = 0; i < MAX; i++) {
        sem_wait(&empty);
        sem_wait(&mutex);
        
        buffer->buf[buffer->nextin++] = i + 1;
        buffer->nextin %= BSIZE;
        buffer->num_produced++;
        
        sem_post(&mutex);
        sem_post(&full);
    }
}

void consumer()
{
    int i;
    for (i = 0; i < MAX; i++) {
        sem_wait(&full);
        sem_wait(&mutex);
        
        buffer->total += buffer->buf[buffer->nextout++];
        buffer->nextout %= BSIZE;
        buffer->num_consumed++;
        
        sem_post(&mutex);
        sem_post(&empty);
    }
}

int
main(int argc, char *argv[])
{
    buffer = (buffer_t *) mmap(NULL, sizeof(buffer_t),
                               PROT_READ | PROT_WRITE,
                               MAP_ANONYMOUS | MAP_PRIVATE,
                               -1, 0);
    buffer->nextin = 0;
    buffer->nextout = 0;
    buffer->num_produced = 0;
    buffer->num_consumed = 0;
    buffer->total = 0;

    // Initialize semaphores
    sem_init(&empty, 0, BSIZE);  // empty slots = buffer size
    sem_init(&full, 0, 0);        // full slots = 0 initially
    sem_init(&mutex, 0, 1);       // mutex for critical section

    if (fork() == 0) {
        // Child process: producer
        producer();
        exit(0);
    } else {
        // Parent process: consumer
        consumer();
        wait(0);
    }

    printf("total = %d\n", buffer->total);
    printf("num_produced = %d\n", buffer->num_produced);
    printf("num_consumed = %d\n", buffer->num_consumed);

    // Cleanup semaphores
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    exit(0);
}

