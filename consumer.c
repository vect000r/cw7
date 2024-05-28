#include <string.h>
#include "semaphores.h"
#include "shmlib.h"

#define NELE 20 // Rozmiar elementu bufora (jednostki towaru) w bajtach
#define NBUF 5  // Liczba elementow bufora

#define SEM_CON "/konsument"
#define SEM_PROD "/producent"
#define SHM_NAME "/memory"

int main()
{
    int fd_out;
    ssize_t bytes_read, bytes_written;
    int wstaw = 0;
    char buffer[NELE];
    int pid;

    typedef struct
    {
        char bufor[NBUF][NELE]; // Wspolny bufor danych
        int wstaw, wyjmij;      // Pozycje wstawiania i wyjmowania z bufora
    } SegmentPD;
    
    // sekcja prywatna
    sem_t *semConsumer = openSem(SEM_CON);
    sem_t *semProducer = openSem(SEM_PROD);
    int shm = open_shm(SHM_NAME);
    SegmentPD *fdshm = (SegmentPD *)mapMem(shm, sizeof(SegmentPD));

    fd_out = open("consumer.txt", O_RDONLY);
    if (fd_out == -1)
    {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // produkcja towaru
        if (bytes_read = write(fd_out, buffer, NELE) == -1)
        {
            perror("Read function error");
            exit(EXIT_FAILURE);
        }

        waitSem(semConsumer);
        // sekcja krytyczna - semafor obniżony

        fdshm->wyjmij = (fdshm->wyjmij + 1) % NELE;
        // strncpy(fdmem->bufor[fdmem->wyjmij]);
        printf("Child process %d occupied semaphore.\n", getpid());
        // podniesienie semafora
        raiseSem(semProducer);
    }
    closeSem(semProducer);
    closeSem(semConsumer);

    exit(EXIT_SUCCESS);
    return 0;           
}