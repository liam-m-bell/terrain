#ifndef MUSGRAVE_H_
#define MUSGRAVE_H_

void musgraveErosion(float **heightfield, const int size, const int iterations, const float maxSedimentCapacity, const float depositionRate, const float soilSoftness, const int rainFrequency, const int rainVolume, const float talusAngle, const float thermalAmount);

#endif