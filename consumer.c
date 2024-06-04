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

    int file = open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file == -1)
    {
        perror("open error");
        exit(EXIT_FAILURE);
    }

    char item[NELE];
    //int i = 1;

    while (1)
    {
        semWait(semConsumer);

        //char item[NELE];
        strncpy(item, fdshm->bufor[fdshm->out], NELE);
        
        fdshm->out = (fdshm->out + 1) % NBUF;

        if (strcmp(item, "\0") == 0)
        {
            break;
        }

        if (write(file, item, strlen(item)) == -1)
        {
            perror("write error");
            exit(EXIT_FAILURE);
        }

        printf("Consumer: %s\n", item);

        semPost(semProducer);
        //i = i + 1;
        
    
    }

    close(file);
    closeSem(semProducer);
    closeSem(semConsumer);

    exit(EXIT_SUCCESS);
}
