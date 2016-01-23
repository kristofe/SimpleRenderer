#ifndef VERTEX_H
#define VERTEX_H
#include "OpenGLHelper.h"
#include "vmath.hpp"
#include <string>
#include <vector>

namespace renderlib{

class Vertex
{
public:
  enum LocationSlot{
    POSITION = 0,
    NORMAL = 1,
    UV = 2,
    TANGENT = 3,
    COLOR = 4
  };
  
public:
  Vertex(){};
  Vertex(const Vertex& other)
  {
    this->position = other.position;
    this->normal = other.normal;
    this->uv = other.uv;
    this->tangent = other.tangent;
    this->color = other.color;
  }
  
  void operator=(const Vertex& other)
  {
    this->position = other.position;
    this->normal = other.normal;
    this->uv = other.uv;
    this->tangent = other.tangent;
    this->color = other.color;
  }
  
  static void getShaderAttributes(std::vector<ShaderAttributeData> &attributes);
  const std::vector<ShaderAttributeData> getShaderAttributes();
  static void getVertexAttributes(std::vector<VertexAttributeInfo> &attributes);
  const std::vector<VertexAttributeInfo> getVertexAttributes();
  int getStrideInBytes();
  static size_t getStride();
  

private:

//Data Members
public:
  Vector3 position;
  Vector3 normal;
  Vector2 uv;
  Vector4 tangent;
  Color   color;
};


}

#endif // VERTEX_H
