/*
 * Comm Lab - puzzle2.c
 * 
 * Ecole polytechnique de Montreal, 2018
 */

// TODO
// Si besoin, ajouter ici les directives d'inclusion
// -------------------------------------------------
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
// -------------------------------------------------

void puzzle2() {
    // TODO
    int fd[2];
    pipe(fd);
    if (fork() == 0)
    {
        close(fd[1]);
        char buffer[50];
        sprintf(buffer ,"%d", fd[0]);
        execl("./puzzle2/telegraph", "telegraph", buffer ,NULL);
    }
    else 
    {
        unsigned char array[] = {129, 124, 120, 114, 123, 18, 118, 128, 18, 217, 214, 216, 214, 211, 217, 110, 209, 214, 113, 209, 115, 18, 114, 123, 113, 0};
        close(fd[0]);
        for (int i = 0 ; i <28 ; i++)
        {
            write(fd[1], &array[i], 1);    
        }
        close(fd[1]);
        wait(NULL);
    }
}