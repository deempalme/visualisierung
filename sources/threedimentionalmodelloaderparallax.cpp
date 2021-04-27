#include "headers/threedimentionalmodelloaderparallax.h"

#include <math.h>

ThreeDimentionalModelLoaderParallax::ThreeDimentionalModelLoaderParallax(unsigned int _type, QOpenGLShaderProgram *_program) :
  m_program(_program),
  scalation(1, 1, 1),
  dataSize(0),
  type(_type),
  left(false)
{
  modelData.create();

  switch(_type){
  case TIRE_LEFT:
    fileString = "tire";
    left = true;
  break;
  case TIRE_RIGHT:
    fileString = "tire";
  break;
  }

  fileName = E_MODELS3D + fileString + ".obj";

  i_position = m_program->attributeLocation("i_position");
  i_uv = m_program->attributeLocation("i_uv");
  i_normal = m_program->attributeLocation("i_normal");
  i_tangent = m_program->attributeLocation("i_tangent");
  i_bitangent = m_program->attributeLocation("i_bitangent");
  u_normal = m_program->uniformLocation("u_normal_mat");
  u_model = m_program->uniformLocation("u_model_mat");
  u_scale = m_program->uniformLocation("parallaxScale");
  m_normal = m_model.normalMatrix();

  readData();
}

ThreeDimentionalModelLoaderParallax::~ThreeDimentionalModelLoaderParallax(){
  modelData.destroy();
}

void ThreeDimentionalModelLoaderParallax::changePosition(const float _xposition, const float _yposition, const float _zposition){
  translation = QVector3D(-_yposition, _zposition, -_xposition);
  m_model.setToIdentity();
  m_model.translate(translation);
  m_model.scale(scalation);
  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderParallax::changeRotation(const float _pitch, const float _yaw, const float _roll){
  m_model.setToIdentity();
  m_model.translate(translation);
  m_model.scale(scalation);

  rotation = QQuaternion::fromEulerAngles(_pitch, _yaw, _roll);
  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderParallax::changeSize(float _width, float _height, float _length){
  if(fileString == "tire") _width /= 0.2;
  if(_length < 0)
    _length = _height;

  scalation = QVector3D(_width, _height, _length);
  m_model.setToIdentity();
  m_model.translate(translation);
  m_model.scale(scalation);
  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderParallax::drawModel(){
  modelData.bind();

  m_program->setUniformValue(u_model, m_model);
  m_program->setUniformValue(u_normal, m_normal);
  m_program->setUniformValue(u_scale, 0.05f);

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

bool ThreeDimentionalModelLoaderParallax::readData(){
  QVector<QVector3D> position(0), normal(0), fnormal(0), tangent(0), bitangent(0);
  QVector<QVector2D> uv(0);
  QVector3D tvector, deltaPos1, deltaPos2;
  QVector2D ttexture, deltaUV1, deltaUV2;
  vector<unsigned int> vertexIndices(0), uvIndices(0), normalIndices(0);
  unsigned int vertexIndex, uvIndex, normalIndex, posIndex;
  float r;

  QFile file(fileName);
  if(file.open(QFile::ReadOnly)){
    while(!file.atEnd()){
      QStringList tdata = QString(file.readLine()).split(" ");
      if(tdata[0] == "v"){
        tvector[0] = QString(tdata[1]).toFloat();
        tvector[1] = QString(tdata[2]).toFloat();
        tvector[2] = QString(tdata[3]).toFloat();
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

          vertexIndices.push_back(vertexIndex);
          uvIndices.push_back(uvIndex);
          normalIndices.push_back(normalIndex);
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

    dataSize = vertexIndices.size();
    vector<ModelDataComplex> allData(dataSize);
    uint i3(0);

    // For each vertex of each triangle
    for(int i = 0; i < dataSize; i++){
      i3 = static_cast<uint>(std::floor(i/3));
      allData[i].position = position[vertexIndices[i] - 1];
      if(uvIndices[i] > 0)
        allData[i].uv = uv[uvIndices[i] - 1];
      else
        allData[i].uv = QVector2D();
      allData[i].normal = fnormal[i3];
      //allData[i].normal = normal[normalIndices[i] - 1];
      allData[i].tangent = tangent[i3];
      allData[i].bitangent = bitangent[i3];
    }

    modelData.bind();
    modelData.allocate(allData.data(), dataSize * sizeof(ModelDataComplex));

    return true;
  }else{
    qWarning() << "file not open:" << fileName;
    return false;
  }
}
