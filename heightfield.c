#include <stdlib.h>

// Allocates memory for a heightfield
float **createHeightfield(const int size){
    float **a = (float**)malloc(size * sizeof(float*));
    for (int i = 0; i < size; i++){
        a[i] = (float*)malloc(size * sizeof(float));
    }

    return a;
}

// Deallocates memory for a heightfield
void freeHeightfield(float **a, const int size){
    for (int i = 0; i < size; i++) {
        free(a[i]);
    }
    free(a);
}