//
//  Texture.h
//  VolumeRenderer
//
//  Created by Kristofer Schlachter on 7/14/14.
//
//

#ifndef __VolumeRenderer__RenderTexture__
#define __VolumeRenderer__RenderTexture__

#include <iostream>
#include <sstream>
#include <string>
#include <memory>
#include "vmath.hpp"
#include "OpenGLHelper.h"
#include "mesh.h"

namespace renderlib{

using namespace vmath;

class RenderTexture
{
public:

  //FIXME: RenderTexture should extend Texture.  Not be a separate class.
  //This is necessary for Material to just have an array of weak_ptr's to
  //Texture and be able to correctly bind them.
  //It is probably necessary to make FBOProxy extend TextureProxy instead of
  //being a separate class.
public:
  RenderTexture();
  ~RenderTexture();

  Vector2 getDimensions();
  void bindTargetToChannel(int textureTarget, FBOProxy::ATTACHMENT attachment = FBOProxy::ATTACHMENT::COLOR_ATTACHMENT0);
  void bindAllTargetsStartingAt(int textureChannel);
  void bindFBO();
  void unbindFBO();
  void setupFBO(int width, int height, bool create, GLenum internalFormat, GLenum format, TextureDataType dataType, int numRenderTargets = 1);
  void setupDepthFBO(int width, int height);

  void matchFBOSizeToViewport();
  void setFBOSize(int w, int h);

  void debugDraw();
  void debugDraw(glm::ivec4 viewport);
  void setupDebugData(Vector2 min, Vector2 max);

  void drawFullscreen();
  void setupFullscreenData();

  void setClearColor(Color c) { _clearColor = c; }
  void clear();
  
  unsigned int getPrimaryColorBufferID(){ return static_cast<unsigned int>(_fboProxy->colorTextureID[0]); }

  //C++ 11 way of hiding these methods
  //Default constructor, no copy constructor or copy assignment operato
  //Texture() = default;
  RenderTexture(const RenderTexture&) = delete;
  RenderTexture & operator=(const RenderTexture&) = delete;

protected:
  std::shared_ptr<FBOProxy> _fboProxy;
  Color _clearColor;
  Mesh* _debugMesh;
  Shader* _debugShader;

  static Mesh* _fullscreenMesh;
  Shader* _fullscreenShader;
  int _numRenderTargets;
};

} //namespace renderlib


#endif /* defined(__VolumeRenderer__RenderTexture__) */
