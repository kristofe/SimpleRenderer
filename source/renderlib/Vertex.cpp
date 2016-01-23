#include "vertex.h"

namespace renderlib{


//FIXME: There should be a clearer way to get attribute/name binding data. Right
//there is shader location binding and vertext attribute data.  One is used
//for glBindAttributeLocation before a shader links and the other is used for
//setting up a VAO with glEnableVertexAttribArray and glVertexAttribPointer.
void Vertex::getShaderAttributes(std::vector<ShaderAttributeData> &attributes)
{
  attributes.push_back(ShaderAttributeData(
                                           "position",
                                           GL_FLOAT_VEC3,
                                           LocationSlot::POSITION)
                       );
  attributes.push_back(ShaderAttributeData(
                                           "normal",
                                           GL_FLOAT_VEC3,
                                           LocationSlot::NORMAL)
                       );
  attributes.push_back(ShaderAttributeData(
                                           "uv",
                                           GL_FLOAT_VEC2,
                                           LocationSlot::UV)
                       );
  attributes.push_back(ShaderAttributeData(
                                           "tangent",
                                           GL_FLOAT_VEC4,
                                           LocationSlot::TANGENT)
                       );
  attributes.push_back(ShaderAttributeData(
                                           "color",
                                           GL_FLOAT_VEC4,
                                           LocationSlot::COLOR)
                       );
}

const std::vector<ShaderAttributeData> Vertex::getShaderAttributes()
{
  std::vector<ShaderAttributeData> attributes;
  Vertex::getShaderAttributes(attributes);
  return attributes;
}

void Vertex::getVertexAttributes(std::vector<VertexAttributeInfo> &attributes)
{
  unsigned long accumSize = 0;
  size_t sizeOfVertex = sizeof(Vertex);

  attributes.push_back(VertexAttributeInfo(
                          "position",
                          BUFFER_OFFSET(accumSize),
                          GL_FLOAT,
                          3,
                          LocationSlot::POSITION,
                          sizeOfVertex)
                        );

  accumSize += sizeof(Vector3);
  attributes.push_back(VertexAttributeInfo(
                         "normal",
                         BUFFER_OFFSET(accumSize),
                         GL_FLOAT,
                         3,
                         LocationSlot::NORMAL,
                         sizeOfVertex)
                        );

  accumSize += sizeof(Vector3);
  attributes.push_back(VertexAttributeInfo(
                         "uv",
                         BUFFER_OFFSET(accumSize),
                         GL_FLOAT,
                         2,
                         LocationSlot::UV,
                         sizeOfVertex)
                        );

  accumSize += sizeof(Vector2);
  attributes.push_back(VertexAttributeInfo(
                         "tangent",
                         BUFFER_OFFSET(accumSize),
                         GL_FLOAT,
                         4,
                         LocationSlot::TANGENT,
                         sizeOfVertex)
                        );

  accumSize += sizeof(Vector4);
  attributes.push_back(VertexAttributeInfo(
                         "color",
                         BUFFER_OFFSET(accumSize),
                         GL_FLOAT,
                         4,
                         LocationSlot::COLOR,
                         sizeOfVertex)
                        );
}

const std::vector<VertexAttributeInfo> Vertex::getVertexAttributes()
{
  std::vector<VertexAttributeInfo> attributes;
  Vertex::getVertexAttributes(attributes);
  return attributes;
}

int Vertex::getStrideInBytes()
{
  return sizeof(Vertex)*sizeof(float);
}

size_t Vertex::getStride()
{
  return sizeof(Vertex);
}

}
