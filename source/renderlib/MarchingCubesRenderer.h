//
//  MarchingCubesRenderer.h
//  Renderer
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#ifndef __Renderer__MarchingCubesRenderer__
#define __Renderer__MarchingCubesRenderer__

#include <iostream>
#include "vmath.hpp"
#include "InputManager.h"
#include "IRenderable.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "trackball.h"
#include "allmath.h"
#include "OpenGLHelper.h"

namespace renderlib {
#if 1
  class GridCell{
  public:
    float val[8];
    vec3 pos[8];
  };

#endif
  class Mesh;
  class Shader;
  class TextureProxy;
  
  class MarchingCubesRenderer : public IRenderable
  {
  public:
    MarchingCubesRenderer();
    virtual ~MarchingCubesRenderer();
    
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
    MarchingCubesRenderer(const MarchingCubesRenderer&) = delete;
    MarchingCubesRenderer & operator=(const MarchingCubesRenderer&) = delete;
  protected:
    void RenderMarchCube(float *data, ivec3 size, ivec3 gridsize, float isolevel);
    int Polygonise(GridCell &grid, float isolevel, vec3 *triangles);
    vec3 VertexInterp(float isolevel, vec3 p1, vec3 p2, float valp1, float valp2);
    
  protected:
    int _renderSortValue;
    uint32_t _id;

    Mesh* _mesh{ nullptr };
    Mesh* _meshGS{ nullptr };
    Shader* _shader{ nullptr };
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
  protected:
    //FROM CRASSIN

    vec3 sphereLightPos;
    vec3 viewOrient;


    //general flags
    bool pause;
    bool wireframe;

    //GLSL
    Shader programObject;
    Shader programObjectGS;
    Shader programObjectFS;

    GLuint edgeTableTex;
    GLuint triTableTex;

    GLuint dataFieldTex[3];

    vec3 cubeSize;
    vec3 cubeStep;

    ivec3 dataSize;

    float *dataField[3];
    float isolevel;

    bool animate;
    bool autoWay;
    bool enableVBO;
    bool enableSwizzledWalk;

    int mode;

    int curData;

    float *gridData;
    GLuint gridDataBuffId;
    GLuint gridDataSwizzledBuffId;

    

  };









} // namespace renderlib
#endif /* defined(__Renderer__MarchingCubesRenderer__) */
