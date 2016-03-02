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
  _elevationIDX = 0;
  _azimuthIDX = 0;
  _lightingIDX = 0;
  _renderTexture0 = new RenderTexture();
  _renderTexture1 = new RenderTexture();
  _downsampledTexture = new RenderTexture();
  
}

SimpleDFPathTracer::~SimpleDFPathTracer()
{
  delete _renderTexture0;
  delete _renderTexture1;
  delete _downsampledTexture;
  delete _shader;
  delete _filterShader;
}
  
void SimpleDFPathTracer::resetFBOs()
{
  _renderTexture0->clear();
  _renderTexture1->clear();
  
}

void SimpleDFPathTracer::resize()
{
  vmath::Vector2 sd =  RenderManager::getInstance().getFramebufferSize();
  _screenDim.x = sd.x;
  _screenDim.y = sd.y;
  _useScreenResolution ? _currentResolution = _screenDim : _currentResolution = _imageDim;
  _renderTexture0->setFBOSize(_currentResolution.x, _currentResolution.y);
  _renderTexture1->setFBOSize(_currentResolution.x, _currentResolution.y);
  resetFBOs();

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
  
  _fullscreenMesh = new Mesh();
  _fullscreenMesh->createScreenQuad(Vector2(-1.0f, -1.f), Vector2(1.0f, 1.0f));
  _filterShader = new Shader();
  _filterShader->registerShader("shaders/lanczosFilter.vert", ShaderType::VERTEX);
  _filterShader->registerShader("shaders/lanczosFilter.frag", ShaderType::FRAGMENT);
  //_filterShader->registerShader("shaders/bicubicFilter.vert", ShaderType::VERTEX);
  //_filterShader->registerShader("shaders/bicubicFilter.frag", ShaderType::FRAGMENT);
  _filterShader->compileShaders();
  _filterShader->linkShaders();
  _fullscreenMesh->bindAttributesToVAO(*_filterShader);

  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!

  const int DFRESOLUTION = 256;
  _imageDim = glm::vec2(96, 96);
  _currentResolution = _imageDim;
  _gridResolution = DFRESOLUTION;
  char outputName[256];
  char inputName[256];
  const char* modelname ="Humvee200k";
  sprintf(inputName, "assets/models/%s.obj", modelname);
  sprintf(outputName, "assets/%s%d.bin", modelname, DFRESOLUTION);

  
  Mesh normalMesh;
  //normalMesh.createSphereMeshData(8, 8);
  //normalMesh.createCube(Vector3(0.5f), Vector3(1.0f));
  //normalMesh.createTriangle();
  
  
  _model.loadModelFromFile(inputName, true, true);
  std::vector<Material> materials;
  _model.collapseMeshes(normalMesh, materials);

  normalMesh.fitIntoUnitCube(_trans, _min, _max);
  //normalMesh.movePivotToBottomMiddle();
  
  TriangleMesh triMesh;
  std::shared_ptr<UniformHGrid> grid = std::make_shared<UniformHGrid>(DFRESOLUTION, glm::vec3(0));
  normalMesh.convertToTriangleMesh(triMesh, grid);
  
  //_texture.createDistanceFieldFromMesh(DFRESOLUTION, triMesh, true, outputName);
  _texture.loadDistanceFieldFromDisk(outputName);

  printf("trans: %3.6f %3.6f %3.6f\n", _trans.x, _trans.y, _trans.z);
  printf("min: %3.6f %3.6f %3.6f\n", _min.x, _min.y, _min.z);
  printf("max: %3.6f %3.6f %3.6f\n", _max.x, _max.y, _max.z);
  _targetHeight = _max.y * 0.5f;
  
  _texture.setupDebugData(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));

  _renderTexture0->setupFBO(_imageDim.x, _imageDim.y, true, GL_RGBA32F, GL_RGBA, TextureDataType::TDT_FLOAT, 1);
  _renderTexture0->setupFullscreenData();

  _renderTexture1->setupFBO(_imageDim.x, _imageDim.y, true, GL_RGBA32F, GL_RGBA, TextureDataType::TDT_FLOAT, 1);
  _renderTexture1->setupFullscreenData();
  _downsampledTexture->setupFBO(_imageDim.x, _imageDim.y, true, GL_RGBA32F, GL_RGBA, TextureDataType::TDT_FLOAT, 1);
  _downsampledTexture->setupDebugData(Vector2(-1.0, -1.0), Vector2(1.0, 1.0));
}

void SimpleDFPathTracer::update(float time)
{


  // Conversion from Euler angles (in radians) to Quaternion
  float azimuth = _azimuths[_azimuthIDX];
  float azimuthRadians = glm::radians(azimuth);
  vec3 EulerAngles(0,azimuthRadians,  0);
  quat q = quat(EulerAngles);
  _m = glm::mat4_cast(q);
  _mInverse = glm::inverse(_m);

  float elevation = -_elevations[_elevationIDX];
  float elevationRadians = glm::radians(elevation);
  vec3 EulerAnglesElevation(elevationRadians, 0,  0);
  quat qElevation = quat(EulerAnglesElevation);
  glm::mat4 elevationMatrix = glm::mat4_cast(qElevation);

  glm::vec3 camPos(0.0f, 0.0f, 2.0f);
  _cameraPosition = mat3(elevationMatrix) * camPos;
  _cameraMatrix = elevationMatrix;
  


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

  _renderTexture0->bindFBO();
  glDisable(GL_DEPTH_TEST);
  _shader->bind();
  _texture.bindToChannel(0);
  _renderTexture1->bindTargetToChannel(1);
  _shader->setUniform("uPreviousFrameTexture",1);
  _shader->setUniform("iGlobalTime", GLFWTime::getCurrentTime());
  _shader->setUniform("iResolution", Vector2(_currentResolution.x,_currentResolution.y));
  _shader->setUniform("iMouse", Vector2(_mousePos.x,_mousePos.y));
  _shader->setUniform("Density", 0);
  _shader->setUniform("uModelMatrix", _m);
  _shader->setUniform("uNormalMatrix", glm::inverseTranspose( _m));
  _shader->setUniform("uModelInverseMatrix", _mInverse);
  _shader->setUniform("uGridResolution", _gridResolution);
  _shader->setUniform("uCameraPosition", _cameraPosition);
  _shader->setUniform("uObjectOffset", _trans);
  _shader->setUniform("uCameraMatrix", _cameraMatrix);
  _shader->setUniform("lightSwitches", _lightSwitching[_lightingIDX],6);
  _shader->setUniform("uTargetHeight", _targetHeight);
  
  _mesh->drawBuffers();
  _shader->unbind();
  
  

// FIXME:  Create signed dist field.
  //FIXME: Create test cases for sphere
  if (_saveFrame)
  {
    //saveScreenShotTGA("screencap.tga", _currentResolution.x, _currentResolution.y);
	  saveScreenShotPNG("screencap.png", _currentResolution.x, _currentResolution.y);
	  _saveFrame = false || _drawDownsampled;
  }

  _renderTexture0->unbindFBO();

  _renderTexture0->drawFullscreen();
  
  if(_drawDownsampled)
  {
    //Now create and draw a downsampled image
    //draw the full res texture to a 96x96 FBO using a filtering shader
    _filterShader->bind();
    _downsampledTexture->bindFBO();
    _renderTexture0->bindTargetToChannel(0);
    _filterShader->setUniform("inputImageTexture",0);
    _filterShader->setUniform("textureWidth",_imageDim.x);
    _filterShader->setUniform("textureHeight",_imageDim.y);
    _fullscreenMesh->drawBuffers();
    _filterShader->unbind();
    
    if (_saveFrame)
    {
      saveScreenShotPNG("screencap96x96.png", _imageDim.x, _imageDim.y);
      _saveFrame = false;
    }
    _downsampledTexture->unbindFBO();
  
    if(!_stretchImage)
    {
      //Now draw the downsampled texture to the screen's upper right corner
      float imageScale = (int)_screenDim.x /(96*3);
      if(imageScale < 1.0f) imageScale = 1.0f;
      
      
      int dim = 96*imageScale;
      _downsampledTexture->debugDraw(glm::ivec4(_screenDim.x-dim,_screenDim.y-dim,dim,dim));
    }
    else{
      _downsampledTexture->debugDraw(glm::ivec4(0,0,_screenDim.x,_screenDim.y));
    }
  }
  
  
  glEnable(GL_DEPTH_TEST);
  
  
  //swap the render targets
  RenderTexture* tmp = _renderTexture0;
  _renderTexture0 = _renderTexture1;
  _renderTexture1 = tmp;

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
  if(key.key == 'D' && key.action == KeyInfo::KeyAction::RELEASE)
  {
	  if (key.mod & GLFW_MOD_SHIFT != 0)
    {
      _stretchImage = ! _stretchImage;
    }
    else
    {
      _drawDownsampled = !_drawDownsampled;
    }
  }
  
  if(key.key == 'Z' && key.action == KeyInfo::KeyAction::RELEASE)
  {
	  if (key.mod & GLFW_MOD_SHIFT != 0)
	  {
		  if (++_elevationIDX >= (unsigned)_elevations.size())
		  {
			  _elevationIDX = 0;
		  }
	  }
	  else
	  {
		  if (--_elevationIDX <  0)
		  {
			  _elevationIDX = (unsigned)_elevations.size() - 1;
		  }
	  }
    resetFBOs();
	  printf("Elevation[%d]: %d\n", (int)_elevationIDX,(int)_elevations[_elevationIDX]);
  }
  if(key.key == 'X' && key.action == KeyInfo::KeyAction::RELEASE)
  {
	  if ((key.mod & GLFW_MOD_SHIFT) != 0)
	  {
		  if (++_azimuthIDX >= (unsigned)_azimuths.size())
		  {
			  _azimuthIDX = 0;
		  }
	  }
	  else 
	  {
		  if (--_azimuthIDX <  0)
		  {
			  _azimuthIDX = (unsigned)_azimuths.size() - 1;
		  }
	  }
    resetFBOs();
  }
  if(key.key == 'C' && key.action == KeyInfo::KeyAction::RELEASE)
  {
	  if ((key.mod & GLFW_MOD_SHIFT) == 0)
	  {
		  if (++_lightingIDX >= (int)_lighting.size())
		  {
			  _lightingIDX = 0;
		  }
	  }
	  else 
	  {
		  if (--_lightingIDX <  0)
		  {
			  _lightingIDX = (int)_lighting.size() - 1;
		  }

	  }
    printf("%1f, %1f, %1f, %1f, %1f, %1f\n",
           _lightSwitching[_lightingIDX][0],
           _lightSwitching[_lightingIDX][1],
           _lightSwitching[_lightingIDX][2],
           _lightSwitching[_lightingIDX][3],
           _lightSwitching[_lightingIDX][4],
           _lightSwitching[_lightingIDX][5]);
    resetFBOs();
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
