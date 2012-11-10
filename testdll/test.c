#include <stdio.h>


static long long int Field = 42;

__declspec (dllexport) void test () {
    printf ("Hello World!\n");
}


__declspec (dllexport) long long int ReturnsStuff () {
    return Field;
}