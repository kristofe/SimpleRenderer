//
//  SimpleDFPathTracer.h
//  Renderer
//
//  Created by Kristofer Schlachter on 1/4/16.
//
//

#ifndef __Renderer__SimpleDFPathTracer__
#define __Renderer__SimpleDFPathTracer__

#include <iostream>
#include "allmath.h"
#include "InputManager.h"
#include "IRenderable.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "Model.h"

namespace renderlib {
  
  class Mesh;
  class Shader;
  class TextureProxy;
  
  class SimpleDFPathTracer : public IRenderable
  {
  public:
    SimpleDFPathTracer();
    virtual ~SimpleDFPathTracer();
    
    void init();
    void update(float time);
    void fixedUpdate(){};
    
    void preRender();
    void draw();
    void postRender();
    void resize();
    void reset(){};
    void resetFBOs();
    
    int  getRenderSortValue() {return _renderSortValue;};
    
    //IControllable methods
    virtual void handleKey(KeyInfo& key);
    virtual void handlePointer(std::vector<PointerInfo>& pointers);
    uint32_t getID() { return _id;};

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    SimpleDFPathTracer(const SimpleDFPathTracer&) = delete;
    SimpleDFPathTracer & operator=(const SimpleDFPathTracer&) = delete;
    
  protected:
    int _renderSortValue;
    uint32_t _id;
    float _gridResolution;
    
    Mesh* _mesh;
    Shader* _shader;
    vmath::Matrix4 _mvp;
    glm::mat4 _m;
    glm::mat4 _mInverse;
    glm::mat4 _cameraMatrix;
    glm::vec2 _screenDim;
    glm::vec2 _imageDim;
    glm::vec2 _currentResolution;
	bool _useScreenResolution;
	bool _saveFrame;
	int _elevationIDX;
	int _azimuthIDX;
	int _lightingIDX;
	glm::vec3 _cameraPosition;

    glm::vec3 _trans, _min, _max;
  
  float _lightSwitching[6][4] = {
      {1.0f, 0.0f, 0.0f, 0.0f},
      {1.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 0.0f},
      {0.0f, 0.0f, 0.0f, 1.0f},
      {1.0f, 1.0f, 0.0f, 0.0f},
      {0.0f, 0.0f, 1.0f, 1.0f}
    };
    Model _model;

	std::vector<std::string> categories{ "animal","human", "plane", "truck", "car" };
	//TODO: figure out each instance for each category
	std::vector<int> instance{ 0,1,2,3,4,5,6,7,8,9 };//Instances are kind of categories (like lion)
	std::vector<float> _elevations{ 30.0f,35.0f,40.0f,45.0f,50.0f,55.0f,60.0f,65.0f,70.0f };
	std::vector<float> _azimuths{ 0.0f,20.0f,40.0f,60.0f,80.0f,100.0f,120.0f,140.0f,160.0f,180.0f,200.0f,220.0f,240.0f,260.0f,280.0f,300.0f,320.0f,340.0f };
	std::vector<int> _lighting{ 0,1,2,3,4,5 };

    
    //TextureProxy* _tex;
    Texture _texture;
    RenderTexture* _renderTexture0{nullptr};
    RenderTexture* _renderTexture1{nullptr};
    glm::vec2 _mousePos;
    int _numSamples{0};
  };

} // namespace renderlib
#endif /* defined(__Renderer__SimpleDFPathTracer__) */
