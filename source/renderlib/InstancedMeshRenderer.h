//
//  InstancedMeshRenderer.h
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#ifndef __Splash__InstancedMeshRenderer__
#define __Splash__InstancedMeshRenderer__

#include <iostream>
#include "vmath.hpp"
#include "IRenderable.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "RenderManager.h"

namespace renderlib {
  
  class Mesh;
  class Shader;
  class TextureProxy;
  
  class InstancedMeshRenderer : public IRenderable
  {
  public:
    InstancedMeshRenderer();
    virtual ~InstancedMeshRenderer();
    
    void init();
    void update(float time);
    void fixedUpdate(){};
    
    void preRender();
    void draw();
    void postRender();
    void resize() {};
    void reset() {};
    void handleKey(KeyInfo& key) {};
    void handlePointer(std::vector<PointerInfo>& pointers){};
    
    int  getRenderSortValue() {return _renderSortValue;};
    uint32_t  getID() {return _renderObjectID;};

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    InstancedMeshRenderer(const InstancedMeshRenderer&) = delete;
    InstancedMeshRenderer & operator=(const InstancedMeshRenderer&) = delete;
    
  protected:
    int _renderSortValue;
    uint32_t _renderObjectID;
    bool _debugDraw = {true};
    
    Mesh* _cubeMesh;
    Mesh* _roomMesh;
    Shader* _shader;
    vmath::Matrix4 _mvp;
    vmath::Matrix4 _mvpRot;
    //vmath::Matrix4 _m;
    mat4 _m;
    
    //TextureProxy* _tex;
    Texture _cubeTexture;
    Texture _wallTexture;
    RenderTexture _renderTexture;
  };

} // namespace renderlib
#endif /* defined(__Splash__InstancedMeshRenderer__) */
