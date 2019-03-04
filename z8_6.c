//Zadanie 6
//Napisz program, który prosi o podanie hasła i kończy działanie, jeśli użytkownik nie wprowadzi hasła w określonym
//czasie. Wprowadzane hasło ma być maskowane i ma być widać znaki (np. ‘x’) w czasie wpisywania. Poprawne
//hasło ma być przechowywane w pliku w postaci zaszyfrowanej (metoda dowolna).

#include <stdio.h>
#include <unistd.h>
#include <memory.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>

#define TIME 6
#define MAX_PASSWORD_LENGTH 20
#define CMD_LENGTH 100
#define HASH_LENGTH 100
#define PASSWORD_FILE "pswd.dat"

#define GREEN  "\x1B[32m"
#define RED  "\x1B[31m"
#define RESET "\033[0m"

static struct termios oldTerminal;

void trim(char *text) {
    int lastChar = strlen(text) - 1;
    if (text[lastChar] == '\n')
        text[lastChar] = '\0';
}

void sigalrm_handler( int sig_num )
{
    printf("\n");
    printf("Nie podales poprawnego hasla w ciagu %d sekund.\n", TIME);
    printf("Program konczy dzialanie.\n");
    fflush(stdout);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminal);
    exit(EXIT_FAILURE);
}

void setAlarm() {
    signal(SIGALRM, sigalrm_handler);
    alarm(TIME);
}

void getPassword(char *password)
{
    static struct termios newTerminal;

    if ((tcgetattr(STDIN_FILENO, &oldTerminal)) == -1) {
        printf("Blad terminala!\n");
        exit(EXIT_FAILURE);
    }

    newTerminal = oldTerminal;
    newTerminal.c_lflag &= ICANON;
    newTerminal.c_lflag &= ECHOE;
    newTerminal.c_lflag &= ~ECHO;
    if ((tcsetattr(STDIN_FILENO, TCSANOW, &newTerminal)) == -1) {
        printf("Blad terminala!\n");
        exit(EXIT_FAILURE);
    }

    int c;
    int pos = 0;
    password[pos] = '\0';
    while (1) {
        if ((c = getchar()) != EOF) {
            if (c == '\n') {
                break;
            }
            if (c == 0x7f) {
                if (pos > 0) {
                    pos--;
                    password[pos] = '\0';
                    printf("\b \b");
                }
            } else {
                password[pos++] = (char) c;
                printf("*");
            }
        }
    }

    printf("\n");
    if ((tcsetattr(STDIN_FILENO, TCSANOW, &oldTerminal)) == -1) {
        printf("Blad terminala!\n");
        exit(EXIT_FAILURE);
    }
}

void encodePassword(char *password, char *hashedPassword) {
    char cmd[CMD_LENGTH];
    sprintf(cmd, "echo -n %s | base64", password);

    FILE *file = popen(cmd, "r");
    if (!file) {
        perror("Blad otwierania pliku");
        exit(EXIT_FAILURE);
    }

    fgets(hashedPassword, HASH_LENGTH, file);
    trim(hashedPassword);

    if (pclose(file) == -1) {
        printf("Blad zamykania pliku.");
        exit(EXIT_FAILURE);
    }
}

void checkPassword(char *password) {
    FILE *pswd = fopen(PASSWORD_FILE, "r");
    if (!pswd) {
        perror("Blad pliku z haslem");
        exit(EXIT_FAILURE);
    }

    char actualPassword[MAX_PASSWORD_LENGTH];
    fgets(actualPassword, MAX_PASSWORD_LENGTH, pswd);
    trim(actualPassword);

    if (fclose(pswd) == -1) {
        printf("Blad zamykania pliku");
        exit(EXIT_FAILURE);
    }

    if (strcmp(password, actualPassword) == 0) {
        printf(GREEN "Haslo prawidlowe.\nGratulacje!\n" RESET);
        exit(EXIT_SUCCESS);
    } else {
        printf(RED "Haslo nieprawidlowe.\nProba wlamania zarejestrowana!\n" RESET);
        exit(EXIT_FAILURE);
    }
}

/*
 * Haslo to qwerty
 * Na podanie hasla jest 6 sekund.
 * */
int main() {
    setAlarm();
    printf("Podaj haslo:\n");
    char password[MAX_PASSWORD_LENGTH];
    char encodedPassword[MAX_PASSWORD_LENGTH];

    getPassword(password);
    encodePassword(password, encodedPassword);
    checkPassword(encodedPassword);

    return EXIT_SUCCESS;
}


