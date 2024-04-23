#ifndef HEIGHTFIELD_H_
#define HEIGHTFIELD_H_

float **createHeightfield(const int size);
void freeHeightfield(float **a, const int size);
void outputHeightfieldAsImage(float **a, const int size, const float maxHeight, char *filename);
float **importImageAsHeightfield(char *filename, int *size, const float maxHeight);

#endif