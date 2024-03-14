#ifndef POISSON_DISK_SAMPLING_H_
#define POISSON_DISK_SAMPLING_H_

#include <vector>
#include "../core/vector.h"

std::vector<Vector> poissonDiskSampling(float radius, Vector domainSize, int samplesBeforeRejection = 30);

#endif