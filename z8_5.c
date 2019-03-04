//Zadanie 5.
//Rozbuduj program z zadania 2 tak, aby potok mógł być dowolnie duży. Przykład:
//simpleredirect who sort head.
//Jest to odpowiednik polecenia who | sort | head.

#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>

#define CMD_LENGTH 150
#define ARG_LENGTH 35
#define ARGS_NUM 10

void executeCommand(char *cmd) {
    int i = 0;
    char *command[ARG_LENGTH];

    char *current = strtok(cmd, " ");
    while (current != NULL) {
        command[i++] = current;
        current = strtok(NULL, " ");
    }
    command[i] = NULL;

    execvp(command[0], command);
    perror("Blad polecenia");
}

void closeDescriptors(int inFd, int outFd) {
    if (inFd != 0) {
        dup2 (inFd, 0);
        close (inFd);
    }

    if (outFd != 1) {
        dup2 (outFd, 1);
        close (outFd);
    }
}

void processArgument(int inFd, int outFd, char *cmd) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Blad procesu");
        exit(EXIT_FAILURE);
    }else if (pid == 0) {
        closeDescriptors(inFd, outFd);
        executeCommand(cmd);
    }
}

void executeAll(int argc, char **argv) {
    int inFd, fd[2];
    
    inFd = 0;
    int i;

    for (i = 1; i < argc - 1; ++i) {
        if (pipe(fd) == -1) {
            perror("Blad potoku\n");
            exit(EXIT_FAILURE);
        }

        processArgument(inFd, fd[1], argv[i]);
        close(fd[1]);
        inFd = fd[0];
    }

    if (inFd != 0) {
        dup2 (inFd, 0);
    }

    executeCommand(argv[i]);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Za malo argumentow\n");
        exit(EXIT_FAILURE);
    }

    executeAll(argc, argv);

    return EXIT_SUCCESS;
}


