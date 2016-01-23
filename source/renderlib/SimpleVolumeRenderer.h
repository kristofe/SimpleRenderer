//
//  SimpleVolumeRenderer.h
//  Renderer
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#ifndef __Renderer__SimpleVolumeRenderer__
#define __Renderer__SimpleVolumeRenderer__

#include <iostream>
#include "vmath.hpp"
#include "InputManager.h"
#include "IRenderable.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "trackball.h"

namespace renderlib {
  
  class Mesh;
  class Shader;
  class TextureProxy;
  
  class SimpleVolumeRenderer : public IRenderable
  {
  public:
    SimpleVolumeRenderer();
    virtual ~SimpleVolumeRenderer();
    
    void init();
    void update(float time);
    void fixedUpdate(){};
    
    void preRender();
    void draw();
    void postRender();
    void resize();
    void reset(){};
    
    int  getRenderSortValue() {return _renderSortValue;};
    
    //IControllable methods
    virtual void handleKey(KeyInfo& key);
    virtual void handlePointer(std::vector<PointerInfo>& pointers);
    uint32_t getID() { return _id;};

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    SimpleVolumeRenderer(const SimpleVolumeRenderer&) = delete;
    SimpleVolumeRenderer & operator=(const SimpleVolumeRenderer&) = delete;
    
  protected:
    int _renderSortValue;
    uint32_t _id;
    
    Mesh* _mesh;
    Shader* _shader;
    vmath::Matrix4 _mvp;
    vmath::Matrix4 _mv;
    vmath::Matrix4 _v;
    vmath::Matrix4 _m;
    vmath::Matrix4 _p;
    vmath::Vector2 _screenDim;
    vmath::Point3 _eyePosition;
    
    Texture _texture;
    RenderTexture _renderTexture;

    Trackball *_trackball;
    glm::vec3 _eyeOffset;
    float _rotAngle{ 0.0f };
  };

} // namespace renderlib
#endif /* defined(__Renderer__SimpleVolumeRenderer__) */
