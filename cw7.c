#include "shmlib.h"
#include "semaphores.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define NELE 20
#define NBUF 5
#define SEM_CON "/konsument"
#define SEM_PROD "/producent"
#define SHM_NAME "/memory"


typedef struct 
{
    char bufor[NBUF][NELE]; // Wspolny bufor danych
    int wstaw, wyjmij; // Pozycje wstawiania i wyjmowania z bufora
} SegmentPD;

int main(int argc, char *argv[])
{
    sem_t *sem_con = createSem(SEM_CON);
    sem_t *sem_prod = createSem(SEM_PROD);
    int shm = create_shm(SHM_NAME, sizeof(SegmentPD));

    printf("Consumer semaphore address: %p, initial value: %d\n", sem_con, getSemValue(sem_con));
    printf("Producer semaphore address: %p, initial value: %d\n", sem_prod, getSemValue(sem_prod));
    printf("Shared memory address: %d, size: %d\n", shm, sizeof(SegmentPD));

    pid_t pid1, pid2;

    (pid1 = fork()) && (pid2 = fork()); 
    
    if (pid1 == 0) 
    {
        execlp("./producer.x", SEM_CON, SEM_PROD, SHM_NAME, "producer.txt", NULL);
    } 
    else if (pid2 == 0) 
    {
        execlp("./consumer.x", SEM_CON, SEM_PROD, SHM_NAME, "consumer.txt", NULL);
    } 
    else 
    {
        
        if(waitpid(pid1, NULL, 0) == -1)
        {
            perror("Wait error 1");
            exit(EXIT_FAILURE);
        } 
        
        if(waitpid(pid2, NULL, 0) == -1)
        {
            perror("Wait error 2");
            exit(EXIT_FAILURE);
        }
        
        // Usuwanie semaforów i pamięci dzielonej
        removeSem(SEM_CON);
        removeSem(SEM_PROD);
        remove_shm(SHM_NAME);
    }
    return 0;
}          