//
//  SingleRenderTarget.h
//  VolumeRenderer
//
//  Created by Kristofer Schlachter on 6/10/14.
//  Copyright (c) 2014 Kristofer Schlachter. All rights reserved.
//

#ifndef __VolumeRenderer__SingleRenderTarget__
#define __VolumeRenderer__SingleRenderTarget__

#include <iostream>
#include <sstream>
#include "vmath.hpp"
#include "Texture.h"

namespace renderlib{
  
using namespace vmath;
  
class SimpleRenderTarget
{
public:
  //FIXME: this should be in the renderer or some global class header
  enum DataType{
    UBYTE,
    USHORT,
    FLOAT16,
    FLOAT32
  };
  
public:
  SimpleRenderTarget(int width, int height, DataType dataType);
  void initDebugData(int min, int max);
  inline Texture& getTexture();
  void resize(int width, int height);
  inline void bind();
  inline void unbind();
  
  //Debug Stuff
  //void debugDraw(Matrix4& projMat, float time);
  //void setupMaterial();
  
  //C++ 11 way of hiding these methods
  //Default constructor, no copy constructor or copy assignment operato
  SimpleRenderTarget() = default;
  SimpleRenderTarget(const SimpleRenderTarget&) = delete;
  SimpleRenderTarget & operator=(const SimpleRenderTarget&) = delete;
  
protected:
  Texture texture;
  int width;
  int height;
  DataType dataType;
  //Debug stuff
  /*
  Matrix4 transform;
  Mesh debugMesh;
  Material debugMaterial;
   */
};

//inlineable methods
/*
void SimpleRenderTarget::initDebugData(int min, int max)
{
  //Debug stuff
  debugMesh = new Mesh();
  debugMaterial = new Material();
  debugMesh.createScreenQuad(min, max);
  setupMaterial();
}

  Texture& SimpleRenderTarget::getTexture(){
  return texture;
};
*/
void SimpleRenderTarget::resize(int width, int height)
{
  if(this->width != width || this->height  != height){
    this->width = width;
    this->height = height;
    texture.setupFBO(width, height, false);
  }
};

void SimpleRenderTarget::bind()
{
  texture.bindFBO();
};

void SimpleRenderTarget::unbind()
{
  texture.unbindFBO();
};




///////////////////////////////////////////////////////////////////////////////
// Debug Drawing routines
///////////////////////////////////////////////////////////////////////////////
/*
void debugDraw(Matrix4& projMat, float time){
  
  debugMaterial.bind(debugMesh);
  debugMaterial.setUniforms(
                               transform.matrix.m,
                               transform.inverse.m,
                               transform.inverseTranspose.m,
                               projMat.m,
                               time
                           );
  drawArrays(debugMesh.primitiveType, 0, debugMesh.numItems);
};

void setupMaterial()
{
  var fsSource="  precision mediump float;\n";
  fsSource +="\n";
  fsSource +="    varying vec4 vPosition;\n";
  fsSource +="    varying vec2 vUV;\n";
  fsSource +="    varying vec4 vColor;\n";
  fsSource +="\n";
  fsSource +="    uniform sampler2D uTexture01;\n";
  fsSource +="\n";
  fsSource +="    void main(void) {\n";
  //fsSource +="      float t= sin(gl_PointCoord.x * 0.01 + uTime);\n"
  //fsSource +="      vec3 color = vec3(t,t,t);\n"
  fsSource +="      vec3 color = texture2D(uTexture01, vUV).rgb;\n"
  fsSource +="      gl_FragColor = vec4(color, 1.0);\n"
  fsSource +="    }\n";
  
  var vsSource="  precision mediump float;\n";
  vsSource +="    attribute vec3 aPosition;\n";
  vsSource +="    attribute vec2 aUV;\n";
  vsSource +="\n";
  vsSource +="    uniform sampler2D uTexture01;\n";
  vsSource +="\n";
  vsSource +="    varying vec4 vPosition;\n";
  vsSource +="    varying vec2 vUV;\n";
  vsSource +="\n";
  vsSource +="    void main(void) {\n";
  vsSource +="        vUV = aUV.xy;\n";
  vsSource +="        gl_Position = vec4(aPosition,1.0);\n";
  vsSource +="    }\n";
  
  this.debugMaterial.shader.initShaderWithSource(fsSource,vsSource);
  this.debugMaterial.zTest = false;
  this.debugMaterial.zWrite = false;
  this.debugMaterial.lineWidth = 1.0;
  this.debugMaterial.setTexture(this.texture);
  
};
 */
  
}//end namespace renderlib

#endif /* defined(__VolumeRenderer__SingleRenderTarget__) */
