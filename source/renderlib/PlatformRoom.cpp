//
//  PlatformRoom.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include "PlatformRoom.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "RenderManager.h"
#include "GLFWTime.h"
//Get rid of this once we have a texture class working
#include "OpenGLHelper.h"

namespace renderlib{

PlatformRoom::PlatformRoom()
{
  _renderSortValue = IRenderable::DefaultSortValue;
}

PlatformRoom::~PlatformRoom()
{

  delete _shader;
  delete _cubeMesh;
  delete _roomMesh;
}

void PlatformRoom::init()
{
  //For now just hard code some stuff in here to see if everything else works!
  _shader = new Shader();
  _shader->registerShader("shaders/vertShader.glsl", ShaderType::VERTEX);
  _shader->registerShader("shaders/fragShader.glsl", ShaderType::FRAGMENT);
  _shader->compileShaders();
  _shader->linkShaders();

  _cubeMesh = new Mesh();
  //_mesh->createScreenQuad(Vector2(-1,-1), Vector2(1,1));
  _cubeMesh->createCube();
  _cubeMesh->bindAttributesToVAO(*_shader);

  _movingCubeMesh = new Mesh();
  //_mesh->createScreenQuad(Vector2(-1,-1), Vector2(1,1));
  _movingCubeMesh->createCube();
  _movingCubeMesh->bindAttributesToVAO(*_shader);

  _roomMesh = new Mesh();
  //_mesh->createScreenQuad(Vector2(-1,-1), Vector2(1,1));
  _roomMesh->createCube();
  Vector2 s(_roomDim.x/20, _roomDim.y/20);
  std::vector<Vector2>& uvs = _roomMesh->getUVVector();
  for (Vector2& uv : uvs){
    uv.x *=  s.x;
    uv.y *=  s.y;
  }
  _roomMesh->constructBuffer();

  _roomMesh->bindAttributesToVAO(*_shader);
  _mvp.identity();

  _cubeTexture.loadPNG("assets/companion_cube.png",TextureFilterMode::LINEAR, TextureClampMode::CLAMP_TO_EDGE);
  _wallTexture.loadPNG("assets/concrete.png",TextureFilterMode::LINEAR, TextureClampMode::REPEAT);
  //_renderTexture.setupFBO(1024,1024,true, GL_RGBA8, GL_RGBA,TextureDataType::TDT_UBYTE);
  //_renderTexture.setupDebugData(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
  //_renderTexture.setClearColor(Color(0.0f, 0.0f, 0.2f, 1.0f));

}

void PlatformRoom::update(float time)
{

  
  // Conversion from Euler angles (in radians) to Quaternion
  /*
  vec3 EulerAngles(time, time, 0);
  quat q = quat(EulerAngles);
  _m = glm::mat4_cast(q);
  */
  

  vec3 p = _mcPosition + _mcVelocity;

  if(p.x < -_roomDim.x/2 || p.x > _roomDim.x/2){
    _mcVelocity.x *= -1.0f;
  }
  if(p.y < -_roomDim.y/2 || p.y > _roomDim.y/2){
    _mcVelocity.y *= -1.0f;
  }
  if(p.z < -_roomDim.z/2 || p.z > _roomDim.z/2){
    _mcVelocity.z *= -1.0f;
  }

  _mcPosition = _mcPosition + _mcVelocity*GLFWTime::getDT()*vec3(4);

  _movingCubeTransform = glm::translate(_mcPosition);
  _movingCubeTransform = _movingCubeTransform * glm::scale(vec3(3, 3, 3));
  

}

void PlatformRoom::preRender()
{


}

void PlatformRoom::draw()
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  Camera& cam = RenderManager::getInstance().getMainCamera();
  mat4 view = cam.getModelview();
  mat4 proj = cam.getProjection();
  _shader->bind();

  mat4 m = glm::scale(_roomDim);
  
  _wallTexture.bindToChannel(0);
  _shader->setUniform("ModelViewProjection", proj * view * m);
  _shader->setUniform("ModelView", view * m);
  _shader->setUniform("NormalTransform", m);
  _shader->setUniform("texture0", 0);
  _roomMesh->drawBuffers();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  mat4 cm = glm::translate(vec3(0.0f,-8.5f,8.0f));
  cm = cm * glm::scale(vec3(8,2,8));
  mat4 v = view;
  _wallTexture.bindToChannel(0);
  _shader->setUniform("ModelViewProjection", proj * v * cm);
  _shader->setUniform("ModelView", v * cm);
  _shader->setUniform("NormalTransform", cm);
  _shader->setUniform("texture0", 0);
  _cubeMesh->drawBuffers();

  _cubeTexture.bindToChannel(0);
  _shader->setUniform("ModelViewProjection", proj * v *_movingCubeTransform);
  _shader->setUniform("ModelView", v * _movingCubeTransform);
  _shader->setUniform("NormalTransform", _movingCubeTransform);
  _shader->setUniform("texture0", 0);
  _movingCubeMesh->drawBuffers();
  _shader->unbind();
}

void PlatformRoom::postRender()
{


}

}// namespace renderlib
