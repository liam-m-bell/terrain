#ifndef STREAM_GRAPH_H_
#define STREAM_GRAPH_H_

#include "stream_node.h"
#include <vector>
#include <tuple>

#include "../core/mesh.h"

Vector circumcentreOfTriangle(Vector a, Vector b, Vector c);
double areaOfTriangle(Vector a, Vector b, Vector c);

class StreamGraph{
public:
    int terrainSize;

    double **upliftField;
    int upliftFieldSize;

    bool variableRainfall;
    double **rainfallField;
    int rainfallFieldSize;

    double timeStep;

    std::vector<StreamNode> nodes;
    std::vector<Triangle> triangles;
    std::vector<std::tuple<int, int>> edges;

    std::vector<LakeNode*> lakeGraph;

    StreamGraph(){

    }

    StreamGraph(int terrainSize, double timeStep, double **upliftField, int upliftFieldSize, bool variableRainfall, double **rainfallField, int rainfallFieldSize){
        this->terrainSize = terrainSize;
        this->timeStep = timeStep;
        this->upliftField = upliftField;
        this->upliftFieldSize = upliftFieldSize;
        this->variableRainfall = variableRainfall;
        this->rainfallField = rainfallField;
        this->rainfallFieldSize = rainfallFieldSize;
    }

    void initialise(int nodeCount, double m, double n, double k, double convergenceThreshold, double minimumTalusAngle, double maximumTalusAngle);
    double getUplift(Vector p);
    void voronoiTessellation();
    void createStreamTrees();
    bool update();
    void calculatePasses();

    Mesh *createMesh();
    double **createHightfield(double precision, double sigma, double *maxHeight);

    double getRainfall(Vector p);
};

#endif
