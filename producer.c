#include <string.h>
#include "semaphores.h"
#include "shmlib.h"

#define NELE 20 // Rozmiar elementu bufora (jednostki towaru) w bajtach
#define NBUF 5  // Liczba elementow bufora

#define SEM_CON "/konsument"
#define SEM_PROD "/producent"
#define SHM_NAME "/memory"

// argv[1]- semafor konsumenta
// argv[2]- semafor producenta
// argv[3]- pamięć dzielona
// argv[4]- nazwa pliku


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

    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    sem_t *semConsumer = openSem(argv[1]);
    sem_t *semProducer = openSem(argv[2]);
    int shm = openMem(argv[3], sizeof(SegmentPD));
    SegmentPD *fdshm = (SegmentPD *)mapMem(shm, sizeof(SegmentPD));

    printf("Consumer semaphore address: %p, initial value: %d\n", semConsumer, getSemValue(semConsumer));
    printf("Producer semaphore address: %p, initial value: %d\n", semProducer, getSemValue(semProducer));
    printf("Shared memory address: %d, size: %d\n", shm, sizeof(SegmentPD));
    
    while (1)
    {
        // produkcja towaru
        if (bytes_read = read(fd_in, buffer, NELE) == -1)
        {
            perror("read function error");
            exit(EXIT_FAILURE);
        }

        waitSem(argv[2]);
        // sekcja krytyczna - semafor obniżony

        fdshm->wstaw = (fdshm->wstaw + 1) % NELE;
        //strcpy(fdshm->bufor[fdshm->wstaw], bytes_read);
        //fdshm->bufor[fdshm->wstaw] = bytes_read;

        printf("Proces potomny %d zajął semafor.\n", getpid());
        raiseSem(argv[1]);
        // podniesienie semafora
    }
    closeSem(argv[1]);
    closeSem(argv[2]);

    exit(EXIT_SUCCESS);
    return 0;

}