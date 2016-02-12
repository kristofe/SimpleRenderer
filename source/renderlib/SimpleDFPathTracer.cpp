//
//  SimpleDFPathTracer.cpp
//  Renderer
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include <memory>
#include "SimpleDFPathTracer.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "RenderManager.h"
#include "GLFWTime.h"
#include "ObjectIDGenerator.h"
#include "TriangleMesh.h"
#include "UniformGrid.h"
#include "UniformHGrid.h"

//Get rid of this once we have a texture class working
#include "OpenGLHelper.h"

namespace renderlib{

SimpleDFPathTracer::SimpleDFPathTracer()
{
  _renderSortValue = IRenderable::DefaultSortValue;
  _id = ObjectIDGenerator::getInstance().getNextID();
  _useScreenResolution = false;
  _saveFrame = false;
}

SimpleDFPathTracer::~SimpleDFPathTracer()
{

}

void SimpleDFPathTracer::resize()
{
  vmath::Vector2 sd =  RenderManager::getInstance().getFramebufferSize();
  _screenDim.x = sd.x;
  _screenDim.y = sd.y;
  _useScreenResolution ? _currentResolution = _screenDim : _currentResolution = _imageDim;
  _renderTexture.setFBOSize(_currentResolution.x, _currentResolution.y);

}
  
void SimpleDFPathTracer::init()
{
  resize();
  _mvp.identity();
  //For now just hard code some stuff in here to see if everything else works!
  _shader = new Shader();
  _shader->registerShader("shaders/passThroughVS.glsl", ShaderType::VERTEX);
  _shader->registerShader("shaders/pathTracingSphereTracer.glsl", ShaderType::FRAGMENT);
  _shader->compileShaders();
  _shader->linkShaders();

  _mesh = new Mesh();
  _mesh->createScreenQuad(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
  _mesh->bindAttributesToVAO(*_shader);
  

  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!

  const int DFRESOLUTION = 32;
  _imageDim = glm::vec2(128, 128);
  _currentResolution = _imageDim;
  _gridResolution = DFRESOLUTION;
  char outputName[256];
  char inputName[256];
  const char* modelname ="LionessSmooth";
  sprintf(inputName, "assets/models/%s.obj", modelname);
  sprintf(outputName, "assets/%s%d.bin", modelname, DFRESOLUTION);

  
  Mesh normalMesh;
  //normalMesh.createSphereMeshData(8, 8);
  //normalMesh.createCube(Vector3(0.5f), Vector3(1.0f));
  //normalMesh.createTriangle();
  _model.loadModelFromFile(inputName, true, true);
  std::vector<Material> materials;
  _model.collapseMeshes(normalMesh, materials);

  normalMesh.fitIntoUnitCube();
  normalMesh.movePivotToBottomMiddle();
  
  TriangleMesh triMesh;
  std::shared_ptr<UniformHGrid> grid = std::make_shared<UniformHGrid>(DFRESOLUTION, glm::vec3(0));
  normalMesh.convertToTriangleMesh(triMesh, grid);
  
  //_texture.createDistanceFieldFromMesh(DFRESOLUTION, triMesh, true, outputName);
  _texture.loadDistanceFieldFromDisk(outputName);

  
  _texture.setupDebugData(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));

  _renderTexture.setupFBO(_imageDim.x, _imageDim.y, true, GL_RGBA8, GL_RGBA, TextureDataType::TDT_UBYTE, 1);
  _renderTexture.setupFullscreenData();

}

void SimpleDFPathTracer::update(float time)
{


  // Conversion from Euler angles (in radians) to Quaternion
  vec3 EulerAngles(0,0.5*time,  0);
  quat q = quat(EulerAngles);
  _m = glm::mat4_cast(q);
  //glm::mat4 xm = glm::translate(glm::vec3(0.125, 0, 0.125));
  //_m = _m * xm;

  _mInverse = glm::inverse(_m);
  
  Matrix4 ModelviewMatrix = Matrix4::identity();

  float n = 1.0f;
  float f = 100.0f;
  float fieldOfView = 0.7f;

  Matrix4 ProjectionMatrix = Matrix4::perspective(fieldOfView, 1, n, f);
  _mvp = ProjectionMatrix * ModelviewMatrix;

}

void SimpleDFPathTracer::preRender()
{


}

void SimpleDFPathTracer::draw()
{

  _renderTexture.bindFBO();
  glDisable(GL_DEPTH_TEST);
  _shader->bind();
  _texture.bindToChannel(0);
  _shader->setUniform("iGlobalTime", GLFWTime::getCurrentTime());
  _shader->setUniform("iResolution", Vector2(_currentResolution.x,_currentResolution.y));
  _shader->setUniform("iMouse", Vector2(_mousePos.x,_mousePos.y));
  _shader->setUniform("Density", 0);
  _shader->setUniform("uModelMatrix", _m);
  _shader->setUniform("uNormalMatrix", glm::inverseTranspose( _m));
  _shader->setUniform("uModelInverseMatrix", _mInverse);
  _shader->setUniform("uGridResolution", _gridResolution);
  
  _mesh->drawBuffers();
  _shader->unbind();

// FIXME:  Create signed dist field.
  //FIXME: Create test cases for sphere
  if (_saveFrame)
  {
	  saveScreenShotTGA("screencap.tga", _imageDim.x, _imageDim.y);
	  _saveFrame = false;
  }

  _renderTexture.unbindFBO();

  _renderTexture.drawFullscreen();
  //_texture.debugDraw();
  
  glEnable(GL_DEPTH_TEST);

}

void SimpleDFPathTracer::postRender()
{


}
void SimpleDFPathTracer::handleKey(KeyInfo& key)
{
  
  if(key.key >= '0' && key.key <= '9' && key.action == KeyInfo::KeyAction::RELEASE)
  {
    //char idx = key.key - '0';
  }
  if(key.key == 'S' && key.action == KeyInfo::KeyAction::RELEASE)
  {
	  _saveFrame = true;
  }
  if(key.key == 'I' && key.action == KeyInfo::KeyAction::RELEASE)
  {
	  _useScreenResolution = !_useScreenResolution;
	  resize();
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
  
void SimpleDFPathTracer::handlePointer(std::vector<PointerInfo>& pointers)
{
  _mousePos = pointers[0].pos;
}

}// namespace renderlib
