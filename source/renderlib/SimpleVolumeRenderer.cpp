//
//  SimpleVolumeRenderer.cpp
//  Renderer
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include "SimpleVolumeRenderer.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "RenderManager.h"
#include "GLFWTime.h"
#include "ObjectIDGenerator.h"


//Get rid of this once we have a texture class working
#include "OpenGLHelper.h"

namespace renderlib{

SimpleVolumeRenderer::SimpleVolumeRenderer()
{
  _renderSortValue = IRenderable::DefaultSortValue;
  _id = ObjectIDGenerator::getInstance().getNextID();
}

SimpleVolumeRenderer::~SimpleVolumeRenderer()
{

  delete _mesh;
  delete _shader;
  delete _trackball;
}

void SimpleVolumeRenderer::resize()
{
  _screenDim = RenderManager::getInstance().getFramebufferSize();
}
  
void SimpleVolumeRenderer::init()
{

  resize();
  _trackball = new Trackball(_screenDim.x, _screenDim.y, _screenDim.y* 0.5f);
  _shader = new Shader();
  _shader->registerShader("shaders/raymarchGeom.glsl", ShaderType::GEOMETRY);
  _shader->registerShader("shaders/raymarchVert.glsl", ShaderType::VERTEX);
  //_shader->registerShader("shaders/raymarchCloudFrag.glsl", ShaderType::FRAGMENT);
  //_shader->registerShader("shaders/raymarchCTScanFrag.glsl", ShaderType::FRAGMENT);
  _shader->registerShader("shaders/raymarchCTScanFragISO.glsl", ShaderType::FRAGMENT);
  _shader->compileShaders();
  _shader->linkShaders();

  _mesh = new Mesh();
  _mesh->createPoint();
  _mesh->bindAttributesToVAO(*_shader);

  _mvp.identity();

  //_texture.createPyroclasticVolume(128, 0.025f);
  _texture.loadRaw3DData("assets/Data/512x512x512x_uint16.raw", 512, 512, 512, TextureDataType::TDT_USHORT);

}

void SimpleVolumeRenderer::update(float time)
{


	/////////
  //Trackball->Update(microseconds);

  _eyePosition = vmath::Point3(0, 0, 5 + _trackball->GetZoom());

  vmath::Vector3 up(0, 1, 0); vmath::Point3 target(0, 0, 0);
  _v = vmath::Matrix4::lookAt(_eyePosition, target, up);

  Matrix4 m(transpose(_trackball->GetRotation()), Vector3(0,0,0));
  _mv = _v * m;


  float n = 1.0f;
  float f = 100.0f;
  float fieldOfView = 0.7f;

  _p = Matrix4::perspective(fieldOfView, 1, n, f);
  _mvp = _p * _mv;

}

void SimpleVolumeRenderer::preRender()
{


}

void SimpleVolumeRenderer::draw()
{
  float t = GLFWTime::getCurrentTime();
  Camera& c = RenderManager::getInstance().getMainCamera();
  mat4 p = c.getProjection();
  mat4 mv = c.getModelview();
  vec3 eyePos =  c.getEyePosition() + _eyeOffset;

  float fov = c.getFieldOfView();
  float s = 2.0f;
  mat4 xform = glm::scale(vec3(s, s, s));
  mv = mv * xform;
  
  eyePos = vec3(glm::inverse(mv)  * vec4(eyePos, 1.0f));

  mv = mv * glm::rotate(GLFWTime::getCurrentTime()*0.5f, vec3(0, 1, 0));

  GLUtil::clear(true, true, false);
  GLUtil::setDepthTest(false);
  GLUtil::cullBackFaces();
  GLUtil::cullOn();
  GLUtil::setBlend(true);
  GLUtil::setBlendFuncToComposite();
  _shader->bind();

  _shader->setUniform("ModelviewProjection", p*mv);
  _shader->setUniform("Modelview", mv);
  _shader->setUniform("ViewMatrix", mv);
  _shader->setUniform("ProjectionMatrix", p);
  _shader->setUniform("RayStartPoints", 1);
  _shader->setUniform("RayStopPoints", 2);
  _shader->setUniform("EyePosition", eyePos);

  
  vec4 rayOrigin(glm::transpose(mv) * vec4(eyePos,1));
  _shader->setUniform("RayOrigin", vec3(rayOrigin));
  

  float focalLength = 1.0f / tan(fov / 2);
  _shader->setUniform("FocalLength", focalLength);
  _shader->setUniform("WindowSize", _screenDim.x, _screenDim.y);

  _mesh->drawBuffers();
  _shader->unbind();

}

void SimpleVolumeRenderer::postRender()
{


}
void SimpleVolumeRenderer::handleKey(KeyInfo& key)
{
  
  printf("Key.key %d\n", key.key);
  if(key.key >= '0' && key.key <= '9' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    //char idx = key.key - '0';
  }
  if(key.key == 'R' && key.action == KeyInfo::KeyAction::RELEASE)
  {
  }
  if(key.key == 'G' && key.action == KeyInfo::KeyAction::RELEASE)
  {
  }
  if(key.key == 'B' && key.action == KeyInfo::KeyAction::RELEASE)
  {
  }
  if(key.key == 'D' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    // _showDebug = !_showDebug;
  }
  if(key.key == ' ' && key.action == KeyInfo::KeyAction::PRESS)
  {
  }
  //264 == down arrow
  if (key.key == 264 && (key.action == KeyInfo::KeyAction::REPEAT
    || key.action == KeyInfo::KeyAction::PRESS))
  {
    _eyeOffset.z += 0.3f;

  }
  //264 == up arrow
  if (key.key == 265 && (key.action == KeyInfo::KeyAction::REPEAT
    || key.action == KeyInfo::KeyAction::PRESS))
  {
    _eyeOffset.z -= 0.3;
  }

  //262 == right
  if (key.key == 262 && (key.action == KeyInfo::KeyAction::REPEAT
    || key.action == KeyInfo::KeyAction::PRESS))
  {
    _rotAngle += 6.25f;

  }
  //253 == left
  if (key.key == 263 && (key.action == KeyInfo::KeyAction::REPEAT
    || key.action == KeyInfo::KeyAction::PRESS))
  {
    _rotAngle -= 6.25f;

  }
  
  
  if(key.action == KeyInfo::KeyAction::PRESS)
  {
    switch (key.key)
    {
      case ',':
      case '<':
        break;
      case '.':
      case '>':
        break;
      case '/':
      case '?':
        break;
    }
  }
}
  
void SimpleVolumeRenderer::handlePointer(std::vector<PointerInfo>& pointers)
{
  PointerInfo& p = pointers[0];
  if (p.dragging){
    _trackball->MouseDown(p.pos.x, p.pos.y);
    _trackball->MouseMove(p.pos.x, p.pos.y);
    _trackball->MouseUp(p.pos.x, p.pos.y);
  }
}

}// namespace renderlib
