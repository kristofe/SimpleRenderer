#ifndef __Splash__Painter_
#define __Splash__Painter_

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
  
  class Painter : public IRenderable
  {
  public:
    Painter();
    virtual ~Painter();
    
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
    void reset() {};
    
    void  simulate();
    int  getRenderSortValue() {return _renderSortValue;};
    uint32_t  getRenderObjectID() {return _id;};
    void  setRenderObjectID(uint32_t id){ _id = id;};
    void  swapRenderTargets();
    void  setupDisplayShaders();
    void  setupSimulationShaders();
    void  debugDraw();
    void  resize();

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    Painter(const Painter&) = delete;
    Painter & operator=(const Painter&) = delete;
    
  protected:
    const float LOW_DAMPING = 0.9985f;
    const float HIGH_DAMPING = 0.995f;
    
    int _renderSortValue;
    uint32_t _id; 
    Mesh* _screenMesh;
    Mesh* _waterMesh;
    Shader* _shader;
    
    Shader* _simulationShader;
    Shader* _copyTextureShader;
    RenderTexture* _rt0;
    RenderTexture* _rt1;
    RenderTexture* _refractionMap;
    Texture  _canvasTexture;

    PointerInfo _pointer;
    Vector2 _fboSize;
    
    
    Matrix4 _waterModelView;
    Matrix4 _waterModel;
    Matrix4 _camera;
    Matrix4 _projection;
    Matrix4 _mvp;
    Matrix4 _waterMVP;
    Vector3 _eyePosition;
    float   _rotAngle;
    float   _sensorAspectRatio;
    Color   _currentColor;
    float   _decayRate;
    float   _minPressure;
  
    bool    _showDebug;
    bool    _smoothingEnabled;
  
    Color   _colors[10];

  };

} // namespace renderlib
#endif /* defined(__Splash__WaveSim__) */
//TODO: Use alpha channel to deform mesh and create normals.
//TODO:  Get alpha channel drawing as grayscale.  Make it an option for
//debug draw on render target.
//Add options for normal drawing as well
//Add option to render MRT as 4 images appropriately rendered.

//FIXME: Bug in resizing FBO.  Look at setupFBO code.  Probably bad viewport size
//And then that isn't updated in wavesim.. so mouse pointer shows up wrapped.???

/*
 
 Kristofer Schlachter: deposition of ink.
 Kristofer Schlachter: if I do additive ink/paint it will "fill" very quickly
 Kristofer Schlachter: from one frame to the next I have to be careful how I accumulate ink into the image
 Kristofer Schlachter: i think that the pressure differences will be lost quickly if the ink is not added carefully
 ilyarosenberg: Yeah, that's a good point.
 Kristofer Schlachter: I am going to assign rgb into the image independent of pressure.  and accumulate in the alpha channel.
 Kristofer Schlachter: and the height field will be the alpha channel
 Kristofer Schlachter: that way we can get normals and "ridging"
 Kristofer Schlachter: but the color won't blow out to white
 Kristofer Schlachter: and I can use some function of alpha to brighten or modify the color somehow.  Maybe use it for the canvas texture
 ilyarosenberg: Yeah, I don't know what the right answer is. I can totally see that if we just accumulate pressure every frame, it's just going to saturate.
 ilyarosenberg: What if we used inter-frame differences? And only looked at positive differences, then we would only deposit ink at the leading edge of the brush.
 Kristofer Schlachter: I am going to use a floating point textures which will help with range
 Kristofer Schlachter: yeah i can save the previous frame and bind it and do a compare.  maybe have another texture that stores frame counter per pixel.
 Kristofer Schlachter: I can render to multiple textures at once.  It is called Multiple Render Targets (Sorry if that sounded patronizing but I wanted to say it so I could just refer to it as MRT)
 ilyarosenberg: I think if you just take the difference between frames, and clamp with an abolute value, you'll just see areas where pressure is increasing, and then you can just accumulate in those areas. That might create a very nice effect.
 Kristofer Schlachter: i guess you could just subtract the current image pressure before adding it
 Kristofer Schlachter: and blend the colors if you want based upon alpha difference. or just overwrite if it is oil paint or something opaque
 ilyarosenberg: I think you want the absolute value in there, because otherwise you would subtract paint at the trailing edge of the brush.
 Kristofer Schlachter: or clamp neg to zero?
 ilyarosenberg: yeah
 
 */
