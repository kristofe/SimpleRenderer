//
//  SimpleDistanceFieldRenderer.h
//  Renderer
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#ifndef __Renderer__SimpleDistanceFieldRenderer__
#define __Renderer__SimpleDistanceFieldRenderer__

#include <iostream>
#include "vmath.hpp"
#include "InputManager.h"
#include "IRenderable.h"
#include "Texture.h"
#include "RenderTexture.h"

namespace renderlib {
  
  class Mesh;
  class Shader;
  class TextureProxy;
  
  class SimpleDistanceFieldRenderer : public IRenderable
  {
  public:
    SimpleDistanceFieldRenderer();
    virtual ~SimpleDistanceFieldRenderer();
    
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
    SimpleDistanceFieldRenderer(const SimpleDistanceFieldRenderer&) = delete;
    SimpleDistanceFieldRenderer & operator=(const SimpleDistanceFieldRenderer&) = delete;
    
  protected:
    int _renderSortValue;
    uint32_t _id;
    
    Mesh* _mesh;
    Shader* _shader;
    vmath::Matrix4 _mvp;
    vmath::Matrix4 _m;
    vmath::Vector2 _screenDim;
    
    //TextureProxy* _tex;
    Texture _texture;
    RenderTexture _renderTexture;
  };

} // namespace renderlib
#endif /* defined(__Renderer__SimpleDistanceFieldRenderer__) */
