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
        //dup2(file, STDERR_FILENO);
        //dup2( STDERR_FILENO,STDOUT_FILENO);
        dup2(file, STDERR_FILENO);
        dup2(file, STDOUT_FILENO);
        execl("./puzzle3/exc1", "exc1", NULL);

    }
    wait(NULL);

    
    if(fork() ==0)
    {
        int fd[2];
        pipe(fd);
        if (fork() == 0)
        {
            close(fd[0]);
            //dup2( STDERR_FILENO, STDOUT_FILENO);
            dup2(fd[1], STDERR_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            execl("./puzzle3/exc2", "exc2", NULL);
        }
        else 
        {
            close(fd[1]);

            dup2(fd[0], STDIN_FILENO);
            dup2(STDOUT_FILENO, STDERR_FILENO);
            close(fd[0]);
            execl("./puzzle3/exc3", "exc3", NULL);
        }
        wait(NULL);
            
    }
  
    wait(NULL);
    if(fork()==0)
    {
        int file = open("./puzzle3/tmpfile", O_RDONLY);
        //if(file < 0) printf("YP, WE GOT A PROBLEM HERE");
        dup2(file, STDIN_FILENO);
        execl("./puzzle3/exc4", "exc4", NULL);
    }
    wait(NULL);
  
    
}