#ifndef HEIGHTFIELD_H_
#define HEIGHTFIELD_H_

double **createHeightfield(const int size);
void freeHeightfield(double **a, const int size);
void outputHeightfieldAsImage(double **a, const int size, const double maxHeight, char *filename);
double **importImageAsHeightfield(char *filename, int *size, const double maxHeight);

#endif