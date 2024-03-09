#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>



int main() {
    int pid = getpid();

    char *heap = malloc(20);
    *heap = 0x61;
    printf("heap (%p) pointing to 0x%x\n", heap, *heap);
    free(heap);
    char *foo = malloc(20);
    *foo = 0x62;
    printf("foo (%p) pointing to 0x%x\n", foo, *foo);
    *heap = 0x63;
    printf("or is it pointing to 0x%x\n", *heap);


    printf("process id: %d\n", pid);
    printf("\n\n /proc/%d/maps \n\n", pid);
    char command[50];
    sprintf(command, "cat /proc/%d/maps", pid);
    system(command);
    return 0;
}
