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

    fdshm->in = 0;

    int file = open(argv[4], O_RDONLY);
    if (file == -1)
    {
        perror("open error");
        exit(EXIT_FAILURE);
    }

   
    while (1)
    {
        
        semWait(semProducer);
        
        int bytes_read = read(file, fdshm->bufor[fdshm->in], NELE);
        if (bytes_read == -1)
        {
            perror("read error");
            exit(EXIT_FAILURE);
        }
        
        if(bytes_read != NELE)
        {
            fdshm->bufor[fdshm->in][bytes_read] = '\0'; // zapisujemy \0
            printf("Producer wrote to buffer: %s\n", fdshm->bufor[fdshm->in]);
            semPost(semConsumer);
            break;
        }
    
        fdshm->in = (fdshm->in + 1) % NBUF;
        semPost(semConsumer);
    }
    


    if(close(file)==-1)
    {
        perror("close error");
        exit(EXIT_FAILURE);
    }
    
    closeSem(semProducer);
    closeSem(semConsumer);
    close_shm(shm);
    
    exit(EXIT_SUCCESS);
}
