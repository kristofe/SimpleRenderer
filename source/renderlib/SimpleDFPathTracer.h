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

	void setLightingArrays(int idx) {
		for (int i = 0; i < 2; i++)
		{
			_currLights[i] = _lights[_lightSwitching[idx][i]];
			_currLightColors[i] = _lightColors[_lightSwitching[idx][i]];
		}
	}

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    SimpleDFPathTracer(const SimpleDFPathTracer&) = delete;
    SimpleDFPathTracer & operator=(const SimpleDFPathTracer&) = delete;
    
  protected:
    int _renderSortValue;
    uint32_t _id;
    float _gridResolution;
    float _cameraDistance{2.0f};
    float _verticalCameraFOV{30.0f};
    
    Mesh* _mesh;
    Mesh* _modelMesh;
    Shader* _shader;
    Shader* _filterShader;
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
	glm::vec3 _targetPoint;
    
    glm::vec3 _bboxCenter;
    glm::vec3 _bboxRadius;
    float _showBBox{1.0f};
    
    glm::vec3 _trans, _min, _max;
    float _targetHeight;
  
    int _lightSwitching[7][2] = {
	  {0, 1},
	  {0, 6},
	  {1, 6},
	  {2, 6},
	  {3, 6},
	  {4, 6},
	  {5, 6}
    };
	glm::vec4 _lights[7] = {
		//vec4(0.5, 3.2, -0.4, 1.0),
		vec4(0.26, 4.6, 0.2333, 1.086),
		vec4(-1.9, 3.35, 2.0,  1.56),//Correct!!! 
		vec4(-0.0, 3.0, 2.0, 0.65),
		vec4(-0.0, 3.0, 2.0, 0.65),
		vec4(-0.0, 3.0, 2.0, 0.65),
		vec4(0.2, 4.0, -2.0, 2.0),
		vec4(0.0, 0.0, 0.0, 0.0)
	};
	glm::vec3 _lightColors[7] = {
		vec3(1.18),
		vec3(0.74),
		vec3(0.1),
		vec3(0.1),
		vec3(0.1),
		vec3(0.1),
		vec3(0.0)

	};

	glm::vec4 _currLights[2];
	glm::vec3 _currLightColors[2];

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
    RenderTexture* _downsampledTexture{nullptr};
    Mesh* _fullscreenMesh;
    bool _drawDownsampled{true};
    bool _stretchImage{false};
    glm::vec2 _mousePos;
  };

} // namespace renderlib
#endif /* defined(__Renderer__SimpleDFPathTracer__) */
