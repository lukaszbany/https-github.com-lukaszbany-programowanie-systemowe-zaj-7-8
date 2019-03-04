//Zadanie 2.
//Zapoznaj się z programem simpleredirect.c. Program ten jest równoważny wykonaniu polecenia:
//ls –l | sort –n –k5.
//a) Czy ma znaczenie, w którym procesie będzie wykonywane polecenie ls?
//b) Co będzie się działo, jeśli deskryptory fd[0] i fd[1] nie będą zamknięte przed wywołaniem execl?
//c) Rozbuduj program simpleredirect.c tak, aby polecenia do wykonania były pobierane z wiersza
//wywołania programu.

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <memory.h>
#include <wait.h>
#include <stdlib.h>

#define CMD_LENGTH 150
#define ARG_LENGTH 35
#define ARGS_NUM 10

void trim(char *text) {
    int lastChar = strlen(text) - 1;
    if (text[lastChar] == '\n')
        text[lastChar] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Bledna ilosc argumentow.");
        exit(EXIT_FAILURE);
    }

    pid_t childpid;
    int fd[2];

    if ((pipe(fd) == -1) || ((childpid = fork()) == -1)) {
        perror("Failed to setup pipeline");
        return 1;
    }

    if (childpid == 0) {                                  /* ls is the child */
        if (dup2(fd[1], STDOUT_FILENO) == -1)
            perror("Failed to redirect stdout of ls");
        else if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
            perror("Failed to close extra pipe descriptors on ls");
        else {

            int i = 0;
            char *args[ARG_LENGTH];

            char *current = strtok(argv[1], " ");
            while (current != NULL) {
                args[i++] = current;
                current = strtok(NULL, " ");
            }
            args[i] = NULL;

            execvp(args[0], args);
            perror("Failed to exec command");
        }
        return 1;
    }
    if (dup2(fd[0], STDIN_FILENO) == -1)               /* sort is the parent */
        perror("Failed to redirect stdin of sort");
    else if ((close(fd[0]) == -1) || (close(fd[1]) == -1))
        perror("Failed to close extra pipe file descriptors on sort");
    else {
        int i = 0;
        char *args[ARG_LENGTH];

        char *current = strtok(argv[2], " ");
        while (current != NULL) {
            args[i++] = current;
            current = strtok(NULL, " ");
        }
        args[i] = NULL;

        execvp(args[0], args);
        perror("Failed to exec command");
    }
    return 1;
}


