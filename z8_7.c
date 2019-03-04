//Zadanie 7
//Uzupełnij program shell z laboratorium 5-6 o „ręczną” obsługę potoków.

#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define ARG_LENGTH 35

int     shellcmd(char *);
void    prstat(int);

void trimWhitespaces(char *text) {
    int length = strlen(text);

    int start = 0;
    int end = length - 1;
    while (text[start] == ' ') {
        start++;
    }
    while (text[end] == ' ') {
        end--;
    }

    char trimmedText[length];
    int j = 0;
    for (int i = start; i <= end; ++i) {
        trimmedText[j++] = text[i];
    }
    trimmedText[j] = '\0';

    strcpy(text, trimmedText);
}

void executeCommand(char *cmd) {
    execlp(cmd, cmd, NULL);
    perror("Blad polecenia");
    exit(EXIT_FAILURE);
}

void closeDescriptors(int inFd, int outFd) {
    if (inFd != 0) {
        if ((dup2(inFd, STDIN_FILENO)) == -1) {
            perror("Blad przekierowania potoku");
            exit(EXIT_FAILURE);
        }else if ((close(inFd)) == -1) {
            perror("Blad zamykania potoku");
            exit(EXIT_FAILURE);
        }
    }

    if (outFd != 1) {
        if ((dup2 (outFd, STDOUT_FILENO)) == -1) {
            perror("Blad przekierowania potoku");
            exit(EXIT_FAILURE);
        }else if ((close (outFd)) == -1) {
            perror("Blad zamykania potoku");
            exit(EXIT_FAILURE);
        }
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
    int originalStdin;
    originalStdin = dup(STDIN_FILENO);

    int inFd, fd[2];
    inFd = 0;

    int i;
    for (i = 0; i < argc - 1; ++i) {
        if (pipe(fd) == -1) {
            perror("Blad potoku\n");
            exit(EXIT_FAILURE);
        }

        processArgument(inFd, fd[1], argv[i]);
        close(fd[1]);
        inFd = fd[0];
    }

    if (inFd != 0) {
        dup2 (inFd, STDIN_FILENO);
    }

    int status;
    pid_t pid = fork();
    if (pid == -1) {
        perror("Blad procesu");
        exit(EXIT_FAILURE);
    }else if (pid == 0) {
        executeCommand(argv[i]);
    }

    waitpid(pid, &status, 0);
    prstat(status);

    dup2(originalStdin, STDIN_FILENO);
}

int main(void)
{
    char command[BUFSIZ];

    /*
     * Forever...
     */
    for (;;) {
        printf("Enter a command: ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            putchar('\n');
            break;
        }
        command[strlen(command)-1] = '\0';

        char *commands[ARG_LENGTH];
        int i = 0;
        char *current = strtok(command, "|");
        while (current != NULL) {
            trimWhitespaces(current);
            commands[i++] = current;
            current = strtok(NULL, "|");
        }
        commands[i] = NULL;

        executeAll(i, commands);
        putchar('\n');
    }

    return 0;
}

void prstat(int status)
{
    if (WIFEXITED(status)) {
        printf("Polecenie zakonczone normalnie z kodem: %d\n", WEXITSTATUS(status));
    } else {
        printf("Polecenie zakonczone sygnalem (kod %d)\n", WSTOPSIG(status));
    }
}
