#ifndef STREAM_GRAPH_H_
#define STREAM_GRAPH_H_

#include "stream_node.h"
#include <vector>
#include <tuple>

#include "../core/mesh.h"

Vector circumcentreOfTriangle(Vector a, Vector b, Vector c);
float areaOfTriangle(Vector a, Vector b, Vector c);

class StreamGraph{
public:
    int terrainSize;

    float **upliftField;
    int upliftFieldSize;

    bool variableRainfall;
    float **rainfallField;
    int rainfallFieldSize;

    float timeStep;

    std::vector<StreamNode> nodes;
    std::vector<Triangle> triangles;
    std::vector<std::tuple<int, int>> edges;

    std::vector<LakeNode*> lakeGraph;

    StreamGraph(int terrainSize, float timeStep, float **upliftField, int upliftFieldSize, bool variableRainfall, float **rainfallField, int rainfallFieldSize){
        this->terrainSize = terrainSize;
        this->timeStep = timeStep;
        this->upliftField = upliftField;
        this->upliftFieldSize = upliftFieldSize;
        this->variableRainfall = variableRainfall;
        this->rainfallField = rainfallField;
        this->rainfallFieldSize = rainfallFieldSize;
    }

    StreamGraph(){

    }
    
    void initialise(int nodeCount, float m, float n, float k, float convergenceThreshold, float minimumTalusAngle, float maximumTalusAngle);
    float getUplift(Vector p);
    void voronoiTessellation();
    void createStreamTrees();
    bool update();
    void calculatePasses();

    Mesh *createMesh();
    float **createHightfield(float precision, float sigma, float *maxHeight);

    float getRainfall(Vector p);
};

#endif
