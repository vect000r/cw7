#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef _SEMAPHORES_H_
#define _SEMAPHORES_H_

// Tworzenie nowego semafora

sem_t *createSem(const char *name)
{
    sem_t *sem = sem_open(name, O_CREAT | O_EXCL, 0666, 1);
    if (sem == SEM_FAILED)
    {
        perror("Failed to create semaphore");
        printf("line: %d \n", __LINE__);
        printf("file: %s \n", __FILE__);
        exit(1);
    }
    return sem;
}

// Otwieranie istniejącego semafora

sem_t *openSem(const char *name)
{
    sem_t *sem = sem_open(name,0);
    if (sem == SEM_FAILED)
    {
        perror("Failed to open semaphore");
        printf("line: %d \n", __LINE__);
        printf("file: %s \n", __FILE__);
        exit(1);
    }
    return sem;
}

// Pobieranie informacji o wartości zwracanej przez semafor

int getSemValue(sem_t *sem)
{
    int value;
    if(sem_getvalue(sem, &value) == -1)
    {
        perror("Failed to get semaphore value");
        exit(1);
    }
    return value;
}

// Podnoszenie semafora

void semPost(sem_t *sem)
{
    if(sem_post(sem) == -1)
    {
        perror("Failed to post semaphore");
        exit(1);
    }
}

// Opuszczanie semafora

void semWait(sem_t *sem)
{
    if(sem_wait(sem) == -1)
    {
        perror("Failed to wait semaphore");
        exit(1);
    }
}

// Zamykanie semafora

void closeSem(sem_t *sem)
{
    if(sem_close(sem) == -1)
    {
        perror("Failed to close semaphore");
        exit(1);
    }
}

// Usuwanie semafora   

void unlinkSem(const char *name)
{
    if(sem_unlink(name) == -1)
    {
        perror("Failed to unlink semaphore");
        exit(1);
    }
}

void setSemValue(sem_t *sem, int value)
{
    while (getSemValue(sem) > 0)
        semWait(sem);
    while (getSemValue(sem) < value)
        semPost(sem);
}

#endif