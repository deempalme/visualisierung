#include "headers/threedimentionalmodelloadernormals.h"

#include <math.h>

ThreeDimentionalModelLoaderNormals::ThreeDimentionalModelLoaderNormals(unsigned int _type,
                                                                       QOpenGLShaderProgram *_program) :
  m_program(_program),
  acceleration(),
  scalation(1, 1, 1),
  dataSize(0),
  type(_type),
  width(-1),
  length(-1),
  height(-1),
  left(false)
{
  modelData.create();

  switch(_type){
  case CHASSIS:
    fileString = "car";
  break;
  case STEERING:
    fileString = "steering";
  break;
  case COORDINATES:
    fileString = "coordinates";
  break;
  case BAR:
    fileString = "bar";
  break;
  }

  fileName = E_MODELS3D + fileString + ".obj";

  i_position = m_program->attributeLocation("i_position");
  i_uv = m_program->attributeLocation("i_uv");
  i_normal = m_program->attributeLocation("i_normal");
  i_tangent = m_program->attributeLocation("i_tangent");
  i_bitangent = m_program->attributeLocation("i_bitangent");
  u_normal = m_program->uniformLocation("u_normal_mat");
  u_normal2 = m_program->uniformLocation("u_normal_mat2");
  u_model = m_program->uniformLocation("u_model_mat");
  u_model2 = m_program->uniformLocation("u_model_mat2");

  m_program->setUniformValue(u_model2, m_model2);
  m_normal2 = m_model2.normalMatrix();
  m_program->setUniformValue(u_normal2, m_normal2);

  m_normal = m_model.normalMatrix();

  readData();
}

ThreeDimentionalModelLoaderNormals::~ThreeDimentionalModelLoaderNormals(){
  modelData.destroy();
}

void ThreeDimentionalModelLoaderNormals::changePosition(const float _xposition, const float _yposition, const float _zposition){
  translation = QVector3D(-_yposition, _zposition, -_xposition);
  m_model.setToIdentity();
  m_model.translate(translation + acceleration);
  m_model.scale(scalation);

  if(type == STEERING){
    m_model2.setToIdentity();
    m_model2.rotate(rotation2);
    m_normal2 = m_model2.normalMatrix();
  }

  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderNormals::applyAcceleration(const float _acceleration){
  acceleration[1] = _acceleration;
  m_model.setToIdentity();
  m_model.translate(translation + acceleration);
  m_model.scale(scalation);

  if(type == STEERING){
    m_model2.setToIdentity();
    m_model2.rotate(rotation2);
    m_normal2 = m_model2.normalMatrix();
  }

  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderNormals::changeRotation(const float _pitch, const float _yaw, const float _roll){
  m_model.setToIdentity();
  m_model.translate(translation + acceleration);
  m_model.scale(scalation);

  if(type == STEERING){
    m_model2.setToIdentity();
    rotation2 = QQuaternion::fromEulerAngles(0, _yaw, 0);
    m_model2.rotate(rotation2);
    m_normal2 = m_model2.normalMatrix();
    rotation = QQuaternion::fromEulerAngles(_pitch, 0, _roll);
  }else{
    rotation = QQuaternion::fromEulerAngles(_pitch, _yaw, _roll);
  }
  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderNormals::changeSize(const float _width, const float _height, const float _length){
  scalation = QVector3D(_width, _height, _length);
  m_model.setToIdentity();
  m_model.translate(translation);
  m_model.scale(scalation);
  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderNormals::changeDimensions(const float _width, const float _length, const float _height){
  if(_width > 0)
    width = _width;
  if(_length > 0)
    length = _length;
  if(_height > 0)
    height = _height;

  readData(true);
}

void ThreeDimentionalModelLoaderNormals::changeChassis(const float _A, const float _B, const float _C,
                                                const float _D, const float _E, const float _F){
  readData(true, _A, _B, _C, _D, _E, _F);
}

void ThreeDimentionalModelLoaderNormals::drawModel(){
  modelData.bind();

  m_program->setUniformValue(u_model, m_model);
  m_program->setUniformValue(u_normal, m_normal);
  m_program->setUniformValue(u_model2, m_model2);
  m_program->setUniformValue(u_normal2, m_normal2);

  // Offset for position
  quintptr offset = 0;

  //Tell OpenGL programmable pipeline where find vertex positions
  m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(ModelDataComplex));

  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_uv, GL_FLOAT, offset, 2, sizeof(ModelDataComplex));

  offset += sizeof(QVector2D);
  m_program->setAttributeBuffer(i_normal, GL_FLOAT, offset, 3, sizeof(ModelDataComplex));

  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_tangent, GL_FLOAT, offset, 3, sizeof(ModelDataComplex));

  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_bitangent, GL_FLOAT, offset, 3, sizeof(ModelDataComplex));

  m_program->enableAttributeArray(i_position);
  m_program->enableAttributeArray(i_uv);
  m_program->enableAttributeArray(i_normal);
  m_program->enableAttributeArray(i_tangent);
  m_program->enableAttributeArray(i_bitangent);

  //Draw cube geometry using indices from VBO
  glDrawArrays(GL_TRIANGLES, 0, dataSize);

  m_program->disableAttributeArray(i_bitangent);
  m_program->disableAttributeArray(i_tangent);
  m_program->disableAttributeArray(i_normal);
  m_program->disableAttributeArray(i_uv);
  m_program->disableAttributeArray(i_position);
}

bool ThreeDimentionalModelLoaderNormals::readData(const bool _dimensions, const float _A, const float _B,
                                           const float _C, const float _D, const float _E, const float _F){
  QVector<QVector3D> position, normal, fnormal, tangent, bitangent;
  QVector<QVector2D> uv;
  QVector3D tvector, deltaPos1, deltaPos2;
  QVector2D ttexture, deltaUV1, deltaUV2;
  QVector<unsigned int> vertexIndices, uvIndices, normalIndices;
  unsigned int vertexIndex, uvIndex, normalIndex, posIndex;
  float r;
  float widthT = (width - 0.2f)/2.0f;

  QFile file(fileName);
  if(file.open(QFile::ReadOnly)){
    while(!file.atEnd()){
      QStringList tdata = QString(file.readLine()).split(" ");
      if(tdata[0] == "v"){
        tvector[0] = QString(tdata[1]).toFloat();
        tvector[1] = QString(tdata[2]).toFloat();
        tvector[2] = QString(tdata[3]).toFloat();
        if(_dimensions){
          switch(type){
          case CHASSIS:
            if(tvector[2] < -1.8f)
              tvector[2] += 0.20f - _A;
            if(tvector[2] < -1.45f)
              tvector[2] += 0.65f - _B;
            if(tvector[2] < 0.0f)
              tvector[2] += 1.15f - _C;

            if(tvector[2] > 1.80f)
              tvector[2] += _E - 0.20f;
            if(tvector[2] > 1.34f)
              tvector[2] += _B - 0.65f;
            if(tvector[2] > 0.0f)
              tvector[2] += _D - 1.15f;
            if(tvector[0] < 0.0f)
              tvector[0] += 0.75f - _F/2.0f;
            if(tvector[0] > 0.0f)
              tvector[0] += _F/2.0f - 0.75f;
          break;
          default:
            if((length > 0) && (width > 0)){
              tvector[0] *= length;
              tvector[1] *= length;
              if(tvector[2] > 0.0f)
                tvector[2] += widthT;
              else
                tvector[2] -= widthT;
            }
          break;
          }
        }
        position.append(tvector);
      }else if(tdata[0] == "vt"){
        ttexture[0] = QString(tdata[1]).toFloat();
        ttexture[1] = QString(tdata[2]).toFloat();
        uv.append(ttexture);
      }else if(tdata[0] == "vn"){
        tvector[0] = QString(tdata[1]).toFloat();
        tvector[1] = QString(tdata[2]).toFloat();
        tvector[2] = QString(tdata[3]).toFloat();
        normal.append(tvector);
      }else if(tdata[0] == "f"){
        QVector3D averageNormal;
        for(int i = 1; i <= 3; i++){
          QStringList tindices = QString(tdata[i]).split("/");
          vertexIndex = QString(tindices[0]).toUInt();
          uvIndex = QString(tindices[1]).toUInt();
          normalIndex = QString(tindices[2]).toUInt();

          vertexIndices.append(vertexIndex);
          uvIndices.append(uvIndex);
          normalIndices.append(normalIndex);
          averageNormal += normal[normalIndex - 1];
        }
        averageNormal /= 3;

        posIndex = vertexIndices.size() - 1;
        deltaPos1 = position[vertexIndices[posIndex] - 1] - position[vertexIndices[posIndex - 2] - 1];
        deltaPos2 = position[vertexIndices[posIndex - 1] - 1] - position[vertexIndices[posIndex - 2] - 1];

        if(uvIndices[posIndex] > 0 && uvIndices[posIndex - 2] > 0){
          deltaUV1 = uv[uvIndices[posIndex] - 1] - uv[uvIndices[posIndex - 2] - 1];
          deltaUV2 = uv[uvIndices[posIndex - 1] - 1] - uv[uvIndices[posIndex - 2] - 1];
        }

        r = 1.0f / (deltaUV1.x() * deltaUV2.y() - deltaUV1.y() * deltaUV2.x());
        fnormal.append(averageNormal.normalized());
        tangent.append(QVector3D((deltaPos1 * deltaUV2.y() - deltaPos2 * deltaUV1.y()) * r).normalized());
        bitangent.append(QVector3D((deltaPos2 * deltaUV1.x() - deltaPos1 * deltaUV2.x()) * r).normalized());
      }
    }
    file.close();

    vector<ModelDataComplex> allData(vertexIndices.size());
    // For each vertex of each triangle
    uint i3(0);
    for(int i = 0; i < vertexIndices.size(); i++){
      i3 = static_cast<uint>(std::floor(i/3));
      allData[i].position = position[vertexIndices[i] - 1];
      if(uvIndices[i] > 0)
        allData[i].uv = uv[uvIndices[i] - 1];
      else
        allData[i].uv = QVector2D();
      //allData[i].normal = fnormal[i3];
      allData[i].normal = normal[normalIndices[i] - 1];
      allData[i].tangent = tangent[i3];
      allData[i].bitangent = bitangent[i3];
    }

    dataSize = vertexIndices.size();
    modelData.bind();
    modelData.allocate(allData.data(), dataSize * sizeof(ModelDataComplex));

    return true;
  }else
    return false;
}
