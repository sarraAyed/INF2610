/*
 * Comm Lab - puzzle3.c
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
#include <stdlib.h>
// -------------------------------------------------

void puzzle3() {
    // TODO
    
    if (fork()==0)
    {
        int file = open("./puzzle3/tmpfile", O_WRONLY);
        dup2(file, STDERR_FILENO);
        dup2( STDERR_FILENO, STDOUT_FILENO);

        execl("./puzzle3/exc1", "exc1", NULL);

    }
    wait(NULL);

    int fd[2];
    pipe(fd);
    if(fork() ==0)
    {
        close(fd[0]);
    //     dup2(fd[1], STDERR_FILENO);
    //     close(fd[1]);
    //     dup2(STDERR_FILENO, STDOUT_FILENO);
    //     execl("./puzzle3/exc2", "exc2", NULL);
    }
    // else if(fork()==0)
    // {
    //     close(fd[1]);
    //     dup2(fd[0], STDERR_FILENO);
    //     close(fd[0]);
    //     dup2(STDERR_FILENO, STDOUT_FILENO);
        
    //     execl("./puzzle3/exc3", "exc3", NULL);
    // }
    while(wait(NULL));
    close(fd[0]);
    close(fd[1]);

    if(fork()==0)
    {
        int file = open("./puzzle3/tmpfile", O_WRONLY);
        dup2(file, STDIN_FILENO);
        // execl("./puzzle3/exc4", "exc4", NULL);
    }
    wait(NULL);
  
    
}