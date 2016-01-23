//
//  SimpleDistanceFieldRenderer.cpp
//  Renderer
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include "SimpleDistanceFieldRenderer.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "RenderManager.h"
#include "GLFWTime.h"
#include "ObjectIDGenerator.h"

//Get rid of this once we have a texture class working
#include "OpenGLHelper.h"

namespace renderlib{

SimpleDistanceFieldRenderer::SimpleDistanceFieldRenderer()
{
  _renderSortValue = IRenderable::DefaultSortValue;
  _id = ObjectIDGenerator::getInstance().getNextID();
}

SimpleDistanceFieldRenderer::~SimpleDistanceFieldRenderer()
{

}

void SimpleDistanceFieldRenderer::resize()
{
  _screenDim = RenderManager::getInstance().getFramebufferSize();
}
  
void SimpleDistanceFieldRenderer::init()
{
  resize();
  //For now just hard code some stuff in here to see if everything else works!
  _shader = new Shader();
  _shader->registerShader("shaders/passThroughVS.glsl", ShaderType::VERTEX);
  _shader->registerShader("shaders/dfNoisedShapesFrag.glsl", ShaderType::FRAGMENT);
  //_shader->registerShader("shaders/dfCubesFrag.glsl", ShaderType::FRAGMENT);
  _shader->compileShaders();
  _shader->linkShaders();

  _mesh = new Mesh();
  _mesh->createScreenQuad(Vector2(-1.0f, -1.0f), Vector2(1.0f, 1.0f));
  _mesh->bindAttributesToVAO(*_shader);

  _mvp.identity();

  _texture.loadPNG("assets/tiles.png",TextureFilterMode::LINEAR, TextureClampMode::CLAMP_TO_EDGE);
  _renderTexture.setupFBO(1024,1024,true, GL_RGBA8, GL_RGBA,TextureDataType::TDT_UBYTE);
  _renderTexture.setupDebugData(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));

}

void SimpleDistanceFieldRenderer::update(float time)
{


  Matrix4 ModelviewMatrix = Matrix4::identity();

  float n = 1.0f;
  float f = 100.0f;
  float fieldOfView = 0.7f;

  Matrix4 ProjectionMatrix = Matrix4::perspective(fieldOfView, 1, n, f);
  _mvp = ProjectionMatrix * ModelviewMatrix;

}

void SimpleDistanceFieldRenderer::preRender()
{


}

void SimpleDistanceFieldRenderer::draw()
{
  _shader->bind();

  _shader->setUniform("uTime", GLFWTime::getCurrentTime());
  _shader->setUniform("uResoltion", Vector3(_screenDim.x,_screenDim.y,0));
  _mesh->drawBuffers();
  _shader->unbind();

}

void SimpleDistanceFieldRenderer::postRender()
{


}
void SimpleDistanceFieldRenderer::handleKey(KeyInfo& key)
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
  
void SimpleDistanceFieldRenderer::handlePointer(std::vector<PointerInfo>& pointers)
{
}

}// namespace renderlib
