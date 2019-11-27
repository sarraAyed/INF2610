/*
 * Comm Lab - puzzle1.c
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
// -------------------------------------------------

#include "libcommlab.h"

void puzzle1() {
    // TODO
    char exchangerPipeWrite[] = {"/tmp/pipe_51d403d4"};
    char exchangerPipeRead[] ={"/tmp/pipe_46aa8044"};

    mkfifo(exchangerPipeRead, 0660);
    mkfifo(exchangerPipeWrite, 0660);

    

    pid_t pidFils= fork();
    if (pidFils == 0)
    {
        int fd= open("/tmp/pipe_46aa8044", O_RDONLY);
        dup2(fd, 0);
        execl("./puzzle1/exchanger", "exchanger", NULL);
        close (fd);
    }
    else 
    {
        int fdwrite= open ("/tmp/pipe_46aa8044", O_WRONLY);
        int fdRead = open ("/tmp/pipe_51d403d4", O_RDONLY);
        dup2(fdwrite,1);
        write(1, "b4bf32d00a3f", 12);
        kill(pidFils, SIGUSR2);

        char  message[1000] ;
        read(fdRead, message, 1000 );
        checkExchangerMessage(message);

        close(fdwrite);
        close (fdRead);
        wait(NULL);
    }
}