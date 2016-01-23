//
//  TrainingDataRenderer.h
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#ifndef __Splash__TrainingDataRenderer__
#define __Splash__TrainingDataRenderer__

#include <iostream>
#include "vmath.hpp"
#include "IRenderable.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "RenderManager.h"
#include "Model.h"
#include "FPSController.h"

namespace renderlib {
  
  class Mesh;
  class Shader;
  class TextureProxy;
  
  class TrainingDataRenderer : public IRenderable
  {
  public:
    TrainingDataRenderer();
    virtual ~TrainingDataRenderer();
    
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
    void loadTrainingData(const std::string& filename, bool labels);
    
    int  getRenderSortValue() {return _renderSortValue;};
    uint32_t  getID() {return _renderObjectID;};

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    TrainingDataRenderer(const TrainingDataRenderer&) = delete;
    TrainingDataRenderer & operator=(const TrainingDataRenderer&) = delete;
    
  protected:
    int _renderSortValue;
    uint32_t _renderObjectID;
    bool _debugDraw = {true};
    
    std::vector<std::vector<vec3> > _positions;
    std::vector<vec3> _labels;
    Mesh* _mesh;
    Mesh* _labelsMesh;
    Shader* _shader;
    vmath::Matrix4 _mvp;
    vmath::Matrix4 _mvpRot;
    //vmath::Matrix4 _m;
    mat4 _m;
    vec3 _position;
    quat _orientation;
    vec2 _angles;

    Model _model;
    
    uint32_t _currVertStart = {0};
    
    
    //TextureProxy* _tex;
    Texture _texture;
    RenderTexture _renderTexture;


    std::shared_ptr<FPSController> _fpsController;
  };

} // namespace renderlib
#endif /* defined(__Splash__TrainingDataRenderer__) */
