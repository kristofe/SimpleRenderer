//
//  SimpleDFPathTracer.cpp
//  Renderer
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include "SimpleDFPathTracer.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "RenderManager.h"
#include "GLFWTime.h"
#include "ObjectIDGenerator.h"

//Get rid of this once we have a texture class working
#include "OpenGLHelper.h"

namespace renderlib{

SimpleDFPathTracer::SimpleDFPathTracer()
{
  _renderSortValue = IRenderable::DefaultSortValue;
  _id = ObjectIDGenerator::getInstance().getNextID();
}

SimpleDFPathTracer::~SimpleDFPathTracer()
{

}

void SimpleDFPathTracer::resize()
{
  _screenDim = RenderManager::getInstance().getFramebufferSize();
}
  
void SimpleDFPathTracer::init()
{
  resize();
  //For now just hard code some stuff in here to see if everything else works!
  _shader = new Shader();
  _shader->registerShader("shaders/passThroughVS.glsl", ShaderType::VERTEX);
  //_shader->registerShader("shaders/pathTracer.glsl", ShaderType::FRAGMENT);
  _shader->registerShader("shaders/pathTracerStart.glsl", ShaderType::FRAGMENT);
  //_shader->registerShader("shaders/dfCubesFrag.glsl", ShaderType::FRAGMENT);
  _shader->compileShaders();
  _shader->linkShaders();

  _mesh = new Mesh();
  _mesh->createScreenQuad(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
  _mesh->bindAttributesToVAO(*_shader);

  _model.loadModelFromFile("assets/models/LionessLowPoly.obj", true, true);
  Mesh modelMesh;
  std::vector<Material> materials;
  _model.collapseMeshes(modelMesh, materials);
  modelMesh.fitIntoUnitCube();
  
  
  _mvp.identity();
  /*
  Mesh testMesh;
  testMesh.createSphereMeshData(4, 4);
  //testMesh.createCube(Vector3(0.5f), Vector3(1.0f));
  //testMesh.createTriangle();
  testMesh.fitIntoUnitCube();
  */

  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!
  //FIXME: There is a problem with the vertex format binding... UVs are invalid!

  
  //FIXME: Need to scale mesh to fit in a 1x1x1 sized box whose coordinates
  //range [0,1]
  //_texture.createDistanceFieldFromMesh(32, testMesh);
  
  Texture tmp;
  //_texture.createDistanceFieldFromMesh(128, modelMesh,true, "assets/lioness_df.bin");
  _texture.loadDistanceFieldFromDisk("assets/lioness_df.bin");

  //_texture.createPyroclasticDistanceField(64, 0.5f, 0.0f);
  
  //_texture.loadPNG("assets/tiles.png",TextureFilterMode::LINEAR, TextureClampMode::CLAMP_TO_EDGE);
  _texture.setupDebugData(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
  //_renderTexture.setupFBO(1024,1024,true, GL_RGBA8, GL_RGBA,TextureDataType::TDT_UBYTE);
  //_renderTexture.setupDebugData(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));

}

void SimpleDFPathTracer::update(float time)
{


  // Conversion from Euler angles (in radians) to Quaternion
  vec3 EulerAngles(time*0.6, time*0.3, 0);
  quat q = quat(EulerAngles);
  _m = glm::mat4_cast(q);
  
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
  glDisable(GL_DEPTH_TEST);
  _shader->bind();
  _texture.bindToChannel(0);
  _shader->setUniform("iGlobalTime", GLFWTime::getCurrentTime());
  _shader->setUniform("iResolution", Vector2(_screenDim.x,_screenDim.y));
  _shader->setUniform("iMouse", Vector2(_mousePos.x,_mousePos.y));
  _shader->setUniform("Density", 0);
  _shader->setUniform("uModelMatrix", _m);
  
  _mesh->drawBuffers();
  _shader->unbind();

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
