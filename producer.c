#include <string.h>
#include "semaphores.h"
#include "shmlib.h"

#define NELE 20 // Rozmiar elementu bufora (jednostki towaru) w bajtach
#define NBUF 5  // Liczba elementow bufora

#define SEM_CON "/konsument"
#define SEM_PROD "/producent"
#define SHM_NAME "/memory"

int main(int argc, char *argv[])
{
    int fd_in;
    ssize_t bytes_read, bytes_written;
    int wstaw = 0;
    char buffer[NELE];
    int pid;

    typedef struct
    {
        char bufor[NBUF][NELE]; // Wspolny bufor danych
        int wstaw, wyjmij;      // Pozycje wstawiania i wyjmowania z bufora
    } SegmentPD;

    sem_t *semConsumer = openSem(SEM_CON);
    sem_t *semProducer = openSem(SEM_PROD);
    int shm = openMem(SHM_NAME, sizeof(SegmentPD));
    SegmentPD *fdshm = (SegmentPD *)mapMem(shm, sizeof(SegmentPD));

    while (1)
    {
        // produkcja towaru
        if (bytes_read = read(fd_in, buffer, NELE) == -1)
        {
            perror("read function error");
            exit(EXIT_FAILURE);
        }

        waitSem(SEM_PROD);
        // sekcja krytyczna - semafor obniżony

        fdshm->wstaw = (fdshm->wstaw + 1) % NELE;
        //strcpy(fdshm->bufor[fdshm->wstaw], bytes_read);
        //fdshm->bufor[fdshm->wstaw] = bytes_read;

        printf("Proces potomny %d zajął semafor.\n", getpid());
        raiseSem(SEM_CON);
        // podniesienie semafora
    }
    closeSem(SEM_PROD);
    closeSem(SEM_CON);

    exit(EXIT_SUCCESS);
    return 0;

}