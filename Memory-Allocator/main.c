#include "lib.h"

// tests
int main(){
    printf("Allocating 5 integers in array\n");
    int *arr = (int*)malloc(5 * sizeof(int));
    arr[0] = 6;
    arr[4] = 67;
    print_heap_status();


    printf("Reallocating size increased from 5 to 10\n");
    arr = (int*)realloc(arr, 10 * sizeof(int));
    arr[9] = 69;
    print_heap_status();


    printf("Free memory\n");
    free(arr);
    print_heap_status();


    printf("Allocating new memory (old memory block should be used in fact)\n");
    char *arr2 = (char*)malloc(5 * sizeof(char));
    print_heap_status();

    return 0;
}
