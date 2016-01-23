//
//  Painter.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 7/19/14.
//
//

#include "Painter.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "GLFWTime.h"
#include "ObjectIDGenerator.h"
#include "RenderManager.h"


namespace renderlib{

using namespace vmath;



Painter::Painter()
{
  _renderSortValue = IRenderable::DefaultSortValue;

  _showDebug = false;
  _rotAngle = -PI/2.0f + 0.001f;
  _decayRate = 0.99f;
  _minPressure = 0.006f;
  _smoothingEnabled = true;
  _currentColor = Color::blue() * 0.8f;

  _colors[4].set(1.0f,0.0f,0.0f);// Red
  _colors[1].set(1.0f,0.5f,0.0f);// Orange
  _colors[2].set(1.0f,1.0f,0.0f);// Yellow
  _colors[3].set(0.0f,1.0f,0.0f);// Green
  _colors[0].set(0.0f,1.0f,1.0f);// Cyan
  _colors[5].set(0.0f,0.5f,1.0f);// Blueish
  _colors[6].set(0.0f,0.0f,1.0f);// Blue
  _colors[7].set(0.5f,0.0f,1.0f);// Purpleish
  _colors[8].set(1.0f,0.0f,1.0f);// Purple
  _colors[9].set(1.0f,0.0f,0.5f);// Violet
  _currentColor = _colors[0];

  _projection = Matrix4::perspective(0.7f, 1.0f, 0.01f, 100.0f);
}

Painter::~Painter()
{
  delete _rt0;
  delete _rt1;
  delete _shader;
  delete _simulationShader;
  delete _screenMesh;
  delete _waterMesh;

}

void Painter::init()
{

  _sensorAspectRatio = 0.5f;//Sensor::getInstance().getSensorAspect();
  
  float s = 800.0f;
  _fboSize = RenderManager::getInstance().getFramebufferSize() * 0.5f;
  //_fboSize.set(_sensorAspectRatio*s,s);


  _id = ObjectIDGenerator::getInstance().getNextID();
  //For now just hard code some stuff in here to see if everything else works!
  setupDisplayShaders();
  setupSimulationShaders();
  GetGLError();

  _screenMesh = new Mesh();
  _screenMesh->createScreenQuadIBO(Vector2(-1,-1), Vector2(1,1));
  _screenMesh->bindAttributesToVAO(*_simulationShader);

  _waterMesh = new Mesh();
  _waterMesh->createTriStripGridMeshData((int)_fboSize.x, (int)_fboSize.y,
                                         false, _sensorAspectRatio);
  _waterMesh->bindAttributesToVAO(*_shader);


  _rt0 = new RenderTexture();
  _rt1 = new RenderTexture();
  Color clear(1.0f, 1.0f, 1.0f, 0.0f);
  _rt0->setClearColor(clear);
  _rt1->setClearColor(clear);
  _rt0->setupFBO((int)_fboSize.x,(int)_fboSize.y,true, GL_RGBA16F,GL_RGBA,TextureDataType::TDT_FLOAT, 4);
  _rt1->setupFBO((int)_fboSize.x,(int)_fboSize.y,true,GL_RGBA16F,GL_RGBA, TextureDataType::TDT_FLOAT, 4);
  _rt0->matchFBOSizeToViewport();
  _rt1->matchFBOSizeToViewport();
  _rt0->setupDebugData(Vector2(-1.0f, -1.0f), Vector2(-0.5, -0.0));
  _rt1->setupDebugData(Vector2(-0.5, -1.0f), Vector2(-0.0, -0.0));
  _rt0->clear();
  _rt1->clear();



  _canvasTexture.loadFile("assets/canvas_normal2.png", TextureFilterMode::LINEAR, TextureClampMode::REPEAT);
  _canvasTexture.setAnisoPercent(1.0f);
  _refractionMap = new RenderTexture();
  _refractionMap->setupFBO((int)_fboSize.x,(int)_fboSize.y,true, GL_RGBA8,GL_RGBA,TextureDataType::TDT_UBYTE);
  _refractionMap->setupDebugData(Vector2(-1.0f, -1.0f), Vector2(1.0, 1.0));
  _refractionMap->matchFBOSizeToViewport();


}

void Painter::update(float time)
{
  _waterModel = Matrix4::rotationX(-PI / 2.0f);

  _eyePosition.set(0.0f, 0.0f, 1.2f);
  Matrix4 rot = Matrix4::rotation(_rotAngle, Vector3::xAxis());
  _eyePosition = rot * _eyePosition;
  Point3 eye;
  eye.setX(_eyePosition.x);
  eye.setY(_eyePosition.y);
  eye.setZ(_eyePosition.z);

  Vector3 up(0, 1, 0); Point3 target(0);
  Matrix4 ViewMatrix = Matrix4::lookAt(eye, target, up);
  _camera = ViewMatrix;

  Matrix4 WaterModelviewMatrix = ViewMatrix * _waterModel;

  Vector2 vp = RenderManager::getInstance().getFramebufferSize();
  float n = 0.1f;
  float f = 100.0f;
  float fieldOfView = 0.9f;

  Matrix4 ProjectionMatrix = Matrix4::perspective(fieldOfView, vp.x/vp.y, n, f);
  _waterModelView = WaterModelviewMatrix;
  _waterMVP =  ProjectionMatrix * WaterModelviewMatrix;


}

void Painter::preRender()
{


}

void Painter::simulate()
{
  Vector2 fb = RenderManager::getInstance().getFramebufferSize();

  Vector2 vp = _rt0->getDimensions();
  //Convert the mouse position into the planes coords in x/z
  Vector2 mousePos(
                   ((_pointer.pos.x/fb.x)) * fb.x,
                   ((_pointer.pos.y/fb.y)) * fb.y
                   );
//  std::cout << _pointer.pos.x << " " << fb.x << " " << vp.x << " " << mousePos.x << std::endl;

//  glEnable(GL_BLEND);
//  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//  glBlendFunc(GL_ONE, GL_ZERO);
  GLUtil::setDepthTest(false);
  GLUtil::setDepthWrite(false);
  _simulationShader->bind();
  _rt0->bindFBO();
  _rt1->bindTargetToChannel(0,FBOProxy::ATTACHMENT::COLOR_ATTACHMENT0);
  _rt1->bindTargetToChannel(3,FBOProxy::ATTACHMENT::COLOR_ATTACHMENT3);
  _simulationShader->setUniform("uTexture", 0);
  
 
  _simulationShader->setUniform("uPreviousSensorInput", 3);
  _simulationShader->setUniform("uInkColor",_currentColor);
  _simulationShader->setUniform("uMouseLocation", mousePos);
  _simulationShader->setUniform("uMouseRadius", 64.0f);
  //_simulationShader->setUniform("uMouseRadius", _pointer.width * 10.0f);
  _simulationShader->setUniform("uMouseDown", (float)(_pointer.down?1.0f:0.0f));
  _simulationShader->setUniform("uMousePressure", _pointer.pressure * 0.1f);
  _simulationShader->setUniform("uViewportSize", vp);
  _simulationShader->setUniform("uDecayRate", _decayRate);
  _simulationShader->setUniform("uDT", GLFWTime::getDT());
  _simulationShader->setUniform("uMinPressure",_minPressure);
  _simulationShader->setUniform("uSmoothing",(float)(_smoothingEnabled?1.0:0.0));
  _screenMesh->drawBuffers();
  _simulationShader->unbind();
  _rt0->unbindFBO();




}


void Painter::draw()
{
  simulate();

  GLUtil::cullBackFaces();

  _shader->bind();
  _rt1->bindTargetToChannel(0, FBOProxy::ATTACHMENT::COLOR_ATTACHMENT0);
  _refractionMap->bindTargetToChannel(1, FBOProxy::ATTACHMENT::COLOR_ATTACHMENT0);
  _canvasTexture.bindToChannel(3);
  _shader->setUniform("uPaintTexture", 0);
  _shader->setUniform("uFBOSize", _fboSize);
  _shader->setUniform("uHeightScale", 1.0f);
  _shader->setUniform("uModelViewTransform", _waterModelView);
  _shader->setUniform("uViewTransform", _camera);
  _shader->setUniform("uModelTransform", _waterModel);
  _shader->setUniform("uMVP", _waterMVP);
  _shader->setUniform("uWorldSpaceCameraPos",_eyePosition);
  _shader->setUniform("uRefraction", 1);
  _shader->setUniform("uCubeReflect", 2);
  _shader->setUniform("uNormalMap", 3);
  _shader->setUniform("uDrawWhite", 0.0f);
  GLUtil::setDepthTest(true);
  _waterMesh->drawBuffers();
  _shader->unbind();

  debugDraw();

  swapRenderTargets();



}

void Painter::debugDraw()
{
  if(!_showDebug) return;

  GLUtil::setDepthTest(false);
  _rt0->debugDraw();
  _rt1->debugDraw();
  GLUtil::setDepthTest(true);
}


void Painter::resize()
{
  _rt0->matchFBOSizeToViewport();
  _rt1->matchFBOSizeToViewport();
  _rt0->clear();
  _rt1->clear();
}
void Painter::postRender()
{


}

void Painter::swapRenderTargets()
{
  RenderTexture* tmp = _rt1;
  _rt1 = _rt0;
  _rt0 = tmp;

}

void Painter::handleKey(KeyInfo& key)
{

  if(key.key >= '0' && key.key <= '9' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    char idx = key.key - '0';
    _currentColor = _colors[(int)idx];
  }
  if(key.key == 'R' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    _currentColor.set(0.8f,0.0f,0.0f,1.0f);
  }
  if(key.key == 'G' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    _currentColor.set(0.0f,0.8f,0.0f,1.0f);
  }
  if(key.key == 'B' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    _currentColor.set(0.0f,0.0f,0.8f,1.0f);
  }
  if(key.key == 'D' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    _showDebug = !_showDebug;
  }
  if(key.key == ' ' && key.action == KeyInfo::KeyAction::PRESS)
  {
    _rt0->clear();
    _rt1->clear();
  }
  //264 == down arrow
  if(key.key == 264 && (key.action == KeyInfo::KeyAction::REPEAT
                        || key.action == KeyInfo::KeyAction::PRESS))
  {
    _rotAngle += 0.12f;

  }
  //264 == up arrow
  if(key.key == 265 && (key.action == KeyInfo::KeyAction::REPEAT
                        || key.action == KeyInfo::KeyAction::PRESS))
  {
    _rotAngle -= 0.12f;
  }
  if(key.key == 'S' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    _smoothingEnabled = !_smoothingEnabled;
  }


//  std::cout << key.key << std::endl;

  if(_rotAngle < -PI/2) _rotAngle = -PI/2 + 0.001f;
  if(_rotAngle > 0.0f) _rotAngle = 0.0f;


  if(key.action == KeyInfo::KeyAction::PRESS)
  {
    switch (key.key)
    {
      case ',':
      case '<':
        //Sensor::getInstance().decrementLerpMethod();
        break;
      case '.':
      case '>':
        //Sensor::getInstance().incrementLerpMethod();
        break;
      case '/':
      case '?':
        //Sensor::getInstance().toggleLerpMethod();
        break;
    }
  }
}

void Painter::handlePointer(std::vector<PointerInfo>& pointers)
{
  _pointer = pointers[0];
//  std::cout << "Pointer event " << _pointer.pos.x << "," << _pointer.pos.y
//              << " " << _pointer.down << " " << _pointer.pressure << std::endl;
}

void Painter::setupDisplayShaders()
{
  _shader = new Shader();
  _shader->registerShader("shaders/paintHeightDraw.vert",ShaderType::VERTEX);
  _shader->registerShader("shaders/paintHeightDraw.frag", ShaderType::FRAGMENT);
  _shader->setAttributeLocations(Mesh::getShaderAttributeLocations());
  _shader->compileShaders();
  _shader->linkShaders();

}

void Painter::setupSimulationShaders()
{
  _simulationShader = new Shader();
  _simulationShader->registerShader("shaders/painterSim.vert", ShaderType::VERTEX);
  _simulationShader->registerShader("shaders/painterSim.frag", ShaderType::FRAGMENT);
  _simulationShader->setAttributeLocations(Mesh::getShaderAttributeLocations());
  _simulationShader->setColorLocations(
                      {{"color",0}, {"color1",1}, {"color2", 2}, {"color3",3}});
  _simulationShader->compileShaders();
  _simulationShader->linkShaders();

  _copyTextureShader = new Shader();
  _copyTextureShader->registerShader("shaders/copyFBO.vert", ShaderType::VERTEX);
  _copyTextureShader->registerShader("shaders/copyFBO.frag", ShaderType::FRAGMENT);
  _copyTextureShader->setAttributeLocations(Mesh::getShaderAttributeLocations());
  _copyTextureShader->compileShaders();
  _copyTextureShader->linkShaders();

}
}// namespace renderlib
