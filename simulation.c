#include <stdlib.h>
#include <stdio.h>

#include "heightfield.h"
#include "vector.h"

float min(float a, float b){
    return (a < b ? a : b);
}

float max(float a, float b){
    return (a > b ? a : b);
}

int getNeighbours(Vector p, const int size, Vector *neighbours){
    int count = 0;
    for (int y = 0; y < 3; y++){
        for (int x = 0; x < 3; x++){
            int nx = p.x + x - 1;
            int ny = p.y + y - 1;

            if (y == 1 && x == 1){
                neighbours[4] = vector2(-1.0f, -1.0f);
            }
            else if (nx < 0 || ny < 0 || (nx >= size) || (ny >= size)){
                neighbours[y * 3 + x] = vector2(-1.0f, -1.0f);
            }
            else{
                neighbours[y * 3 + x] = vector2(nx, ny);
                count++;
            }
            
        }
    }
    
    return count;
}

void smooth(float **heightfield, const int size){
    for (int z = 0; z < size; z++){
        for (int x = 0; x < size; x++){
            float avg = 0.0f;
            Vector neighbours[9];
            int neighboursCount = getNeighbours(vector2(x, z), size, neighbours);
            for (int n = 0; n < 9; n++){
                if (neighbours[n].x < 0){
                    // Edge
                    continue;
                }
                avg += heightfield[(int)neighbours[n].y][(int)neighbours[n].x];
            }
            avg /= neighboursCount;
            float a = 1.0f;
            heightfield[z][x] = a * avg + (1.0f - a) * heightfield[z][x];
        }
    }
}

typedef struct {
    float height;
    float water;
    float sediment;
} Vertex;

void hydraulicErosion(float **heightfield, const int size, const int iterations, const float maxSedimentCapacity, const float depositionRate, const float soilSoftness){
    // Allocates memory for a erosion vertex field
    Vertex **grid = (Vertex**)malloc(size * sizeof(Vertex*));
    for (int z = 0; z < size; z++){
        grid[z] = (Vertex*)malloc(size * sizeof(Vertex));
        for (int x = 0; x < size; x++){
            grid[z][x].height = heightfield[z][x];//(float)((z)/(float)size * 20);//
            grid[z][x].water = 0.0f;
            grid[z][x].sediment = 0.0f;
        }
    }

    Vertex **gridUpdate = (Vertex**)malloc(size * sizeof(Vertex*));
    for (int z = 0; z < size; z++){
        gridUpdate[z] = (Vertex*)malloc(size * sizeof(Vertex));
        for (int x = 0; x < size; x++){
            Vertex zeroVertex = {0.0f, 0.0f, 0.0f};
            gridUpdate[z][x] = zeroVertex;
        }
    }

    for (int i = 0; i < iterations; i++){
        if (i % (iterations/100) == 0){
            int pct = ((float) i / (float)iterations) * 100;
            printf("Simulating Erosion: (%3d%%)\r", pct);
            fflush(stdout);
        }
        
        //Rainfall
        if (i % 10 == 0){
            for (int z = 0; z < size; z++){
                for (int x = 0; x < size; x++){   
                    grid[z][x].water += grid[z][x].height * .005f;
                }
            }
        }

        // Erosion
        for (int z = 0; z < size; z++){
            for (int x = 0; x < size; x++){
                Vertex v = grid[z][x];
                Vertex vNew = {0.0f, 0.0f, 0.0f};

                Vector neighbours[9];
                int neighboursCount = getNeighbours(vector2(x, z), size, neighbours);
                float deltaWaterTotal = 0.0f;
                float heightDifferenceTotal = 0.0f;
                for (int n = 0; n < 9; n++){
                    if (neighbours[n].x < 0){
                        // Edge
                        continue;
                    }

                    int nx = neighbours[n].x;
                    int nz = neighbours[n].y;
                    Vertex u = grid[nz][nx];

                    float deltaWater = (v.height + v.water) - (u.height + u.water);

                    if(deltaWater > 0) {
                        deltaWaterTotal += deltaWater;
                    }
                    else{
                        // Higher
                        neighbours[n] = vector2(-1.0f, -1.0f);
                    }
                }
                  
                if (deltaWaterTotal <= 0.0f){
                    vNew.height += depositionRate * v.sediment;
                    vNew.sediment += (1.0f - depositionRate) * v.sediment - v.sediment;
                }

                for (int n = 0; n < 9; n++){
                    if (neighbours[n].x < 0){
                        // Edge
                        continue;
                    }
                    
                    int nx = neighbours[n].x;
                    int nz = neighbours[n].y;
                    Vertex u = grid[nz][nx];
                    Vertex uNew = {0.0f, 0.0f, 0.0f};

                    float deltaWater = (v.height + v.water) - (u.height + u.water);
                    deltaWater = min(v.water, deltaWater) * deltaWater / deltaWaterTotal;

                    if (deltaWater <= 0.0f){
                        continue;
                    }

                    vNew.water += -deltaWater;
                    uNew.water += deltaWater;

                    float sedimentCapacity = maxSedimentCapacity * deltaWater;

                    if (v.sediment > sedimentCapacity){
                        uNew.sediment += sedimentCapacity;
                        vNew.height += depositionRate * (v.sediment - sedimentCapacity);
                        vNew.sediment += (1.0f - depositionRate) * (v.sediment - sedimentCapacity) - v.sediment;
                    }
                    else{
                        uNew.sediment += v.sediment + soilSoftness * (sedimentCapacity - v.sediment);
                        vNew.height += -soilSoftness * (sedimentCapacity - v.sediment);
                        vNew.sediment += -v.sediment;
                    }

                    // Save updated values for vertex u
                    gridUpdate[nz][nx] = uNew;

                    // Save updated values for vertex u
                    gridUpdate[nz][nx].water += uNew.water;
                    gridUpdate[nz][nx].sediment += uNew.sediment;
                }

                // Save updated values for vertex v
                gridUpdate[z][x].height += vNew.height;
                gridUpdate[z][x].water += vNew.water;
                gridUpdate[z][x].sediment += vNew.sediment;
            }
        }

        for (int z = 0; z < size; z++){ 
            for (int x = 0; x < size; x++){
                grid[z][x].height += gridUpdate[z][x].height;
                grid[z][x].water += gridUpdate[z][x].water;
                grid[z][x].sediment += gridUpdate[z][x].sediment;

                grid[z][x].height = max(0.0f, grid[z][x].height);
                grid[z][x].water = max(0.0f, grid[z][x].water);
                grid[z][x].sediment = max(0.0f, grid[z][x].sediment);

                Vertex zeroVertex = {0.0f, 0.0f, 0.0f};
                gridUpdate[z][x] = zeroVertex;
            }
        }   
    }

    for (int z = 0; z < size; z++){
        for (int x = 0; x < size; x++){
            heightfield[z][x] = grid[z][x].height;
        }
    }

    for (int z = 0; z < size; z++){
        free(grid[z]);
        free(gridUpdate[z]);
    }
    free(grid);
    free(gridUpdate);

    smooth(heightfield, size);
}

// void hydraulicErosion(float **heightfield, const int size, const int iterations, const float maxSedimentCapacity, const float depositionRate, const float soilSoftness){
//     // Allocates memory for a erosion vertex field
//     Vertex **grid = (Vertex**)malloc(size * sizeof(Vertex*));
//     for (int z = 0; z < size; z++){
//         grid[z] = (Vertex*)malloc(size * sizeof(Vertex));
//         for (int x = 0; x < size; x++){
//             grid[z][x].height = heightfield[z][x];//(float)((z)/(float)size * 20);//
//             grid[z][x].water = 0.0f;
//             grid[z][x].sediment = 0.0f;
//         }
//     }

//     Vertex **gridUpdate = (Vertex**)malloc(size * sizeof(Vertex*));
//     for (int z = 0; z < size; z++){
//         gridUpdate[z] = (Vertex*)malloc(size * sizeof(Vertex));
//         for (int x = 0; x < size; x++){
//             gridUpdate[z][x] = grid[z][x];
//         }
//     }

//     for (int i = 0; i < iterations; i++){
//         // Rainfall
//         if (i % 30 == 0){
//             for (int z = 0; z < size; z++){
//                 for (int x = 0; x < size; x++){   
//                     grid[z][x].water += grid[z][x].height * .005f;
//                 }
//             }
//         }

//         // Erosion
//         for (int z = 0; z < size; z++){
//             for (int x = 0; x < size; x++){
//                 Vertex v = grid[z][x];
//                 Vertex vNew;

//                 Vector neighbours[9];
//                 int neighboursCount = getNeighbours(vector2(x, z), size, neighbours);
//                 float deltaWaterTotal = 0.0f;
//                 float heightDifferenceTotal = 0.0f;
//                 for (int n = 0; n < 9; n++){
//                     if (neighbours[n].x < 0){
//                         // Edge
//                         continue;
//                     }

//                     int nx = neighbours[n].x;
//                     int nz = neighbours[n].y;
//                     Vertex u = grid[nz][nx];

//                     float deltaWater = (v.height + v.water) - (u.height + u.water);

//                     if(deltaWater > 0) {
//                         deltaWaterTotal += deltaWater;
//                     }
//                     else{
//                         // Higher
//                         neighbours[n] = vector2(-1.0f, -1.0f);
//                     }
//                 }
                  
//                 if (deltaWaterTotal <= 0.0f){
//                     vNew.height = v.height + depositionRate * v.sediment;
//                     vNew.sediment = (1.0f - depositionRate) * v.sediment;
//                 }

//                 for (int n = 0; n < 9; n++){
//                     if (neighbours[n].x < 0){
//                         // Edge
//                         continue;
//                     }
                    
//                     int nx = neighbours[n].x;
//                     int nz = neighbours[n].y;
//                     Vertex u = grid[nz][nx];
//                     Vertex uNew;

//                     float deltaWater = (v.height + v.water) - (u.height + u.water);
//                     deltaWater = min(v.water, deltaWater) * deltaWater / deltaWaterTotal;

//                     vNew.water = v.water -deltaWater;
//                     uNew.water = u.water + deltaWater;

//                     float sedimentCapacity = maxSedimentCapacity * deltaWater;

//                     if (v.sediment >= sedimentCapacity){
//                         uNew.sediment = u.sediment + sedimentCapacity;
//                         vNew.height = v.height + depositionRate * (v.sediment - sedimentCapacity);
//                         vNew.sediment = (1.0f - depositionRate) * (v.sediment - sedimentCapacity);
//                     }
//                     else{
//                         uNew.sediment = u.sediment + v.sediment + soilSoftness * (sedimentCapacity - v.sediment);
//                         vNew.height = v.height - soilSoftness * (sedimentCapacity - v.sediment);
//                         vNew.sediment = 0.0f;
//                     }

//                     // Thermal
//                     // float talusAngle = 0.3;

//                     // if (vNew.height - uNew.height > talusAngle){
//                     //     uNew.height += 0.001 * (vNew.height - uNew.height - talusAngle);
//                     // }

//                     // Save updated values for vertex u
//                     gridUpdate[nz][nx] = uNew;
//                 }

//                 // Save updated values for vertex v
//                 gridUpdate[z][x] = vNew;
//                 //gridUpdate[z][x].height = max(0.0f, vNew.height);
//             }
//         }
//     }

//     for (int z = 0; z < size; z++){
//         for (int x = 0; x < size; x++){
//             heightfield[z][x] = grid[z][x].height;
//         }
//     }

//     for (int z = 0; z < size; z++){
//         free(grid[z]);
//         free(gridUpdate[z]);
//     }
//     free(grid);
//     free(gridUpdate);
// }
