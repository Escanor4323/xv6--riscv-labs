#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READ_LOOPS 5
#define WRITE_LOOPS 5
#define NULL 0

typedef struct {
    int data;
    int readcount;
    int read_ops;
    int read_sum;
    int write_ops;
} shared_t;

static shared_t *shared;
static sem_t mutex;   // protects readcount updates
static sem_t rwlock;  // allows either multiple readers or one writer

static int num_readers = 1;
static int num_writers = 1;

static void
reader(void)
{
    int i;
    for (i = 0; i < READ_LOOPS; i++) {
        sem_wait(&mutex);
        shared->readcount++;
        if (shared->readcount == 1) {
            // first reader locks writers out
            sem_wait(&rwlock);
        }
        sem_post(&mutex);

        // critical section (shared reading allowed)
        shared->read_sum += shared->data;
        shared->read_ops++;

        sem_wait(&mutex);
        shared->readcount--;
        if (shared->readcount == 0) {
            // last reader releases writer access
            sem_post(&rwlock);
        }
        sem_post(&mutex);
    }
}

static void
writer(void)
{
    int i;
    for (i = 0; i < WRITE_LOOPS; i++) {
        sem_wait(&rwlock); // exclusive access
        shared->data++;
        shared->write_ops++;
        sem_post(&rwlock);
    }
}

int
main(int argc, char *argv[])
{
    int i, pid;

    if (argc >= 3) {
        num_readers = atoi(argv[1]);
        num_writers = atoi(argv[2]);
    }

    shared = (shared_t *) mmap(NULL, sizeof(shared_t),
                               PROT_READ | PROT_WRITE,
                               MAP_ANONYMOUS | MAP_SHARED,
                               -1, 0);
    shared->data = 0;
    shared->readcount = 0;
    shared->read_ops = 0;
    shared->read_sum = 0;
    shared->write_ops = 0;

    sem_init(&mutex, 0, 1);
    sem_init(&rwlock, 0, 1);

    // fork readers
    for (i = 0; i < num_readers; i++) {
        pid = fork();
        if (pid == 0) {
            reader();
            exit(0);
        }
    }

    // fork writers
    for (i = 0; i < num_writers; i++) {
        pid = fork();
        if (pid == 0) {
            writer();
            exit(0);
        }
    }

    for (i = 0; i < num_readers + num_writers; i++) {
        wait(0);
    }

    printf("readers=%d writers=%d\n", num_readers, num_writers);
    printf("final data=%d\n", shared->data);
    printf("read_ops=%d read_sum=%d\n", shared->read_ops, shared->read_sum);
    printf("write_ops=%d\n", shared->write_ops);

    sem_destroy(&mutex);
    sem_destroy(&rwlock);

    exit(0);
}
