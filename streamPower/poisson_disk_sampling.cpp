// // Using algorithm introduced by Robert Bridson (Fast Poisson Disk Sampling in Arbitrary Dimensions)
// // Implementation based on https://sighack.com/post/poisson-disk-sampling-bridsons-algorithm

// #include <stdlib.h>

// #include "poisson_disk_sampling.h"

// float randomRange(float min, float max){
//     return min + (max - min) * ((float)rand() / RAND_MAX);
// }

// bool isValidPoint(Vector point, float radius, Vector domainSize, std::vector<Vector> points, int** grid){
//     float cellSize = radius / sqrt(2.0f);

//     // Check in domain bounds
//     if (point.x >=0 && point.x < domainSize.x && point.y >= 0 && point.y < domainSize.y){
//         // Check neighbouring cells for points within radius
//         int cellX = (int)(point.x / cellSize);
//         int cellY = (int)(point.y / cellSize);
//         int startX = std::max(0, cellX - 1);
//         int endX = std::min(cellX + 1, (int)(domainSize.x / cellSize));
//         int startY = std::max(0, cellY - 1);
//         int endY = std::min(cellY + 1, (int)(domainSize.y / cellSize));

//         for (int i = startX; i <= endX; i++){
//             for (int j = startY; j <= endY; j++){
//                 int index = grid[j][i] -1;
//                 if (index != -1){
//                     float distanceSquared = (point - points[index]).lengthSquared();
//                     if (distanceSquared < pow(radius, 2)) {
//                         return false;
//                     }
//                 }
//             }
//         }
//         return true;
//     }

//     return false;
// }

// std::vector<Vector> poissonDiskSampling(float radius, Vector domainSize, int samplesBeforeRejection){
//     float cellSize = radius / sqrt(2.0f);

//     int **grid = (int**)malloc((int)std::ceil(domainSize.y / cellSize) * sizeof(int*));
//     for (int i = 0; i < domainSize.y / cellSize; i++){
//         grid[i] = (int*)calloc((int)std::ceil(domainSize.x / cellSize), sizeof(int));
//     }

//     std::vector<Vector> points;
//     std::vector<Vector> activePoints;

//     activePoints.push_back(0.5f * domainSize);

//     // While active list not empty
//     while (activePoints.size() > 0){
//         int index = (int)randomRange(0, activePoints.size());
//         Vector centre = activePoints[index];

//         bool pointValid = false;
// 		for (int i = 0; i < samplesBeforeRejection; i++){
//             // Spherical sampling
//             float angle = randomRange(0, 2.0f * M_PI);
//             Vector direction = Vector(cos(angle), sin(angle));
//             Vector samplePoint = randomRange(radius, 2.0f * radius) * direction + centre;

//             // Check if point is valid
//             if (isValidPoint(samplePoint, radius, domainSize, points, grid)){
//                 points.push_back(samplePoint);
//                 activePoints.push_back(samplePoint);
//                 grid[(int)(samplePoint.y / cellSize)][(int)(samplePoint.x / cellSize)] = points.size();
//                 pointValid = true;
//                 break;
//             }
//         }

//         // If no valid point found after max samples, instead remove original point from active list
//         if (!pointValid){
//             activePoints.erase(activePoints.begin() + index);
//         }
//     }

//     for (int i = 0; i < (int)std::ceil(domainSize.y / cellSize); i++){
//         free(grid[i]);
//     }
//     free(grid);

//     return points;
// } 
