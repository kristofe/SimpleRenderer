//
//  RenderTexture.cpp
//  VolumeRenderer
//
//  Created by Kristofer Schlachter on 7/14/14.
//
//
#include "RenderTexture.h"
#include "glutil.h"
#include "ImageUtils.h"

namespace renderlib {

RenderTexture::RenderTexture()
{
  _fboProxy = std::make_shared<FBOProxy>();
  _clearColor = Color::black();
  _numRenderTargets = 1;
  //setupFullscreenData();
}
  
RenderTexture::~RenderTexture()
{
  _fboProxy.reset();
  if(_debugMesh != nullptr)
  {
    delete _debugMesh;
    _debugMesh = nullptr;
  }
  
  if(_debugShader != nullptr)
  {
    delete _debugShader;
    _debugShader = nullptr;
  }

  if (_fullscreenMesh != nullptr)
  {
	  delete _fullscreenMesh;
	  _fullscreenMesh = nullptr;
  }

  if (_fullscreenShader != nullptr)
  {
	  delete _fullscreenShader;
	  _fullscreenShader = nullptr;
  }
  if (_tonemappingMesh != nullptr)
  {
	  delete _tonemappingMesh;
	  _tonemappingMesh = nullptr;
  }

  if (_tonemappingShader != nullptr)
  {
	  delete _tonemappingShader;
	  _tonemappingShader = nullptr;
  }
}
  
Vector2 RenderTexture::getDimensions()
{
  return Vector2((float)_fboProxy->width, (float)_fboProxy->height);
}

void RenderTexture::bindAllTargetsStartingAt(int textureChannel)
{
  for(int i = 0; i < _numRenderTargets; i++){
    _fboProxy->bindToChannel(textureChannel + i, (int)(FBOProxy::ATTACHMENT::COLOR_ATTACHMENT0 + i));
  }
}

void RenderTexture::bindTargetToChannel(int textureChannel, FBOProxy::ATTACHMENT mrtChannel)
{
  _fboProxy->bindToChannel(textureChannel, (int)mrtChannel);
}
  
void RenderTexture::bindFBO()
{
  _fboProxy->bindFBO();
}

void RenderTexture::unbindFBO()
{
  _fboProxy->unbindFBO();
}

void RenderTexture::setupFBO(int width, int height, bool create, GLenum internalFormat, GLenum format, TextureDataType dataType, int numRenderTargets)
{
  _numRenderTargets = numRenderTargets;
  _fboProxy->setupFBO(width, height, create, internalFormat, format, dataType, _numRenderTargets);
}

void RenderTexture::setupDepthFBO(int width, int height)
{
  _fboProxy->setupDepthFBO(width, height);
}

void RenderTexture::matchFBOSizeToViewport()
{
  _fboProxy->matchFBOSizeToViewport();
}

void RenderTexture::setFBOSize(int w, int h)
{
  _fboProxy->setFBOSize(w,h);
}

void RenderTexture::debugDraw()
{

  if(_debugMesh == nullptr) return;
  
  _debugShader->bind();

  bindAllTargetsStartingAt(0);
  
  _debugShader->setUniform("uTextureCount",(float)_fboProxy->numColorAttachments);
  _debugShader->setUniform("uTexture0",0);
  _debugShader->setUniform("uTexture1",1);
  _debugShader->setUniform("uTexture2",2);
  _debugShader->setUniform("uTexture3",3);
  _debugMesh->drawBuffers();

  _debugShader->unbind();
}
 void RenderTexture::debugDraw(glm::ivec4 viewport)
  {
    
    if(_debugMesh == nullptr) return;
    
    _debugShader->bind();
    
    bindAllTargetsStartingAt(0);
    
    _debugShader->setUniform("uTexture0",0);
    
    int cachedViewport[4];
    glGetIntegerv(GL_VIEWPORT, cachedViewport);
    glViewport(viewport.x,viewport.y, viewport.z, viewport.w);
    GetGLError();
    _debugMesh->drawBuffers();
    glViewport(cachedViewport[0], cachedViewport[1],
               cachedViewport[2], cachedViewport[3]);
    GetGLError();
    _debugShader->unbind();
}
void RenderTexture::setupDebugData(Vector2 min, Vector2 max)
{
	if (_debugShader != nullptr)
		return;

  std::stringstream fsSource;
  fsSource
  << "#version 150\n"
  << "  in vec2 vUV;\n"
  << "  out vec4 color;\n"
  << "\n"
  << "  uniform float uTextureCount;\n"
  << "  uniform sampler2D uTexture0;\n"
  << "  uniform sampler2D uTexture1;\n"
  << "  uniform sampler2D uTexture2;\n"
  << "  uniform sampler2D uTexture3;\n"
  << "\n"
  << "  void main(void) {\n"
  << "    vec4 c;\n"
  << "    vec2 uv = vUV;\n"
  << "    if(uTextureCount > 1.0){\n"
  << "      if(uv.x <= 0.5 && uv.y <= 0.5) {//texture0\n"
  << "        c = texture(uTexture0, vUV * 2.0);\n"
  << "      }\n"
  << "      else if(uv.x > 0.5 && uv.y <= 0.5) {//texture1\n"
  << "        uv = vec2(vUV.x - 0.5, vUV.y);\n"
  << "        c = texture(uTexture1, uv * 2);\n"
  << "        //c = vec4(1,0,0,1);\n"
  << "      }\n"
  << "      else if(uv.x <= 0.5 && uv.y > 0.5) {//texture2\n"
  << "        uv = vec2(vUV.x, vUV.y - 0.5);\n"
  << "        c = texture(uTexture2, uv * 2);\n"
  << "        //c = vec4(0,1,0,1);\n"
  << "      }\n"
  << "      else if(uv.x > 0.5 && uv.y > 0.5) {//texture3\n"
  << "        uv = vec2(vUV.x - 0.5, vUV.y - 0.5);\n"
  << "        c = texture(uTexture3, uv * 2);\n"
  << "        //c = vec4(0,0,1,1);\n"
  << "      }\n"
  << "    }\n"
  << "    else {\n"
  << "       c = texture(uTexture0, vUV);\n"
  << "    }\n"
  << "    color = c;\n"
  << "  }\n";

  std::stringstream vsSource;
  vsSource 
  << "#version 150\n"
  << "  in vec3 position;\n"
  << "  in vec3 normal;\n"
  << "  in vec2 uv;\n"
  << "  in vec4 tangent;\n"
  << "  in vec4 color;\n"
  << "\n"
  << "  uniform sampler2D uTexture0;\n"
  << "  uniform sampler2D uTexture1;\n"
  << "  uniform sampler2D uTexture2;\n"
  << "  uniform sampler2D uTexture3;\n"
  << "\n"
  << "  out vec2 vUV;\n"
  << "\n"
  << "  void main(void) {\n"
  << "    vUV = uv.xy;\n"
  << "    gl_Position = vec4(position,1.0);\n"
  << "  }\n";

  _debugShader = new Shader();
  _debugShader->registerShaderSource(vsSource.str(), ShaderType::VERTEX, "RenderTextureDebugVertShader");
  _debugShader->registerShaderSource(fsSource.str(), ShaderType::FRAGMENT, "RenderTextureDebugFragShader");
  _debugShader->setAttributeLocations(Mesh::getShaderAttributeLocations());
  _debugShader->compileShaders();
  _debugShader->linkShaders();

  _debugMesh = new Mesh();
  _debugMesh->createScreenQuad(min, max);
  _debugMesh->bindAttributesToVAO(*_debugShader);
}

//FIXME: LEAKED
Mesh* RenderTexture::_fullscreenMesh = nullptr;
//FIXME: LEAKED
Mesh* RenderTexture::_tonemappingMesh = nullptr;

void RenderTexture::drawFullscreen()
{
 	_fullscreenShader->bind();
 	bindTargetToChannel(0, FBOProxy::ATTACHMENT::COLOR_ATTACHMENT0);
	_fullscreenShader->setUniform("uTexture0", 0);
	_fullscreenMesh->drawBuffers();
  
	_fullscreenShader->unbind();
}
  

void RenderTexture::drawFullscreenToneMapped(int type)
{
 	_tonemappingShader->bind();
 	bindTargetToChannel(0, FBOProxy::ATTACHMENT::COLOR_ATTACHMENT0);
	_tonemappingShader->setUniform("uTexture0", 0);
	_tonemappingShader->setUniform("uType", type);
	_tonemappingMesh->drawBuffers();
  
	_tonemappingShader->unbind();
}

void RenderTexture::setupToneMappingData()
{

  _tonemappingShader = new Shader();
  _tonemappingShader->registerShader("shaders/tonemapping.vert", ShaderType::VERTEX);
  _tonemappingShader->registerShader("shaders/tonemapping.frag", ShaderType::FRAGMENT);
  _tonemappingShader->setAttributeLocations(Mesh::getShaderAttributeLocations());
  _tonemappingShader->compileShaders();
  _tonemappingShader->linkShaders();

	if (_tonemappingMesh == nullptr)
	{
		_tonemappingMesh = new Mesh();
		_tonemappingMesh->createScreenQuad(Vector2(-1.0f, -1.f), Vector2(1.0f, 1.0f));
		_tonemappingMesh->bindAttributesToVAO(*_tonemappingShader);
	}

}
void RenderTexture::setupFullscreenData()
{
  std::stringstream fsSource;
  fsSource
	  << "#version 150\n"
	  << "  in vec2 vUV;\n"
	  << "  out vec4 color;\n"
	  << "\n"
	  << "  uniform sampler2D uTexture0;\n"
	  << "\n"
	  << "  void main(void) {\n"
	  << "    vec3 c = texture(uTexture0, vUV).rgb;\n"
	  << "    color = vec4(c, 1.0);\n"
    << "  }\n";

  std::stringstream vsSource;
  vsSource
    << "#version 150\n"
    << "  in vec3 position;\n"
    << "  in vec3 normal;\n"
    << "  in vec2 uv;\n"
    << "  in vec4 tangent;\n"
    << "  in vec4 color;\n"
    << "\n"
    << "  uniform sampler2D uTexture0;\n"
    << "\n"
    << "  out vec2 vUV;\n"
    << "\n"
    << "  void main(void) {\n"
    << "    vUV = uv.xy;\n"
    << "    gl_Position = vec4(position,1.0);\n"
    << "  }\n";

  _fullscreenShader = new Shader();
  _fullscreenShader->registerShaderSource(vsSource.str(), ShaderType::VERTEX, "RenderTextureFullscreenVertShader");
  _fullscreenShader->registerShaderSource(fsSource.str(), ShaderType::FRAGMENT, "RenderTextureFullscreenFragShader");
  _fullscreenShader->setAttributeLocations(Mesh::getShaderAttributeLocations());
  _fullscreenShader->compileShaders();
  _fullscreenShader->linkShaders();

	if (_fullscreenMesh == nullptr)
	{
		_fullscreenMesh = new Mesh();
		_fullscreenMesh->createScreenQuad(Vector2(-1.0f, -1.f), Vector2(1.0f, 1.0f));
		_fullscreenMesh->bindAttributesToVAO(*_fullscreenShader);
	}

}
  
void RenderTexture::clear()
{
  _fboProxy->clear(_clearColor);
  
}

} // namespace renderlib
