#ifndef __Splash__WaveSim__
#define __Splash__WaveSim__

#include <iostream>
#include "InputManager.h"
#include "vmath.hpp"
#include "IRenderable.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "Texture.h"

namespace renderlib {
  
  class Mesh;
  class Shader;
  class TextureProxy;
  
  class WaveSim : public IRenderable
  {
  public:
    WaveSim();
    virtual ~WaveSim();
    
    //IRenderable methods
    void init();
    void update(float time);
    void fixedUpdate(){};
    
    void preRender();
    void draw();
    void postRender();

    //IControllable methods
    virtual void handleKey(KeyInfo& key);
    virtual void handlePointer(std::vector<PointerInfo>& pointers);
    uint32_t getID() { return _id;};
    
    void  simulate();
    int  getRenderSortValue() {return _renderSortValue;};
    uint32_t  getRenderObjectID() {return _id;};
    void  setRenderObjectID(uint32_t id){ _id = id;};
    void  swapRenderTargets();
    void  setupDisplayShaders();
    void  setupSimulationShaders();
    void  debugDraw();
    void  resize();
    void reset(){};

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    WaveSim(const WaveSim&) = delete;
    WaveSim & operator=(const WaveSim&) = delete;
    
  protected:
    const float LOW_DAMPING = 0.9985f;
    const float HIGH_DAMPING = 0.995f;
    
    int _renderSortValue;
    uint32_t _id; 
    Mesh* _screenMesh;
    Mesh* _waterMesh;
    Mesh* _tankMesh;
    Shader* _shader;
    Shader* _tankShader;
    
    Shader* _simulationShader;
    RenderTexture* _rt0;
    RenderTexture* _rt1;
    RenderTexture* _refractionMap;

    PointerInfo _pointer;
    Vector2 _fboSize;
    
    
    Texture _cubeMap;
    Texture _tankTexture;
    Matrix4 _waterModelView;
    Matrix4 _waterModel;
    Matrix4 _tankTransform;
    Matrix4 _camera;
    Matrix4 _projection;
    Matrix4 _mvp;
    Matrix4 _waterMVP;
    Vector3 _eyePosition;
    float   _rotAngle;
    float   _damping;
    float   _sensorAspectRatio;
	float   _lastSimTime;

  };

} // namespace renderlib
#endif /* defined(__Splash__WaveSim__) */
