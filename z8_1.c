//Zadanie 1.
//Zapoznaj się z programem pipedemo.c.
//Napisz program, który będzie wysyłał list automatycznie generowany przez program. Do wysyłania listu użyj
//polecenia mail.
//Użyteczna funkcja: do utworzenia składowych polecenia można użyć instrukcji:
//sprintf(arg,"-s 'Wiadomosc od procesu PID %d'", getpid());

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define ADDRESS "banyl@wit.edu.pl"

int main ()
{
    int fd[2];
    int fd2[2];
    pid_t pid;
    pipe(fd);
    pipe(fd2);
    pid = fork();
    if (pid == (pid_t) 0) {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);

        dup2(fd2[1], STDOUT_FILENO);
        close(fd2[0]);
        close(fd2[1]);

        execlp("sort", "sort", NULL);
    }
    else {
        FILE* strumien;
        close (fd[0]);
        strumien = fdopen(fd[1], "w");
        fprintf(strumien, "Witam.\n");
        fprintf(strumien, "Welcome.\n");
        fprintf(strumien, "Bienvenue.\n");
        fprintf(strumien, "Willkommen.\n");
        fflush(strumien);
        close(fd[1]);

        waitpid(pid, NULL, 0);
        pid_t pid2;
        pid2 = fork();

        if (pid2 == (pid_t) 0) {
            dup2(fd2[0], STDIN_FILENO);
            close(fd2[0]);
            close(fd2[1]);

            char arg[50];
            sprintf(arg,"-s 'Wiadomosc od procesu PID %d'",getpid());
            execlp("mail", "mail", arg, ADDRESS, NULL);
        }
    }

    return 0;
}

