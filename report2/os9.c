#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

volatile int interupt = 0;

void handler(int _signal, siginfo_t *siginfo, void *context) {
    printf("caught signal %d\n", _signal);
    printf("uid is %d\n", siginfo->si_uid);
    printf("pid is %d\n", siginfo->si_pid);
    interupt = 1;
}

int main(void) {
    int pid = getpid();
    printf("when you interupt this you will get some info (pid: %d)\n", pid);

    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = handler;
    sigaction(SIGINT, &sa, NULL);

    while (!interupt) {}
    return 0;
}
