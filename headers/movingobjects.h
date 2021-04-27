#ifndef MOVINGOBJECTS_H
#define MOVINGOBJECTS_H

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QVector>
#include <vector>

#include <QElapsedTimer>

#include "headers/definitions.h"

using std::vector;

struct ObjectData{
  QVector3D position;
  QVector3D color;
  QVector3D normal;
};

class MovingObjects{
public:
    MovingObjects(QOpenGLShaderProgram *_program, const vector<Obstacle> *const _obstacles);
    ~MovingObjects();
    //in the case you want to change the input pointers you should call this function
    void changeInput(const vector<Obstacle> *const _newInputObstacle);
    //set the tranformation matrix as a POINTER, the POINTER has a bigger priority
    //than the normal varibale, if you define both, the pointer will prevail
    void setTransformationMatrix(const QMatrix4x4 *const _transformationMatrix);
    void translateObstacleMatrix(const float _translationX = 0, const float _translationY = 0, const float _translationZ = 0);
    void rotateObstacleMatrix(const float _pitch = 0, const float _yaw = 0, const float _roll = 0);
    //function to draw the obstacles into screen
    void drawObstacle();
    //function to update the boxes, if you only update the data in the vector _obstacles the information
    //of the boxes will not be updated, so it is necessary to call this function too
    void updateObstacle();

private:
    QOpenGLShaderProgram *m_program;
    unsigned int i_position, i_color, i_normal, u_model, u_vehicle, sizeObstacles, sizeArrows;
    QOpenGLBuffer obstacleBuffer, indexObstacleBuffer, arrowsBuffer, indexArrowsBuffer;

    QVector3D nP[8], nL[12], nN2[6];
    int nF1[24], nF2[18], nF3[12];
    ObjectData arrow[42];

    //this is a pointer which will connect to the input data without making any copies
    const vector<Obstacle> *obstacles;
    //this is a pointer to the transformation matrix of the obstacles
    const QMatrix4x4 *transformationMatrixPtr;
    QMatrix4x4 transformationMatrix, emptyMatrx;
};

#endif // MOVINGOBJECTS_H
