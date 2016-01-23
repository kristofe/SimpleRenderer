//
//  WaveSim.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include "WaveSim.h"
#include "mesh.h"
#include "Texture.h"
#include "GLFWTime.h"
#include "ImageUtils.h"
#include "ObjectIDGenerator.h"
#include "RenderManager.h"


namespace renderlib{

using namespace vmath;

WaveSim::WaveSim()
{
  _renderSortValue = IRenderable::DefaultSortValue;

  _rotAngle = -PI/4.0f;

  _projection = Matrix4::perspective(0.7f, 1.0f, 0.01f, 100.0f);
  _damping = HIGH_DAMPING;
  _lastSimTime = GLFWTime::getTime();
}

WaveSim::~WaveSim()
{
  delete _rt0;
  delete _rt1;
  delete _shader;
  delete _simulationShader;
  delete _tankShader;
  delete _screenMesh;
  delete _waterMesh;
  delete _tankMesh;
}

void WaveSim::init()
{

  _sensorAspectRatio = 3.0/2.0;
  
  float s = 256.0f;
  _fboSize.set(_sensorAspectRatio*s,s);

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

  _tankMesh = new Mesh();
  //FIXME: Reverse the normals!
  _tankMesh->createOpenTopCube(Vector3(_sensorAspectRatio * 1.0f, 0.2f, 1.0f), Vector3(_sensorAspectRatio * 2.0f, 0.4f, 2.0f));
  _tankMesh->bindAttributesToVAO(*_tankShader);

  _rt0 = new RenderTexture();
  _rt1 = new RenderTexture();
  _rt0->setupFBO((int)_fboSize.x,(int)_fboSize.y,true, GL_RGBA32F, GL_RGBA, TextureDataType::TDT_FLOAT);
  _rt1->setupFBO((int)_fboSize.x,(int)_fboSize.y,true, GL_RGBA32F, GL_RGBA, TextureDataType::TDT_FLOAT);
  _rt0->setupDebugData(Vector2(-1.0f, -1.0f), Vector2(-0.5, -0.5));
  _rt1->setupDebugData(Vector2(-0.5, -1.0f), Vector2(-0.0, -0.5));

  _cubeMap.loadCubemapPNG("assets/pond");

  _tankTexture.loadFile("assets/tiles.png", TextureFilterMode::LINEAR, TextureClampMode::REPEAT);
  _tankTexture.setAnisoPercent(1.0f);
  _refractionMap = new RenderTexture();
  _refractionMap->setupFBO((int)_fboSize.x,(int)_fboSize.y,true, GL_RGBA8, GL_RGBA, TextureDataType::TDT_UBYTE);
  _refractionMap->setupDebugData(Vector2(-1.0f, -1.0f), Vector2(1.0, 1.0));
  _refractionMap->matchFBOSizeToViewport();

}

void WaveSim::update(float time)
{
  _waterModel = Matrix4::rotationX(-PI / 2.0f);// * Matrix4::scale(Vector3(_sensorProportion, 1.0f,1.0f));

  _eyePosition.set(0, 0, 1.4f);
//  Matrix4 rot = Matrix4::rotation(sin(time * 0.5f) * 0.5 + 0.5 + -PI/4.0f, Vector3::xAxis());
  Matrix4 rot = Matrix4::rotation(_rotAngle, Vector3::xAxis());
  _eyePosition = rot * _eyePosition;
  Point3 eye;
  eye.setX(_eyePosition.x);
  eye.setY(_eyePosition.y);
  eye.setZ(_eyePosition.z);

  Vector3 up(0, 1, 0); Point3 target(0);
  Matrix4 ViewMatrix = Matrix4::lookAt(eye, target, up);
  _camera = ViewMatrix;

  _tankTransform =  Matrix4::identity();
  Matrix4 TankModelviewMatrix = ViewMatrix * _tankTransform;
  Matrix4 WaterModelviewMatrix = ViewMatrix * _waterModel;

  Vector2 vp = RenderManager::getInstance().getFramebufferSize();
  float n = 1.0f;
  float f = 100.0f;
  float fieldOfView = 0.9f;

  Matrix4 ProjectionMatrix = Matrix4::perspective(fieldOfView, vp.x/vp.y, n, f);
  _mvp = ProjectionMatrix * TankModelviewMatrix;
  _waterModelView = WaterModelviewMatrix;
  _waterMVP =  ProjectionMatrix * WaterModelviewMatrix;


}

void WaveSim::preRender()
{


}

void WaveSim::simulate()
{
	float time = GLFWTime::getTime();
	const float simDT = 0.016f;
	if (time - _lastSimTime < simDT)
	{
		return;
	}
	_lastSimTime = time;
  Vector2 vp = RenderManager::getInstance().getFramebufferSize();
  //Convert the mouse position into the planes coords in x/z
  Vector2 mousePos(
                   ((_pointer.pos.x/vp.x)) * _fboSize.x,
                   ((_pointer.pos.y/vp.y)) * _fboSize.y
                   );
//  std::cout << _pointer.pos.x << " " << vp.x << " " << _fboSize.x << " " << mousePos.x << std::endl;

  _simulationShader->bind();
  _rt0->bindFBO();
  _rt1->bindAllTargetsStartingAt(0);
  _simulationShader->setUniform("uTexture", 0);
  /*
 if(_sensorMapLowRez != nullptr)
  {
    _sensorMapLowRez->bindToChannel(1);
    _sensorMapHighRez->bindToChannel(2);
    _simulationShader->setUniform("uSensorData", 1);
    _simulationShader->setUniform("uSensorDataHighRez", 2);
  }
  */
  _simulationShader->setUniform("uFBOSize", _fboSize);
  _simulationShader->setUniform("uMouseLocation", mousePos);
  _simulationShader->setUniform("uMouseRadius", 12.0f);
  _simulationShader->setUniform("uDamping", _damping);
  _simulationShader->setUniform("uMouseDown", (float)(_pointer.down?1.0f:0.0f));
  _simulationShader->setUniform("uMousePressure", _pointer.pressure);
  GLUtil::setDepthTest(false);
  GLUtil::setDepthWrite(false);
  _screenMesh->drawBuffers();
  _simulationShader->unbind();
  _rt0->unbindFBO();
  GLUtil::setDepthTest(true);
  GLUtil::setDepthWrite(true);

  swapRenderTargets();
}


void WaveSim::draw()
{
  simulate();

  // draw the cube
  GLUtil::setDepthTest(true);
  GLUtil::setDepthWrite(true);
  GLUtil::cullFrontFaces();
  _refractionMap->matchFBOSizeToViewport();
  _refractionMap->bindFBO();
  GLUtil::clear(true, true, false);
  _tankShader->bind();
  _tankTexture.bindToChannel(0);
  _tankShader->setUniform("texture0", 0);
  _tankShader->setUniform("mtxModel", _tankTransform);
  _tankShader->setUniform("mtxNormal", _tankTransform);
  _tankShader->setUniform("mtxCameraInverse", _camera);
  _tankShader->setUniform("mtxMVP", _mvp);
  _tankMesh->drawBuffers();
  _tankShader->unbind();
  _refractionMap->unbindFBO();
  GLUtil::cullBackFaces();


  GLUtil::setDepthTest(false);
  GLUtil::setDepthWrite(false);
  _refractionMap->drawFullscreen();

  GLUtil::setDepthTest(false);
  GLUtil::setDepthWrite(false);
  _shader->bind();
  _rt0->bindAllTargetsStartingAt(0);
  _refractionMap->bindAllTargetsStartingAt(1);
  _cubeMap.bindToChannel(2);
  _shader->setUniform("uWaveTexture", 0);
  _shader->setUniform("uFBOSize", _fboSize);
  _shader->setUniform("uWaveScale", 1.0f);
  _shader->setUniform("uModelViewTransform", _waterModelView);
  _shader->setUniform("uViewTransform", _camera);
  _shader->setUniform("uModelTransform", _waterModel);
  _shader->setUniform("uMVP", _waterMVP);
  _shader->setUniform("uWorldSpaceCameraPos",_eyePosition);
  _shader->setUniform("uRefraction", 1);
  _shader->setUniform("uCubeReflect", 2);
   _waterMesh->drawBuffers();
  _shader->unbind();

  //swapRenderTargets();

  //debugDraw();

}

void WaveSim::debugDraw()
{
  _rt0->debugDraw();
  _rt1->debugDraw();
}


void WaveSim::resize()
{
  _refractionMap->matchFBOSizeToViewport();
}
void WaveSim::postRender()
{


}

void WaveSim::swapRenderTargets()
{
  RenderTexture* tmp = _rt1;
  _rt1 = _rt0;
  _rt0 = tmp;

}

void WaveSim::handleKey(KeyInfo& key)
{

  if(key.key == ' ' && key.action == KeyInfo::KeyAction::PRESS)
  {
    _damping = 0.0f;
  }
  if(key.key == ' ' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    _damping = LOW_DAMPING;
  }
  //264 == down arrow
  if(key.key == 264 && (key.action == KeyInfo::KeyAction::REPEAT
                        || key.action == KeyInfo::KeyAction::PRESS))
  {
    _rotAngle += 0.03f;

  }
  //264 == up arrow
  if(key.key == 265 && (key.action == KeyInfo::KeyAction::REPEAT
                        || key.action == KeyInfo::KeyAction::PRESS))
  {
    _rotAngle -= 0.03f;
  }

  if((key.key == 'd' || key.key == 'D' )&& key.action == KeyInfo::KeyAction::PRESS)
  {
    _damping = _damping == LOW_DAMPING?HIGH_DAMPING:LOW_DAMPING;
    std::cout << "_damping = " << _damping << std::endl;
  }


  if(_rotAngle < -PI/2) _rotAngle = -PI/2 + 0.002f;
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

void WaveSim::handlePointer(std::vector<PointerInfo>& pointers)
{
  _pointer = pointers[0];
//  std::cout << "Pointer event " << _pointer.pos.x << "," << _pointer.pos.y
//              << " " << _pointer.down << " " << _pointer.pressure << std::endl;
}

void WaveSim::setupDisplayShaders()
{
  _shader = new Shader();
  _shader->registerShader("shaders/waveSimDraw.vert",ShaderType::VERTEX);
  _shader->registerShader("shaders/waveSimDraw.frag", ShaderType::FRAGMENT);
  _shader->setAttributeLocations(Mesh::getShaderAttributeLocations());
  _shader->compileShaders();
  _shader->linkShaders();

  _tankShader = new Shader();
  _tankShader->registerShader("shaders/texturedMesh.vert",ShaderType::VERTEX);
  _tankShader->registerShader("shaders/texturedMesh.frag", ShaderType::FRAGMENT);
  _tankShader->setAttributeLocations(Mesh::getShaderAttributeLocations());
  _tankShader->compileShaders();
  _tankShader->linkShaders();

}

void WaveSim::setupSimulationShaders()
{
  _simulationShader = new Shader();
  _simulationShader->registerShader("shaders/waveSim.vert", ShaderType::VERTEX);
  _simulationShader->registerShader("shaders/waveSim.frag", ShaderType::FRAGMENT);
  _simulationShader->setAttributeLocations(Mesh::getShaderAttributeLocations());
  _simulationShader->compileShaders();
  _simulationShader->linkShaders();

}
}// namespace renderlib
