#include <string.h>
#include "semaphores.h"
#include "shmlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

// argv[1]- semafor konsumenta
// argv[2]- semafor producenta
// argv[3]- pamięć dzielona
// argv[4]- nazwa pliku

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        perror("Wrong number of arguments");
        exit(EXIT_FAILURE);
    }

    sem_t *semConsumer = openSem(argv[1]);
    sem_t *semProducer = openSem(argv[2]);
    int shm = open_shm(argv[3]);
    SegmentPD *fdshm = (SegmentPD *)map_shm(shm, sizeof(SegmentPD));

    int file = open(argv[4], O_RDONLY);
    if (file == -1)
    {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    char item[NELE];
    ssize_t bytes_read;
    while ((bytes_read = read(file, item, NELE)) > 0)
    {
        semWait(semProducer);

        memcpy(fdshm->bufor[fdshm->in], item, bytes_read);
        //fdshm->bufor[fdshm->in] = item;
        
        fdshm->in = (fdshm->in + 1) % NBUF;
        
        //fdshm->bufor[fdshm->in][bytes_read] = '\0'; // zapisujemy \0
        semPost(semConsumer);
    }
    //fdshm->bufor[fdshm->in][bytes_read - 1] = '\0'; // zapisujemy \0
    if(bytes_read == NELE) {
        semWait(semProducer);
        strncpy(fdshm->bufor[fdshm->in], "\0", 1);
        fdshm->in = (fdshm->in + 1) % NBUF;
        semPost(semConsumer);
    }


    close(file);
    closeSem(semProducer);
    closeSem(semConsumer);

    exit(EXIT_SUCCESS);
}
