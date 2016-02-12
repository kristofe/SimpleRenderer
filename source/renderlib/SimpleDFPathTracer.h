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
    glm::vec2 _screenDim;
    glm::vec2 _imageDim;
    glm::vec2 _currentResolution;
	bool _useScreenResolution;
	bool _saveFrame;
    
    Model _model;

	std::vector<std::string> categories{ "animal","human", "plane", "truck", "car" };
	//TODO: figure out each instance for each category
	std::vector<int> instance{ 0,1,2,3,4,5,6,7,8,9 };//Instances are kind of categories (like lion)
	std::vector<int> elevations{ 30,35,40,45,50,55,60,65,70 };
	std::vector<int> azimuths{ 0,20,40,60,80,100,120,140,160,180,200,220,240,260,280,300,320,340 };
	std::vector<int> lighting{ 0,1,2,3,4,5 };

    
    //TextureProxy* _tex;
    Texture _texture;
    RenderTexture _renderTexture;
    glm::vec2 _mousePos;
  };

} // namespace renderlib
#endif /* defined(__Renderer__SimpleDFPathTracer__) */
