#include "shmlib.h"
#include "semaphores.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define SEM_CON "/konsument"
#define SEM_PROD "/producent"
#define SHM_NAME "/memory"

void cleanup()
{
    unlinkSem(SEM_CON);
    unlinkSem(SEM_PROD);
    delete_shm(SHM_NAME);
}



int main(int argc, char *argv[])
{
    if(signal(SIGINT,cleanup) == SIG_ERR)
    {
        perror("Signal handling error");
        exit(EXIT_FAILURE);
    }
    
    atexit(cleanup);

    sem_t *sem_con = createSem(SEM_CON);
    if (sem_con == SEM_FAILED)
    {
        perror("Consumer semaphore creation failed");
        exit(EXIT_FAILURE);
    }
    sem_t *sem_prod = createSem(SEM_PROD);
    if (sem_prod == SEM_FAILED)
    {
        perror("Producer semaphore creation failed");
        unlinkSem(SEM_CON);
        exit(EXIT_FAILURE);
    }
    int shm = create_shm(SHM_NAME, sizeof(SegmentPD));
    if (shm == -1)
    {
        perror("Shared memory creation failed");
        unlinkSem(SEM_CON);
        unlinkSem(SEM_PROD);
        exit(EXIT_FAILURE);
    }

    // Inicjalizacja semaforów
    if (sem_init(sem_con, 1, 0) == -1) {
        perror("sem_init failed for consumer semaphore");
        exit(EXIT_FAILURE);
    }
    if (sem_init(sem_prod, 1, NBUF) == -1) {
        perror("sem_init failed for producer semaphore");
        exit(EXIT_FAILURE);
    }

    SegmentPD *segment = (SegmentPD *)map_shm(shm, sizeof(SegmentPD));
    segment->in = 0;
    segment->out = 0;

    printf("Consumer semaphore address: %p, initial value: %d\n", (void *)sem_con, getSemValue(sem_con));
    printf("Producer semaphore address: %p, initial value: %d\n", (void *)sem_prod, getSemValue(sem_prod));
    printf("Shared memory address: %d, size: %ld\n", shm, sizeof(SegmentPD));

    pid_t pid1, pid2;

    pid1 = fork();
    if (pid1 == -1)
    {
        perror("Fork failed for producer");
        cleanup();
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0)
    {
        execlp("./producer.x", "./producer.x", SEM_CON, SEM_PROD, SHM_NAME, "producer.txt", NULL);
        perror("execlp failed for producer");
        exit(EXIT_FAILURE);
    }

    pid2 = fork();
    if (pid2 == -1)
    {
        perror("Fork failed for consumer");
        cleanup();
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0)
    {
        execlp("./consumer.x", "./consumer.x", SEM_CON, SEM_PROD, SHM_NAME, "consumer.txt", NULL);
        perror("execlp failed for consumer");
        exit(EXIT_FAILURE);
    }

    if (waitpid(pid1, NULL, 0) == -1)
    {
        perror("Wait error for producer");
    }

    if (waitpid(pid2, NULL, 0) == -1)
    {
        perror("Wait error for consumer");
    }

    return 0;
}
