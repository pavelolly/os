#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

char global[] = "This is global string";
int c;
int a = 5;
const int b = 5;

int main() {
    int pid = getpid();
foo:
    printf("process id: %d\n", pid);
    printf("global string: %p\n", &global);
    printf("main() is: %p\n", (void*)main);
    printf("the code: %p\n", &&foo);
    printf("int a = %d; is: %p\n", a, &a);
    printf("int c; is: %p\n", &c);
    printf("const int b = %d; is: %p\n", b, &b);
    
    printf("\n\n /proc/%d/maps \n\n", pid);
    char command[50];
    sprintf(command, "cat /proc/%d/maps", pid);
    system(command);
    return 0;
}
