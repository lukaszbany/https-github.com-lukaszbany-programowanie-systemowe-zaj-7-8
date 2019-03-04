//Zadanie 4.
//Program czyta z dwóch plików, do których napływają dane. Napisz program, który będzie czytał te pliki
//do momentu, kiedy zostaną zakończone i wyświetlał sumę przeczytanych bajtów z obydwu plików.

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define BUFFER_SIZE 256

void saveChars(int numerOfChars, int milisecondsInterval) {
    for (int i = 0; i < numerOfChars; i++) {
        struct timespec time;
        time.tv_sec = 0;
        time.tv_nsec = milisecondsInterval * 100000000;
        nanosleep(&time, NULL);
        printf("+");
    }
}

int main() {
    int fd1[2];
    int fd2[2];
    pid_t pid1;
    if ((pipe(fd1) == -1) || (pipe(fd2) == -1) || ((pid1 = fork()) == -1)) {
        perror("Nie udalo sie utworzyc potoku");
        return 1;
    }

    if (pid1 == 0) {
        if ((dup2(fd1[1], STDOUT_FILENO)) == -1)               /* sort is the parent */
            perror("Blad przekierowania wyjscia do potoku");
        else if ((close(fd1[0]) == -1) || (close(fd1[1]) == -1))
            perror("Blad zamykania deskryptorow pliku");

        saveChars(10, 2);
    } else {
        pid_t pid2;
        if ((pid2 = fork()) == -1) {
            perror("Nie udalo sie utworzyc potoku");
            return 1;
        }

        if (pid2 == 0) {
            if ((dup2(fd2[1], STDOUT_FILENO)) == -1)               /* sort is the parent */
                perror("Blad przekierowania wyjscia do potoku");
            else if ((close(fd2[0]) == -1) || (close(fd2[1]) == -1))
                perror("Blad zamykania deskryptorow pliku");

            saveChars(20, 2);
        } else {
            printf("Odbieranie danych...\n");
            int counter1 = 0;
            int counter2 = 0;

            int bytes;
            char buf[BUFFER_SIZE];

            bytes = read(fd1[0], buf, sizeof(buf));
            if (bytes > 0) {
                counter1 += bytes;
            }

            bytes = read(fd2[0], buf, sizeof(buf));
            if (bytes > 0) {
                counter2 += bytes;
            }

            printf("Odebrano %d i %d bajtow\n", counter1, counter2);
            printf("Lacznie %d bajtow\n", counter1 + counter2);
        }
    }

    return 0;
}
