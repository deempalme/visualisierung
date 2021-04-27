#include "headers/skybox.h"

Skybox::Skybox(QOpenGLShaderProgram *_program) :
  m_program(_program),
  skyboxIndexBuffer(QOpenGLBuffer::IndexBuffer)
{
  initializeOpenGLFunctions();

  i_position = m_program->attributeLocation("i_position");

  glGenTextures(1, &ntexture);
  m_program->setUniformValue("skybox", ntexture);

  skyboxBuffer.create();
  skyboxIndexBuffer.create();

  //cube coordinates for the skybox creation
  GLfloat cube_vertices[24] = {
    -1.0,  1.0,  1.0,
    -1.0, -1.0,  1.0,
     1.0, -1.0,  1.0,
     1.0,  1.0,  1.0,
    -1.0,  1.0, -1.0,
    -1.0, -1.0, -1.0,
     1.0, -1.0, -1.0,
     1.0,  1.0, -1.0,
  };

  GLushort cube_indices[36] = {
    0, 0, 1, 3, 2, 2,
    3, 3, 2, 7, 6, 6,
    7, 7, 6, 4, 5, 5,
    4, 4, 5, 0, 1, 1,
    0, 0, 3, 4, 7, 7,
    1, 1, 2, 5, 6, 6
  };

  skyboxBuffer.create();
  skyboxBuffer.bind();
  skyboxBuffer.allocate(cube_vertices, 24 * sizeof(GLfloat));

  skyboxIndexBuffer.create();
  skyboxIndexBuffer.bind();
  skyboxIndexBuffer.allocate(cube_indices, 36 * sizeof(GLushort));

  // Loads a cubemap texture from 6 individual texture faces
  // Order should be:
  // +X (right)
  // -X (left)
  // +Y (top)
  // -Y (bottom)
  // +Z (front)
  // -Z (back)
  const QImage posx = QImage(":/images/c_lf").convertToFormat(QImage::Format_ARGB32);
  const QImage posy = QImage(":/images/c_up").convertToFormat(QImage::Format_ARGB32);
  const QImage posz = QImage(":/images/c_ft").convertToFormat(QImage::Format_ARGB32);
  const QImage negx = QImage(":/images/c_rt").convertToFormat(QImage::Format_ARGB32);
  const QImage negy = QImage(":/images/c_dn").convertToFormat(QImage::Format_ARGB32);
  const QImage negz = QImage(":/images/c_bk").convertToFormat(QImage::Format_ARGB32);

  const int sizex = posx.width();
  const int sizey = posx.height();
  glEnable(GL_TEXTURE_CUBE_MAP);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, ntexture);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, sizex, sizey, 0, GL_BGRA, GL_UNSIGNED_BYTE, posx.bits());
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, sizex, sizey, 0, GL_BGRA, GL_UNSIGNED_BYTE, negx.bits());
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, sizex, sizey, 0, GL_BGRA, GL_UNSIGNED_BYTE, posy.bits());
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, sizex, sizey, 0, GL_BGRA, GL_UNSIGNED_BYTE, negy.bits());
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, sizex, sizey, 0, GL_BGRA, GL_UNSIGNED_BYTE, posz.bits());
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, sizex, sizey, 0, GL_BGRA, GL_UNSIGNED_BYTE, negz.bits());

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Skybox::~Skybox(){
  skyboxBuffer.destroy();
  skyboxIndexBuffer.destroy();
}

void Skybox::drawSkybox(){
  // ----------------------------- drawing the skybox -------------------------------------
  skyboxIndexBuffer.bind();
  skyboxBuffer.bind();
  glEnable(GL_TEXTURE_CUBE_MAP);
  glActiveTexture(GL_TEXTURE0 + ntexture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, ntexture);

  //Tell OpenGL programmable pipeline where find vertex positions
  m_program->setAttributeBuffer(i_position, GL_FLOAT, 0, 3, 3 * sizeof(GLfloat));

  m_program->enableAttributeArray(i_position);
  glDrawElements(GL_TRIANGLE_STRIP, 36, GL_UNSIGNED_SHORT, NULL);
  m_program->disableAttributeArray(i_position);
  glDisable(GL_TEXTURE_CUBE_MAP);
}
