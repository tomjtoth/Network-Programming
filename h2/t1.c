#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    int i;
    fork();
    for (i = 1; i <= 5; i++)
        printf(" i = %d \n", i);
    exit(0);
}
