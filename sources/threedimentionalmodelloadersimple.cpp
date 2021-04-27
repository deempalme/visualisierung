#include "headers/threedimentionalmodelloadersimple.h"

ThreeDimentionalModelLoaderSimple::ThreeDimentionalModelLoaderSimple(unsigned int _type,
                                                                     QOpenGLShaderProgram *_program) :
  m_program(_program),
  scalation(1, 1, 1),
  dataSize(0),
  type(_type)
{
  modelData.create();

  switch(_type){
  case DB5:
    fileString = "db5";
  break;
  case WINDOWS:
    fileString = "db5";
  break;
  case SHUTTLE:
    fileString = "shuttle_body";
  break;
  case SHUTTLE_CHASSIS:
    fileString = "shuttle";
  break;
  case SHUTTLE_WINDOWS:
    fileString = "shuttle_windows";
  break;
  case STREET_SCOOTER:
    //street scooter position:
    //latitude: 50.78418
    //longitude: 6.042664
    fileString = "street_scooter";
  break;
  case CAMPUS_BOULEVAR:
    //campus boulevar position:
    //latitude: 50.78236
    //longitude: 6.046611
    fileString = "campus_boulevar";
  break;
  }

  fileName = E_MODELS3D + fileString;
  if(_type == WINDOWS) fileName += "windows";
  fileName += ".obj";

  i_position = m_program->attributeLocation("i_position");
  i_normal = m_program->attributeLocation("i_normal");
  i_uv = m_program->attributeLocation("i_uv");
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

ThreeDimentionalModelLoaderSimple::~ThreeDimentionalModelLoaderSimple(){
  modelData.destroy();
}

void ThreeDimentionalModelLoaderSimple::changePosition(const float _xposition, const float _yposition, const float _zposition){
  translation = QVector3D(-_yposition, _zposition, -_xposition);
  m_model.setToIdentity();
  m_model.translate(translation);
  m_model.scale(scalation);

  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderSimple::applyAcceleration(const float _acceleration){
  QVector3D translation2 = QVector3D(translation[0], translation[1] + _acceleration, translation[2]);
  m_model.setToIdentity();
  m_model.translate(translation2);
  m_model.scale(scalation);

  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderSimple::changeRotation(const float _pitch, const float _yaw, const float _roll){
  m_model.setToIdentity();
  m_model.translate(translation);
  m_model.scale(scalation);

  rotation = QQuaternion::fromEulerAngles(_pitch, _yaw, _roll);
  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderSimple::changeSize(const float _width, const float _height, const float _length){
  scalation = QVector3D(_width, _height, _length);
  m_model.setToIdentity();
  m_model.translate(translation);
  m_model.scale(scalation);
  m_model.rotate(rotation);
  m_normal = m_model.normalMatrix();
}

void ThreeDimentionalModelLoaderSimple::drawModel(){
  modelData.bind();

  m_program->setUniformValue(u_model, m_model);
  m_program->setUniformValue(u_normal, m_normal);
  m_program->setUniformValue(u_model2, m_model2);
  m_program->setUniformValue(u_normal2, m_normal2);

  // Offset for position
  quintptr offset = 0;

  //Tell OpenGL programmable pipeline where find vertex positions
  m_program->setAttributeBuffer(i_position, GL_FLOAT, offset, 3, sizeof(ModelDataSimple));

  offset += sizeof(QVector3D);
  m_program->setAttributeBuffer(i_uv, GL_FLOAT, offset, 2, sizeof(ModelDataSimple));

  offset += sizeof(QVector2D);
  m_program->setAttributeBuffer(i_normal, GL_FLOAT, offset, 3, sizeof(ModelDataSimple));

  m_program->enableAttributeArray(i_position);
  m_program->enableAttributeArray(i_uv);
  m_program->enableAttributeArray(i_normal);

  //Draw cube geometry using indices from VBO
  glDrawArrays(GL_TRIANGLES, 0, dataSize);

  m_program->disableAttributeArray(i_normal);
  m_program->disableAttributeArray(i_uv);
  m_program->disableAttributeArray(i_position);
}

bool ThreeDimentionalModelLoaderSimple::readData(){
  QVector<QVector3D> position, normal;
  QVector<QVector2D> uv;
  QVector3D tvector;
  QVector2D ttexture;
  QVector<unsigned int> vertexIndices, uvIndices, normalIndices;
  unsigned int vertexIndex, uvIndex, normalIndex;
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
        for(int i = 1; i <= 3; i++){
          QStringList tindices = QString(tdata[i]).split("/");
          vertexIndex = QString(tindices[0]).toUInt();
          uvIndex = QString(tindices[1]).toUInt();
          normalIndex = QString(tindices[2]).toUInt();

          vertexIndices.append(vertexIndex);
          uvIndices.append(uvIndex);
          normalIndices.append(normalIndex);
        }
      }
    }
    file.close();

    vector<ModelDataSimple> allData(vertexIndices.size());
    // For each vertex of each triangle
    for(int i = 0; i < vertexIndices.size(); i++){
      allData[i].position = position[vertexIndices[i] - 1];
      allData[i].normal = normal[normalIndices[i] - 1];
      if(uvIndices[i] > 0)
        allData[i].uv = uv[uvIndices[i] - 1];
      else
        allData[i].uv = QVector2D();
    }

    dataSize = vertexIndices.size();
    modelData.bind();
    modelData.allocate(allData.data(), dataSize * sizeof(ModelDataSimple));

    return true;
  }else
    return false;
}
