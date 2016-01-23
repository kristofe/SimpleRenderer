//
//  Camera.h
//  Splash
//
//  Created by Kristofer Schlachter on 7/15/14.
#pragma once

#include <iostream>
#include "OpenGLHelper.h"
#include "allmath.h"
#include "shader.h"
#include "mesh.h"
#include "Material.h"
#include <vector>
#include <memory>





namespace renderlib{
using glm::ivec3;
using glm::ivec2;
using glm::uvec2;
using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::quat;

class Camera
{
public:
  Camera() {};
  virtual ~Camera() {};

  mat4 getProjection(){return _projection;}
  mat4 getModelview(){return _modelview;}
  vec3 getEyePosition(){return vec3(_eyePosition);}
  float getFieldOfView(){return _fieldOfView;}

  void setModelview(mat4 m) {_modelview = m;}
  void setProjection(mat4 m) {_projection = m;}
  void setEyePosition(vec3 p) {_eyePosition = p;}
  void setFieldOfView(float f) {_fieldOfView = f;}


  Camera(const Camera&) = delete;
  Camera & operator=(const Camera&) = delete;
private:

protected:
  mat4 _modelview;
  mat4 _projection;
  vec3 _eyePosition;
  float _fieldOfView;

};

}//namespace renderlib


