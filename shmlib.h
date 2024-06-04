#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#define NELE 20
#define NBUF 6


#ifndef SHMLIB_H
#define SHMLIB_H

typedef struct
{
    char bufor[NBUF][NELE]; // Wspolny bufor danych
    int in, out;      // Pozycje wstawiania i wyjmowania z bufora
} SegmentPD;


// Tworzenie pamięci dzielonej

int create_shm(const char *name, int size) 
{
    int fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL | O_TRUNC, 0644);
    if (fd == -1) 
    {
        perror("shm_open function error");
        return -1;
    }

    if (ftruncate(fd, size) == -1) 
    {
        perror("ftruncate function error");
        return -1;
    }

    return fd;
}

// Usuwanie pamięci dzielonej

int delete_shm(const char *name)
{
    if (shm_unlink(name) == -1) 
    {
        perror("Failed to delete shared memory");
        return -1;
    }
}

//  Otwieranie pamięci dzielonej

int open_shm(const char *name) 
{
    int fd = shm_open(name, O_RDWR, 0644);
    if (fd == -1) 
    {
        perror("shm_open function error");
        return -1;
    }

    return fd;
}

// Zamykanie pamięci dzielonej

int close_shm(int fd) 
{
    if (close(fd) == -1) 
    {
        perror("close function error");
        return -1;
    }
}

// Odwzorowanie pamięci dzielonej

void *map_shm(int fd, int size) 
{
    void *ptr = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) 
    {
        perror("mmap function error");
        return NULL;
    }

    return ptr;
}

// Usuwanie odwzorowania pamięci dzielonej

int unmap_shm(void *ptr, int size) 
{
    if (munmap(ptr, size) == -1) 
    {
        perror("munmap function error");
        return -1;
    }
}

#endif