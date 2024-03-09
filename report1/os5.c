#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

typedef unsigned long ulong;

void zot(ulong *stop) {
    ulong r = 0x3;
    ulong *i;
    for (i = &r; i <= stop; ++i) {
	printf("%p 0x%lx\n", i, *i);
    }
}

void foo(ulong *stop) {
    ulong q = 0x2;
    zot(stop);
}

int main() {
    int pid = getpid();

    unsigned long p = 0x1;
    foo(&p);
back:
    printf("  p: %p \n", &p);
    printf("  back: %p \n", &&back);

    printf("process id: %d\n", pid);
    printf("\n\n /proc/%d/maps \n\n", pid);
    char command[50];
    sprintf(command, "cat /proc/%d/maps", pid);
    system(command);
    return 0;
}
