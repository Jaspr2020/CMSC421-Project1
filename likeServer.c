#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int test();

int main() {
    test();
    return 0;
}

int test() {
    fork();
    printf("Hello World!");
    return 0;
}