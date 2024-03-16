#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void handler(int _signal) {
    printf("caught signal %d\n", _signal);
    alarm(5);
    signal(SIGALRM, handler);
}

int main(void) {
    char name[80];
    alarm(5);
    signal(SIGALRM, handler);

    printf("Input a name: ");
    while (1) {
        if (fgets(name, sizeof(name), stdin))
            break;
    }
    printf("Hi, %s", name);
    return 0;
}
