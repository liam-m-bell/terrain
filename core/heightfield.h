#ifndef HEIGHTFIELD_H_
#define HEIGHTFIELD_H_

// Allocates memory for a heightfield
float **createHeightfield(const int size);

// Deallocates memory for a heightfield
void freeHeightfield(float **a, const int size);

void outputHeightfieldAsImage(float **a, const int size, const float maxHeight, char *filename);

#endif