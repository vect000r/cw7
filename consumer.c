#include <string.h>
#include "semaphores.h"
#include "shmlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
// argv[1]- semafor konsumenta
// argv[2]- semafor producenta
// argv[3]- pamięć dzielona
// argv[4]- nazwa pliku

// Funkcja sprawdzająca czy dane w buforze się skończyły (czy natrafiliśmy na znak \0)

bool is_out_of_data(SegmentPD *fdshm)
{
    for (int i = 0; i < NELE; i++)
    {
        if (fdshm->bufor[fdshm->out][i] == '\0')
        {
            return true;
        }
    }
    return false;
}

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

    //char item[NELE];
    fdshm->out = 0;    

    while (1)
    {
        semWait(semConsumer);
        if(is_out_of_data(fdshm)==false)
        {
            if(write(file, fdshm->bufor[fdshm->out], NELE) == -1)
            {
                perror("write error");
                exit(EXIT_FAILURE);
            }
            
            printf("Consumer wrote to file: %s\n", fdshm->bufor[fdshm->out]);
            
        }
        else
        {
            if(write(file, fdshm->bufor[fdshm->out], strlen(fdshm->bufor[fdshm->out])) == -1)
            {
                perror("write error");
                exit(EXIT_FAILURE);
            }
        
            printf("Consumer wrote to file:\n");

            if(write(STDOUT_FILENO, fdshm->bufor[fdshm->out], NELE) == -1)
            {
                perror("write error");
                exit(EXIT_FAILURE);
            }


            break;
        }

        fdshm->out = (fdshm->out + 1) % NBUF;

        semPost(semProducer);
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
