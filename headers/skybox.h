#ifndef SKYBOX_H
#define SKYBOX_H

#include <QOpenGLFunctions_3_0>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QImage>

class Skybox : protected QOpenGLFunctions_3_0
{
public:
  Skybox(QOpenGLShaderProgram *_program);
  ~Skybox();

  void drawSkybox();

private:
  QOpenGLShaderProgram *m_program;
  QOpenGLBuffer skyboxBuffer, skyboxIndexBuffer;
  int i_position;

  GLuint ntexture;
};

#endif // SKYBOX_H
