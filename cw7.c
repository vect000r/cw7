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

int main()
{
    sem_t *sem_con = createSem(SEM_CON);
    sem_t *sem_prod = createSem(SEM_PROD);
    int shm = create_shm(SHM_NAME, sizeof(SegmentPD));

    printf("Consumer semaphore address: %p, initial value: %d\n", sem_con, getSemValue(sem_con));
    printf("Producer semaphore address: %p, initial value: %d\n", sem_prod, getSemValue(sem_prod));

    int pid_prod = fork();
    switch (pid_prod)
    {
    case -1:
        // błąd
        perror("Błąd forka");
        exit(EXIT_FAILURE);
        break;
    case 0:
        // child
        // producent
        if (execlp("./producer.x", "./producer.x", SEM_CON, SEM_PROD, SHM_NAME, NULL) == -1)
        {
            perror("Execlp error in child process\n");
            exit(EXIT_FAILURE);
        }
        break;
    default:
        // parent
        int pid_con = fork();
        switch (pid_con)
        {
        case -1:
            // błąd
            perror("Błąd forka");
            exit(EXIT_FAILURE);
            break;
        case 0:
            // child
            // konsument
            if (execlp("./consumer.x", "./consumer.x", SEM_PROD, SHM_NAME, NULL) == -1)
            {
                perror("Execlp error in child process\n");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            // parent
            if (waitpid(pid_prod, NULL, 0) == -1)
            {
                perror("Wait error");
                exit(EXIT_FAILURE);
            }
            if (waitpid(pid_con, NULL, 0) == -1)
            {
                perror("Wait error");
                exit(EXIT_FAILURE);
            }
            // Usuwanie semaforów i pamięci dzielonej
            removeSem(SEM_CON);
            removeSem(SEM_PROD);
            remove_shm(SHM_NAME);
            break;
        }
        break;
    }
}