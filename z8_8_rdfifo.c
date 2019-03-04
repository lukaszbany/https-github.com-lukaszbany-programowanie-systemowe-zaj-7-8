//Zadanie 8
//Napisz program, który zbiera komunikaty od wielu programów i wyświetla je na ekranie. Do komunikacji użyj
//potoku nazwanego.
//Wskazówka: Utwórz program rdfifo, którego zadaniem jest utworzenie kolejki FIFO i czytanie z niej danych.
//Utwórz program wrfifo, który otwiera kolejkę FIFO tylko do zapisu i wpisuje do niej dane (np. swoj pid i czas).
//W jaki sposób przekażesz wspólną nazwę kolejki FIFO do tych programów? W jaki sposób zapewnić działanie
//programu zbierającego komunikaty również wtedy, kiedy nie ma programu piszącego do łącza? Jak zapewnić to, że
//komunikaty pochodzące od różnych programów wyświetlane są w całości, tzn. nie są rozdzielane komunikatami od
//innych programów?

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <stdbool.h>
#include <fcntl.h>

#define SETTINGS_FILE "z8_8.dat"
#define MESSAGE_LENGTH 150
#define NAME_LENGTH 30
#define MODE 0666

static char fifoName[NAME_LENGTH];
static bool working = true;
static int fifo;

void trim(char *text) {
    int lastChar = strlen(text) - 1;
    if (text[lastChar] == '\n')
        text[lastChar] = '\0';
}

void safeExit(int status) {
    close(fifo);
    unlink(fifoName);
    exit(status);
}

void sighandler( int sig_num )
{
    printf("\n");
    printf("Koncze dzialanie programu.\n");

    working = false;
}

void milisecondsSleep(long milisecondsInterval) {
    struct timespec time;
    if (milisecondsInterval >= 1000) {
        time.tv_sec = milisecondsInterval / 1000;
        time.tv_nsec = (milisecondsInterval % 1000) * 1000000;
    } else {
        time.tv_sec = 0;
        time.tv_nsec = milisecondsInterval * 1000000;
    }

    nanosleep(&time, NULL);
}

void loadFifoName() {
    FILE *settings = fopen(SETTINGS_FILE, "r");
    if (!settings) {
        perror("Blad pliku z ustawieniami");
        safeExit(EXIT_FAILURE);
    }
    fgets(fifoName, NAME_LENGTH, settings);
    trim(fifoName);
}

void createFifo() {
    if (mkfifo(fifoName, MODE)) {
        perror("Blad tworzenia kolejki");
        safeExit(EXIT_FAILURE);
    }
}

void setup() {
    signal(SIGINT, sighandler);
    loadFifoName();
    createFifo();
}

int openFifo() {
    fifo = open(fifoName, O_RDONLY);
    if (fifo == -1) {
        perror("Blad otwierania kolejki FIFO");
        safeExit(EXIT_FAILURE);
    }

    return fifo;
}

int main()
{
    setup();
    openFifo();

    printf("Czekam na komunikaty\n");
    char message[MESSAGE_LENGTH];

    while (working) {
        int bytesRead = read(fifo, message, sizeof(message));
        if (bytesRead == -1) {
            perror("Blad podczas odczytu z kolejki");
            safeExit(EXIT_FAILURE);
        } else if (bytesRead > 0) {
            printf("%s", message);
            fflush(stdout);
        }
    }

    safeExit(EXIT_SUCCESS);
}

