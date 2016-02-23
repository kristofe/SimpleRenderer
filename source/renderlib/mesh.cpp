#include <cstring>
#include "OpenGLHelper.h"
#include "vmath.hpp"
#include "mesh.h"
#include "TriangleMesh.h"
#include "vertex.h"
#include "Collision.h"

namespace renderlib
{
  ///////////////////////////////////////////////////////////////////////////////
  // Mesh Class Methods
  ///////////////////////////////////////////////////////////////////////////////
  Mesh::Mesh() :bufferInfo(nullptr), _materialIndex(0)
  {
    //Vertex::GetAttributeInfo(&_attributes);
  }

  int Mesh::getVertexListSizeInBytes()
  {
    return (int)_verts.size()*sizeof(Vertex);
  }

  int Mesh::getIndexListSizeInBytes()
  {
    return (int)_indices.size()*sizeof(uint32_t);
  }

  void Mesh::drawBuffers()
  {
    bufferInfo->drawBuffer(0, _vertexCount);
  }
  
  void Mesh::drawBuffersRange(uint32_t startOffset, uint32_t numVertices)
  {
    bufferInfo->drawBuffer(startOffset, numVertices);
  }
  
  void Mesh::setPrimitiveTypeToPoints()
  {
    bufferInfo->primitiveType = GL_POINTS;
  }
  
  void Mesh::setPrimitiveTypeToTriangles()
  {
    bufferInfo->primitiveType = GL_TRIANGLES;
  }

  void Mesh::getBoundingBox(vec3& min, vec3& max)
  {
    min.x = _min.x;
    min.y = _min.y;
    min.z = _min.z;
    max.x = _max.x;
    max.y = _max.y;
    max.z = _max.z;
  }

  void Mesh::calculateBoundingBox(vec3& min, vec3& max)
  {
    _min.x = _min.y = _min.z = 99999999.0f;
    _max.x = _max.y = _max.z = -99999999.0f;
    for (Vertex v : _verts)
    {
      if (v.position.x < _min.x)
        _min.x = v.position.x;
      if (v.position.y < _min.y)
        _min.y = v.position.y;
      if (v.position.z < _min.z)
        _min.z = v.position.z;

      if (v.position.x > _max.x)
        _max.x = v.position.x;
      if (v.position.y > _max.y)
        _max.y = v.position.y;
      if (v.position.z > _max.z)
        _max.z = v.position.z;
    }

    getBoundingBox(min, max);
  }

  bool Mesh::constructBuffer()
  {
    if (bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }
    _verts.clear();

    for (int i = 0; i < (int)_positions.size(); ++i)
    {
      Vertex v;
      v.position = _positions[i];
      _verts.push_back(v);
    }

    for(int i = 0; i < (int)_normals.size(); ++i)
    {
      _verts[i].normal = _normals[i];
    }

    for(int i = 0; i < (int)_uvs.size(); ++i)
    {
      _verts[i].uv = _uvs[i];
    }

    for(int i = 0; i < (int)_tangents.size(); ++i)
    {
      _verts[i].tangent = _tangents[i];
    }

    for(int i = 0; i < (int)_colors.size(); ++i)
    {
      _verts[i].color = _colors[i];
    }

    _vertexCount = (uint32_t) _verts.size();
   
    bool expand = false;
    if (_vertexCount > _maxVertexCount)
    {
      _maxVertexCount = _vertexCount;
      expand = true;
    }

    if (_bufferConstructed == false){
      if (_indices.size() > 0)
      {
        bufferInfo->numIndices = _indices.size();
        bufferInfo->constructBuffer(&_verts[0].position.x,
          getVertexListSizeInBytes(),
          &_indices[0],
          getIndexListSizeInBytes());
      }
      else{
        bufferInfo->constructBuffer(&_verts[0].position.x, getVertexListSizeInBytes());
      }
      _bufferConstructed = true;
    }
    else
    {

      if (_indices.size() > 0)
      {
        bufferInfo->numIndices = _indices.size();
        bufferInfo->updateBuffer(&_verts[0].position.x,
          getVertexListSizeInBytes(),
          &_indices[0],
          getIndexListSizeInBytes(), expand);
      }
      else{
        bufferInfo->updateBuffer(&_verts[0].position.x, getVertexListSizeInBytes(),expand);
      }

    }

    return true;
  }


  bool Mesh::bindAttributesToVAO(Shader& program)
  {
    bufferInfo->bindAttributesToVAO(_verts[0].getVertexAttributes(), program);
    return true;

  }

  bool Mesh::bindAttributesToVAO()
  {
    bufferInfo->bindAttributesToVAO(_verts[0].getVertexAttributes());
    return true;;

  }

  void Mesh::enableVertexAttributes()
  {
    bufferInfo->enableVertexAttributes();

  }

  void Mesh::createPoint()
  {
    Vector3 norm(0, 0, 1);
    _positions.push_back(Vector3(0.0f,0.0f,0.0f));

    if (bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }

    bufferInfo->bufferType = BufferStorage::ARRAY_BUFFER;
    bufferInfo->memTransferBehavior = BufferUsage::STATIC_DRAW;
    bufferInfo->primitiveType = GL_POINTS;


    constructBuffer();
  }
  
  void Mesh::createTriangle()
  {
    Vector3 norm(0, 0, -1);
    _positions.push_back(Vector3(0.0f,0.0f,0.5f));
    _positions.push_back(Vector3(1.0f,0.0f,0.5f));
    _positions.push_back(Vector3(0.5f,1.0f,0.5f));
  
    _normals.push_back(norm);
    _normals.push_back(norm);
    _normals.push_back(norm);

    if (bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }

    bufferInfo->bufferType = BufferStorage::ARRAY_BUFFER;
    bufferInfo->memTransferBehavior = BufferUsage::STATIC_DRAW;
    bufferInfo->primitiveType = GL_TRIANGLES;


    constructBuffer();
  }

  void Mesh::createPointGrid(Vector3 cubeSize)
  {
    Vector3 cubeStep = Vector3(2.0f/cubeSize.x, 2.0f/cubeSize.y, 2.0f/cubeSize.z);
    //_positions.reserve(cubeSize.x*cubeSize.y*cubeSize.z);
    for (float k = -1; k < 1.0f; k += cubeStep.z)
    for (float j = -1; j < 1.0f; j += cubeStep.y)
    for (float i = -1; i < 1.0f; i += cubeStep.x){
      _positions.push_back(Vector3(i, j, k));
    }

    if (bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }

    bufferInfo->bufferType = BufferStorage::ARRAY_BUFFER;
    bufferInfo->memTransferBehavior = BufferUsage::STATIC_DRAW;
    bufferInfo->primitiveType = GL_POINTS;


    constructBuffer();
  }

  void Mesh::createScreenQuadIBO(Vector2 min,Vector2 max)
  {
    Vector3 norm(0,0,1);
    Vector3 p0( min.x, min.y, 0.0);
    Vector3 p1( max.x, min.y, 0.0);
    Vector3 p2( max.x, max.y, 0.0);
    Vector3 p3( min.x, max.y, 0.0);

    Vector2 uv0( 0.0f, 0.0f);
    Vector2 uv1( 1.0f, 0.0f);
    Vector2 uv2( 1.0f, 1.0f);
    Vector2 uv3( 0.0f, 1.0f);

    _positions.push_back(p0);
    _positions.push_back(p1);
    _positions.push_back(p2);
    _positions.push_back(p3);
    _normals.push_back(norm);
    _normals.push_back(norm);
    _normals.push_back(norm);
    _normals.push_back(norm);
    _uvs.push_back(uv0);
    _uvs.push_back(uv1);
    _uvs.push_back(uv2);
    _uvs.push_back(uv3);

    _indices.push_back(0);
    _indices.push_back(1);
    _indices.push_back(2);

    _indices.push_back(0);
    _indices.push_back(2);
    _indices.push_back(3);


    if(bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }

    bufferInfo->bufferType = BufferStorage::ARRAY_BUFFER;
    bufferInfo->memTransferBehavior = BufferUsage::STATIC_DRAW;
    bufferInfo->primitiveType = GL_TRIANGLES;

    bufferInfo->numIndices = (GLuint)_indices.size();

    constructBuffer();
  }

  void Mesh::createScreenQuad(Vector2 min,Vector2 max)
  {
    Color c(0.0, 0.0, 0.0, 1.0);
    Vector3 norm(0,0,1);
    Vector4 tangent(0,0,0,0);
    Vector3 p0( min.x, min.y, 0.0);
    Vector3 p1( max.x, min.y, 0.0);
    Vector3 p2( min.x, max.y, 0.0);
    Vector3 p3( min.x, max.y, 0.0);
    Vector3 p4( max.x, min.y, 0.0);
    Vector3 p5( max.x, max.y, 0.0);

    Vector2 uv0( 0.0f, 0.0f);
    Vector2 uv1( 1.0f, 0.0f);
    Vector2 uv2( 0.0f, 1.0f);
    Vector2 uv3( 0.0f, 1.0f);
    Vector2 uv4( 1.0f, 0.0f);
    Vector2 uv5( 1.0f, 1.0f);

    //First Triangle
    _positions.push_back(p0);
    _positions.push_back(p1);
    _positions.push_back(p2);
    _normals.push_back(norm);
    _normals.push_back(norm);
    _normals.push_back(norm);
    _tangents.push_back(tangent);
    _tangents.push_back(tangent);
    _tangents.push_back(tangent);
    _uvs.push_back(uv0);
    _uvs.push_back(uv1);
    _uvs.push_back(uv2);
    _colors.push_back(c);
    _colors.push_back(c);
    _colors.push_back(c);

    //Second Triangle
    _positions.push_back(p3);
    _positions.push_back(p4);
    _positions.push_back(p5);
    _normals.push_back(norm);
    _normals.push_back(norm);
    _normals.push_back(norm);
    _tangents.push_back(tangent);
    _tangents.push_back(tangent);
    _tangents.push_back(tangent);
    _uvs.push_back(uv3);
    _uvs.push_back(uv4);
    _uvs.push_back(uv5);
    _colors.push_back(c);
    _colors.push_back(c);
    _colors.push_back(c);


    if(bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }

    bufferInfo->bufferType = GL_ARRAY_BUFFER;
    bufferInfo->memTransferBehavior = GL_STATIC_DRAW;
    bufferInfo->primitiveType = GL_TRIANGLES;


    constructBuffer();
  }

  void Mesh::createSphereMeshData(int slices, int stacks)
  {
    for (int stack = 0; stack < stacks; stack++) {
      float theta = stack * PI / stacks;
      float thetaNext = (stack + 1) * PI / stacks;
      float sinTheta = sinf(theta);
      float cosTheta = cosf(theta);
      float sinThetaNext = sinf(thetaNext);
      float cosThetaNext = cosf(thetaNext);


      for (int slice = 0; slice < slices; slice++) {
        float phi = slice * 2.0f * PI / slices;
        float phiNext = (slice + 1) * 2.0f * PI / slices;
        float sinPhi = sinf(phi);
        float cosPhi = cosf(phi);
        float sinPhiNext = sinf(phiNext);
        float cosPhiNext = cosf(phiNext);

        float xTop = cosPhi * sinTheta;
        float xTopNext = cosPhiNext * sinTheta;
        float xBottom = cosPhi * sinThetaNext;
        float xBottomNext = cosPhiNext * sinThetaNext;

        float yTop = cosTheta;
        float yBottom = cosThetaNext;

        float zTop = sinPhi * sinTheta;
        float zTopNext = sinPhiNext * sinTheta;
        float zBottom = sinPhi * sinThetaNext;
        float zBottomNext = sinPhiNext * sinThetaNext;


        float u = 1.0f - (slice / slices);
        float uNext = 1.0f - ((slice + 1)/ slices);
        float v = 1.0f - (stack / stacks);
        float vNext = 1.0f - ((stack + 1) / stacks);


        Vector3 topPos(xTop, yTop, zTop);
        Vector3 topNextPos(xTopNext, yTop, zTopNext);
        Vector3 bottomPos(xBottom, yBottom, zBottom);
        Vector3 bottomNextPos(xBottomNext, yBottom, zBottomNext);

        Vector2 topUV(u,v);
        Vector2 topNextUV(uNext,v);
        Vector2 bottomUV(u,vNext);
        Vector2 bottomNextUV(uNext,vNext);

        normalize(topPos);
        normalize(topNextPos);
        normalize(bottomPos);
        normalize(bottomNextPos);

        Vector3 topTan = cross(normalize(topPos - bottomPos),topPos);
        Vector3 topNextTan = cross( normalize(topNextPos - bottomNextPos),topNextPos);
        Vector3 bottomTan = cross( normalize(topPos - bottomPos),bottomPos);
        Vector3 bottomNextTan = cross( normalize(topNextPos - bottomNextPos),bottomNextPos);

        //First Triangle
        _positions.push_back(topPos);
        _positions.push_back(topNextPos);
        _positions.push_back(bottomPos);
        _normals.push_back(topPos);
        _normals.push_back(topNextPos);
        _normals.push_back(bottomPos);
        _uvs.push_back(topUV);
        _uvs.push_back(topNextUV);
        _uvs.push_back(bottomUV);
        _tangents.push_back(Vector4(topTan));
        _tangents.push_back(Vector4(topNextTan));
        _tangents.push_back(Vector4(bottomTan));

        //Second Triangle
        _positions.push_back(topNextPos);
        _positions.push_back(bottomNextPos);
        _positions.push_back(bottomPos);
        _normals.push_back(topNextPos);
        _normals.push_back(bottomNextPos);
        _normals.push_back(bottomPos);
        _uvs.push_back(topNextUV);
        _uvs.push_back(bottomNextUV);
        _uvs.push_back(bottomUV);
        _tangents.push_back(Vector4(topNextTan));
        _tangents.push_back(Vector4(bottomNextTan));
        _tangents.push_back(Vector4(bottomTan));

      }
    }

    if(bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }

    bufferInfo->bufferType = GL_ARRAY_BUFFER;
    bufferInfo->memTransferBehavior = GL_STATIC_DRAW;
    bufferInfo->primitiveType = GL_TRIANGLES;

    constructBuffer();

  }

  void Mesh::createTriStripGridMeshData(int n, int m, bool tileUVs, float aspect)
  {

    float xpos = 0.0f;
    float ypos = 0.0f;
    float zpos = 0.0f;
    float xinc = 1.0f/n;
    float yinc = 1.0f/m;

    float x, y, y2, z, u, v, v2;
    Vector3 normal;
    Vector3 p1;
    Vector3 p2;
    Vector2 uv1;
    Vector2 uv2;
    Vector4 tangent;
    for(int j = 0; j < m; ++j){
      for(int i = 0; i <= n; ++i){
        x = 0.5f-xpos;
        y = 0.5f-ypos;
        y2 = y + yinc;
        z = zpos;

        u = 0.5f + x;
        v =  0.5f + y - yinc;
        v2 = 0.5f + y2 - yinc;

        x *= aspect;

        normal.set(0.0f, 0.0f, 1.0f);
        p1.set(x,y,z);
        p2.set(x,y2,z);
        uv1.set(u,v);
        uv2.set(u,v2);
        tangent.set(1.0f, 0.0f, 0.0f, 0.0f);



        //Part of degenerate triangle
        if(i == 0 && j > 0){
          _positions.push_back(p1);
          _normals.push_back(normal);
          _uvs.push_back(uv1);
          _tangents.push_back(tangent);
        }
        _positions.push_back(p1);
        _normals.push_back(normal);
        _uvs.push_back(uv1);
        _tangents.push_back(tangent);

        _positions.push_back(p2);
        _normals.push_back(normal);
        _uvs.push_back(uv2);
        _tangents.push_back(tangent);
        xpos += xinc;
      }
      //Part of degenerate triangle
      _positions.push_back(p2);
      _normals.push_back(normal);
      _uvs.push_back(uv2);
      _tangents.push_back(tangent);
      //vertices.push(x, y2, z, 0.0, 0.0, 1.0, u, v2, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

      ypos += yinc;
      xpos = 0.0;
    }

    if(bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }

    bufferInfo->bufferType = GL_ARRAY_BUFFER;
    bufferInfo->memTransferBehavior = GL_STATIC_DRAW;
    bufferInfo->primitiveType = GL_TRIANGLE_STRIP;

    constructBuffer();
  }


 void Mesh::createCube(Vector3 size, Vector3 uvScale)
  {
    const int numVerts = 6*6;
    const int numElements = 8;

    Vector3 unit = size*0.5f;
    float u = uvScale.x;
    float v = uvScale.y;
    float w = uvScale.z;

    float cube[numVerts*numElements] = {
      //Front Face
      -unit.x, -unit.y, unit.z, 0.f, 0.f, 1.f, 0.f*u, 0.f*v,//0
      unit.x,  unit.y, unit.z, 0.f, 0.f, 1.f, 1.f*u, 1.f*v,//2
      -unit.x,  unit.y, unit.z, 0.f, 0.f, 1.f, 0.f*u, 1.f*v,//1
      unit.x, -unit.y, unit.z, 0.f, 0.f, 1.f, 1.f*u, 0.f*v,//3
      unit.x,  unit.y, unit.z, 0.f, 0.f, 1.f, 1.f*u, 1.f*v,//2
      -unit.x, -unit.y, unit.z, 0.f, 0.f, 1.f, 0.f*u, 0.f*v,//0

      //Back Face
      unit.x, -unit.y, -unit.z, 0.f, 0.f, -1.f, 0.f*u, 0.f*v,//7
      -unit.x,  unit.y, -unit.z, 0.f, 0.f, -1.f, 1.f*u, 1.f*v,//5
      unit.x,  unit.y, -unit.z, 0.f, 0.f, -1.f, 0.f*u, 1.f*v,//6
      -unit.x, -unit.y, -unit.z, 0.f, 0.f, -1.f, 1.f*u, 0.f*v,//4
      -unit.x,  unit.y, -unit.z, 0.f, 0.f, -1.f, 1.f*u, 1.f*v,//5
      unit.x, -unit.y, -unit.z, 0.f, 0.f, -1.f, 0.f*u, 0.f*v,//7

      //Top Face
      -unit.x,  unit.y, unit.z, 0.f, 1.f, 0.f, 0.f*u, 0.f*w,//1
      unit.x,  unit.y, -unit.z, 0.f, 1.f, 0.f, 1.f*u, 1.f*w,//6
      -unit.x,  unit.y, -unit.z, 0.f, 1.f, 0.f, 0.f*u, 1.f*w,//5
      unit.x,  unit.y, unit.z, 0.f, 1.f, 0.f, 1.f*u, 0.f*w,//2
      unit.x,  unit.y, -unit.z, 0.f, 1.f, 0.f, 1.f*u, 1.f*w,//6
      -unit.x,  unit.y, unit.z, 0.f, 1.f, 0.f, 0.f*u, 0.f*w,//1

      //Bottom Face
      -unit.x, -unit.y, -unit.z, 0.f, -1.f, 0.f, 0.f*u, 0.f*w,//4
      unit.x, -unit.y, unit.z, 0.f, -1.f, 0.f, 1.f*u, 1.f*w,//3
      -unit.x, -unit.y, unit.z, 0.f, -1.f, 0.f, 0.f*u, 1.f*w,//0
      unit.x, -unit.y, -unit.z, 0.f, -1.f, 0.f, 1.f*u, 0.f*w,//7
      unit.x, -unit.y, unit.z, 0.f, -1.f, 0.f, 1.f*u, 1.f*w,//3
      -unit.x, -unit.y, -unit.z, 0.f, -1.f, 0.f, 0.f*u, 0.f*w,//4

      //Left Face
      -unit.x, -unit.y, -unit.z, -1.f, 0.f, 0.f, 0.f*w, 0.f*v,//4
      -unit.x,  unit.y, unit.z, -1.f, 0.f, 0.f, 1.f*w, 1.f*v,//1
      -unit.x,  unit.y, -unit.z, -1.f, 0.f, 0.f, 0.f*w, 1.f*v,//5
      -unit.x, -unit.y, unit.z, -1.f, 0.f, 0.f, 1.f*w, 0.f*v,//0
      -unit.x,  unit.y, unit.z, -1.f, 0.f, 0.f, 1.f*w, 1.f*v,//1
      -unit.x, -unit.y, -unit.z, -1.f, 0.f, 0.f, 0.f*w, 0.f*v,//4

      //Right Face
      unit.x, -unit.y, unit.z, 1.f, 0.f, 0.f, 0.f*w, 0.f*v,//3
      unit.x,  unit.y, -unit.z, 1.f, 0.f, 0.f, 1.f*w, 1.f*v,//6
      unit.x,  unit.y, unit.z, 1.f, 0.f, 0.f, 0.f*w, 1.f*v,//2
      unit.x, -unit.y, -unit.z, 1.f, 0.f, 0.f, 1.f*w, 0.f*v,//7
      unit.x,  unit.y, -unit.z, 1.f, 0.f, 0.f, 1.f*w, 1.f*v,//6
      unit.x, -unit.y, unit.z, 1.f, 0.f, 0.f, 0.f*w, 0.f*v//3
    };

    for(int vert = 0; vert < numVerts; vert++)
    {
      int i = vert * numElements;
      _positions.push_back(Vector3(cube[i], cube[i+1], cube[i+2]));
      _normals.push_back(Vector3(cube[i+3], cube[i+4], cube[i+5]));
      _uvs.push_back(Vector2(cube[i+6], cube[i+7]));
      _colors.push_back(Color(1,1,1,1));
    }

    if(bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }

    bufferInfo->bufferType = GL_ARRAY_BUFFER;
    bufferInfo->memTransferBehavior = GL_STATIC_DRAW;
    bufferInfo->primitiveType = GL_TRIANGLES;


    constructBuffer();
  }

 void Mesh::createOpenTopCube(Vector3 size, Vector3 uvScale)
  {
    const int numVerts = 6*5;
    const int numElements = 8;

    Vector3 unit = size*0.5f;
    float u = uvScale.x;
    float v = uvScale.y;
    float w = uvScale.z;

    float cube[numVerts*numElements] = {
      //Front Face
      -unit.x, -unit.y, unit.z, 0.f, 0.f, 1.f, 0.f*u, 0.f*v,//0
      unit.x,  unit.y, unit.z, 0.f, 0.f, 1.f, 1.f*u, 1.f*v,//2
      -unit.x,  unit.y, unit.z, 0.f, 0.f, 1.f, 0.f*u, 1.f*v,//1
      unit.x, -unit.y, unit.z, 0.f, 0.f, 1.f, 1.f*u, 0.f*v,//3
      unit.x,  unit.y, unit.z, 0.f, 0.f, 1.f, 1.f*u, 1.f*v,//2
      -unit.x, -unit.y, unit.z, 0.f, 0.f, 1.f, 0.f*u, 0.f*v,//0

      //Back Face
      unit.x, -unit.y, -unit.z, 0.f, 0.f, -1.f, 0.f*u, 0.f*v,//7
      -unit.x,  unit.y, -unit.z, 0.f, 0.f, -1.f, 1.f*u, 1.f*v,//5
      unit.x,  unit.y, -unit.z, 0.f, 0.f, -1.f, 0.f*u, 1.f*v,//6
      -unit.x, -unit.y, -unit.z, 0.f, 0.f, -1.f, 1.f*u, 0.f*v,//4
      -unit.x,  unit.y, -unit.z, 0.f, 0.f, -1.f, 1.f*u, 1.f*v,//5
      unit.x, -unit.y, -unit.z, 0.f, 0.f, -1.f, 0.f*u, 0.f*v,//7

//      //Top Face
//      -unit.x,  unit.y, unit.z, 0.f, 1.f, 0.f, 0.f*u, 0.f*w,//1
//      unit.x,  unit.y, -unit.z, 0.f, 1.f, 0.f, 1.f*u, 1.f*w,//6
//      -unit.x,  unit.y, -unit.z, 0.f, 1.f, 0.f, 0.f*u, 1.f*w,//5
//      unit.x,  unit.y, unit.z, 0.f, 1.f, 0.f, 1.f*u, 0.f*w,//2
//      unit.x,  unit.y, -unit.z, 0.f, 1.f, 0.f, 1.f*u, 1.f*w,//6
//  setupLayoutAttributes    -unit.x,  unit.y, unit.z, 0.f, 1.f, 0.f, 0.f*u, 0.f*w,//1

      //Bottom Face
      -unit.x, -unit.y, -unit.z, 0.f, -1.f, 0.f, 0.f*u, 0.f*w,//4
      unit.x, -unit.y, unit.z, 0.f, -1.f, 0.f, 1.f*u, 1.f*w,//3
      -unit.x, -unit.y, unit.z, 0.f, -1.f, 0.f, 0.f*u, 1.f*w,//0
      unit.x, -unit.y, -unit.z, 0.f, -1.f, 0.f, 1.f*u, 0.f*w,//7
      unit.x, -unit.y, unit.z, 0.f, -1.f, 0.f, 1.f*u, 1.f*w,//3
      -unit.x, -unit.y, -unit.z, 0.f, -1.f, 0.f, 0.f*u, 0.f*w,//4

      //Left Face
      -unit.x, -unit.y, -unit.z, -1.f, 0.f, 0.f, 0.f*w, 0.f*v,//4
      -unit.x,  unit.y, unit.z, -1.f, 0.f, 0.f, 1.f*w, 1.f*v,//1
      -unit.x,  unit.y, -unit.z, -1.f, 0.f, 0.f, 0.f*w, 1.f*v,//5
      -unit.x, -unit.y, unit.z, -1.f, 0.f, 0.f, 1.f*w, 0.f*v,//0
      -unit.x,  unit.y, unit.z, -1.f, 0.f, 0.f, 1.f*w, 1.f*v,//1
      -unit.x, -unit.y, -unit.z, -1.f, 0.f, 0.f, 0.f*w, 0.f*v,//4

      //Right Face
      unit.x, -unit.y, unit.z, 1.f, 0.f, 0.f, 0.f*w, 0.f*v,//3
      unit.x,  unit.y, -unit.z, 1.f, 0.f, 0.f, 1.f*w, 1.f*v,//6
      unit.x,  unit.y, unit.z, 1.f, 0.f, 0.f, 0.f*w, 1.f*v,//2
      unit.x, -unit.y, -unit.z, 1.f, 0.f, 0.f, 1.f*w, 0.f*v,//7
      unit.x,  unit.y, -unit.z, 1.f, 0.f, 0.f, 1.f*w, 1.f*v,//6
      unit.x, -unit.y, unit.z, 1.f, 0.f, 0.f, 0.f*w, 0.f*v//3
    };
    /*
    Vector3 unit = size*0.5f;

    float cube[numVerts*numElements] = {
    //Front Face
    -unit.x, -unit.y, unit.z, 0.f, 0.f, 1.f, 0.f, 0.f,//0
     unit.x,  unit.y, unit.z, 0.f, 0.f, 1.f, 1.f, 1.f,//2
    -unit.x,  unit.y, unit.z, 0.f, 0.f, 1.f, 0.f, 1.f,//1
     unit.x, -unit.y, unit.z, 0.f, 0.f, 1.f, 1.f, 0.f,//3
     unit.x,  unit.y, unit.z, 0.f, 0.f, 1.f, 1.f, 1.f,//2
    -unit.x, -unit.y, unit.z, 0.f, 0.f, 1.f, 0.f, 0.f,//0

    //Back Face
     unit.x, -unit.y, -unit.z, 0.f, 0.f, -1.f, 0.f, 0.f,//7
    -unit.x,  unit.y, -unit.z, 0.f, 0.f, -1.f, 1.f, 1.f,//5
     unit.x,  unit.y, -unit.z, 0.f, 0.f, -1.f, 0.f, 1.f,//6
    -unit.x, -unit.y, -unit.z, 0.f, 0.f, -1.f, 1.f, 0.f,//4
    -unit.x,  unit.y, -unit.z, 0.f, 0.f, -1.f, 1.f, 1.f,//5
     unit.x, -unit.y, -unit.z, 0.f, 0.f, -1.f, 0.f, 0.f,//7

    //Top Face
//    -unit.x,  unit.y, unit.z, 0.f, 1.f, 0.f, 0.f, 0.f,//1
//     unit.x,  unit.y, -unit.z, 0.f, 1.f, 0.f, 1.f, 1.f,//6
//    -unit.x,  unit.y, -unit.z, 0.f, 1.f, 0.f, 0.f, 1.f,//5
//     unit.x,  unit.y, unit.z, 0.f, 1.f, 0.f, 1.f, 0.f,//2
//     unit.x,  unit.y, -unit.z, 0.f, 1.f, 0.f, 1.f, 1.f,//6
//    -unit.x,  unit.y, unit.z, 0.f, 1.f, 0.f, 0.f, 0.f,//1

    //Bottom Face
    -unit.x, -unit.y, -unit.z, 0.f, -1.f, 0.f, 0.f, 0.f,//4
     unit.x, -unit.y, unit.z, 0.f, -1.f, 0.f, 1.f, 1.f,//3
    -unit.x, -unit.y, unit.z, 0.f, -1.f, 0.f, 0.f, 1.f,//0
     unit.x, -unit.y, -unit.z, 0.f, -1.f, 0.f, 1.f, 0.f,//7
     unit.x, -unit.y, unit.z, 0.f, -1.f, 0.f, 1.f, 1.f,//3
    -unit.x, -unit.y, -unit.z, 0.f, -1.f, 0.f, 0.f, 0.f,//4

    //Left Face
    -unit.x, -unit.y, -unit.z, -1.f, 0.f, 0.f, 0.f, 0.f,//4
    -unit.x,  unit.y, unit.z, -1.f, 0.f, 0.f, 1.f, 1.f,//1
    -unit.x,  unit.y, -unit.z, -1.f, 0.f, 0.f, 0.f, 1.f,//5
    -unit.x, -unit.y, unit.z, -1.f, 0.f, 0.f, 1.f, 0.f,//0
    -unit.x,  unit.y, unit.z, -1.f, 0.f, 0.f, 1.f, 1.f,//1
    -unit.x, -unit.y, -unit.z, -1.f, 0.f, 0.f, 0.f, 0.f,//4

    //Right Face
     unit.x, -unit.y, unit.z, 1.f, 0.f, 0.f, 0.f, 0.f,//3
     unit.x,  unit.y, -unit.z, 1.f, 0.f, 0.f, 1.f, 1.f,//6
     unit.x,  unit.y, unit.z, 1.f, 0.f, 0.f, 0.f, 1.f,//2
     unit.x, -unit.y, -unit.z, 1.f, 0.f, 0.f, 1.f, 0.f,//7
     unit.x,  unit.y, -unit.z, 1.f, 0.f, 0.f, 1.f, 1.f,//6
     unit.x, -unit.y, unit.z, 1.f, 0.f, 0.f, 0.f, 0.f//3

    };
  */

    for(int vert = 0; vert < numVerts; vert++)
    {
      int i = vert * numElements;
      _positions.push_back(Vector3(cube[i], cube[i+1], cube[i+2]));
      _normals.push_back(Vector3(cube[i+3], cube[i+4], cube[i+5]));
      _uvs.push_back(Vector2(cube[i+6], cube[i+7]));
      _colors.push_back(Color(1,1,1,1));
    }

    if(bufferInfo == nullptr)
    {
      bufferInfo = new MeshBufferInfo();
    }

    bufferInfo->bufferType = GL_ARRAY_BUFFER;
    bufferInfo->memTransferBehavior = GL_STATIC_DRAW;
    bufferInfo->primitiveType = GL_TRIANGLES;


    constructBuffer();
  }



/*
  void Mesh::calculateTangentArray(long vertexCount, const Vector3 *vertex,
      const Vector3 *normal, const Vector2 *texcoord, long triangleCount,
      const Triangle *triangle, Vector4 *tangent)
  {
      Vector3 *tan1 = new Vector3[vertexCount * 2];
      Vector3 *tan2 = tan1 + vertexCount;
      memset(tan1, 0,vertexCount * sizeof(Vector3) * 2);

      for (long a = 0; a < triangleCount; a++)
      {
          long i1 = triangle->index[0];
          long i2 = triangle->index[1];
          long i3 = triangle->index[2];

          const Vector3& v1 = vertex[i1];
          const Vector3& v2 = vertex[i2];
          const Vector3& v3 = vertex[i3];

          const Vector2& w1 = texcoord[i1];
          const Vector2& w2 = texcoord[i2];
          const Vector2& w3 = texcoord[i3];

          float x1 = v2.x - v1.x;
          float x2 = v3.x - v1.x;
          float y1 = v2.y - v1.y;
          float y2 = v3.y - v1.y;
          float z1 = v2.z - v1.z;
          float z2 = v3.z - v1.z;

          float s1 = w2.x - w1.x;
          float s2 = w3.x - w1.x;
          float t1 = w2.y - w1.y;
          float t2 = w3.y - w1.y;

          float r = 1.0F / (s1 * t2 - s2 * t1);
          Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                  (t2 * z1 - t1 * z2) * r);
          Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                  (s1 * z2 - s2 * z1) * r);

          tan1[i1] += sdir;
          tan1[i2] += sdir;
          tan1[i3] += sdir;

          tan2[i1] += tdir;
          tan2[i2] += tdir;
          tan2[i3] += tdir;

          triangle++;
      }

      for (long a = 0; a < vertexCount; a++)
      {
          const Vector3& n = normal[a];
          const Vector3& t = tan1[a];

          // Gram-Schmidt orthogonalize
          tangent[a] = normalize((t - n * dot(n, t));

          // Calculate handedness
          tangent[a].w = (dot(cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
      }

      delete[] tan1;
  }
  */
  void Mesh::calculateTangentArrayNonIndexedTriangles()
  {
    long vertexCount = (long) _positions.size();
    long triangleCount = vertexCount/3;

    Vector3 *tan1 = new Vector3[vertexCount * 2];
    Vector3 *tan2 = tan1 + vertexCount;
    memset(tan1, 0,vertexCount * sizeof(Vector3) * 2);

    for (long a = 0; a < triangleCount; a++)
    {
        long i1 = a*3;
        long i2 = a*3 + 1;
        long i3 = a*3 + 2;

        const Vector3& v1 = _positions[i1];
        const Vector3& v2 = _positions[i2];
        const Vector3& v3 = _positions[i3];

        const Vector2& w1 = _uvs[i1];
        const Vector2& w2 = _uvs[i2];
        const Vector2& w3 = _uvs[i3];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.x - w1.x;
        float s2 = w3.x - w1.x;
        float t1 = w2.y - w1.y;
        float t2 = w3.y - w1.y;

        float r = 1.0F / (s1 * t2 - s2 * t1);
        Vector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
                (t2 * z1 - t1 * z2) * r);
        Vector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
                (s1 * z2 - s2 * z1) * r);

        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;

    }

    for (long a = 0; a < vertexCount; a++)
    {
        const Vector3& n = _normals[a];
        const Vector3& t = tan1[a];

        // Gram-Schmidt orthogonalize
        _tangents[a] = normalize(t - n * dot(n, t));

        // Calculate handedness
        _tangents[a].w = (dot(cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
    }

    delete[] tan1;
  }
  void Mesh::movePivotToBottomMiddle()
  {
    vec3 min, max;
    calculateBoundingBox(min, max);
    //translate everything so each dim is greater than 1
    glm::vec3 trans;
    glm::vec3 center = (max + min);
    center = center * glm::vec3(0.5,0.5,0.5);
    
    trans = glm::vec3(0.5,0.5,0.5) - center;
    glm::mat4 xform = glm::translate(glm::vec3(trans.x,-min.y,trans.z));
    
    transformMesh(xform);
    
    //REMOVE LATER: Check that the mesh is transformed correctly
    for(size_t i = 0; i < _positions.size(); ++i)
    {
      bool outOfBounds = false;
      if(_positions[i].x < 0.0f)
        outOfBounds = true;
      if(_positions[i].x > 1.0f)
        outOfBounds = true;
      if(_positions[i].y < 0.0f)
        outOfBounds = true;
      if(_positions[i].y > 1.0f)
        outOfBounds = true;
      if(_positions[i].z < 0.0f)
        outOfBounds = true;
      if(_positions[i].z > 1.0f)
        outOfBounds = true;
      if(outOfBounds)
      {
        printf("Position[%d] %3.4f, %3.4f, %3.4f\n", (int)i, _positions[i].x,
               _positions[i].y, _positions[i].z);
      }
    }
    
  }
  
  void Mesh::fitIntoUnitCube(glm::vec3& trans, glm::vec3& min, glm::vec3& max)
  {
	//This now assumes that the pivot point/origin is at the base of the model and
	//at the center of its rotation.

    //vec3 min, max;
    calculateBoundingBox(min, max);
    //translate everything so each dim is greater than 1
    
    //glm::vec3 trans;
    if( min.x < 0)
      trans.x = -min.x;
    if(min.y < 0)
      trans.y = -min.y;
    if(min.z)
      trans.z = -min.z;
  
    printf("fit trans: %3.6f %3.6f %3.6f\n", trans.x, trans.y, trans.z);

	//trans = glm::vec3(1.0, 1.0, 1.0);
	trans = glm::vec3(0.5, 0.0, 0.5);

    //find the longest side and scale everything so it fits into a dim of one.
    glm::vec3 diff = max - min;
    float maxDim = diff.x;
    if(diff.y > maxDim)
    {
      maxDim = diff.y;
    }
    if(diff.z > maxDim)
    {
      maxDim = diff.z;
    }
  
	glm::mat4 xform = glm::scale(glm::vec3(1.0f / (maxDim*1.1)));
    transformMesh(xform);

    xform = xform * glm::translate(trans);
    transformMesh(xform);
  
    //REMOVE LATER: Check that the mesh is transformed correctly
    for(size_t i = 0; i < _positions.size(); ++i)
    {
      bool outOfBounds = false;
      if(_positions[i].x < 0.0f)
        outOfBounds = true;
      if(_positions[i].x > 1.0f)
        outOfBounds = true;
      if(_positions[i].y < 0.0f)
        outOfBounds = true;
      if(_positions[i].y > 1.0f)
        outOfBounds = true;
      if(_positions[i].z < 0.0f)
        outOfBounds = true;
      if(_positions[i].z > 1.0f)
        outOfBounds = true;
      if(outOfBounds)
      {
        printf("Position[%d] %3.4f, %3.4f, %3.4f\n", (int)i, _positions[i].x,
               _positions[i].y, _positions[i].z);
      }
    }
  
  }
  
  void Mesh::transformMesh(glm::mat4 xform)
  {
    for(size_t i = 0; i < _positions.size(); ++i)
    {
      glm::vec4 p(_positions[i].x,_positions[i].y,_positions[i].z,1.0f);
      p = xform * p;
      Vector3 pp(p.x,p.y,p.z);
      _positions[i] = pp;
    }
  
    constructBuffer();
  }
 
  void Mesh::convertToTriangleMesh(TriangleMesh& triMesh, std::shared_ptr<UniformHGrid> grid)
  {
	  if (_indices.size() > 0)
	  {
		  for (unsigned int i = 0; i < _indices.size() / 3; ++i)
		  {
			  int i0 = _indices[i * 3];
			  int i1 = _indices[i * 3 + 1];
			  int i2 = _indices[i * 3 + 2];
			  //Now go through each triangle
			  vec3 p0(_positions[i0].x, _positions[i0].y, _positions[i0].z);
			  vec3 p1(_positions[i1].x, _positions[i1].y, _positions[i1].z);
			  vec3 p2(_positions[i2].x, _positions[i2].y, _positions[i2].z);

			  vec3 n0(_normals[i0].x, _normals[i0].y, _normals[i0].z);
			  vec3 n1(_normals[i1].x, _normals[i1].y, _normals[i1].z);
			  vec3 n2(_normals[i2].x, _normals[i2].y, _normals[i2].z);

			  triMesh.addTriangle(p0, p1, p2, n0, n1, n2);
		  }
	  }
	  else
	  {
		  for (unsigned int i = 0; i < _positions.size() / 3; ++i)
		  {
			  int i0 = i * 3;
			  int i1 = i * 3 + 1;
			  int i2 = i * 3 + 2;
			  //Now go through each triangle
			  vec3 p0(_positions[i0].x, _positions[i0].y, _positions[i0].z);
			  vec3 p1(_positions[i1].x, _positions[i1].y, _positions[i1].z);
			  vec3 p2(_positions[i2].x, _positions[i2].y, _positions[i2].z);

			  vec3 n0(_normals[i0].x, _normals[i0].y, _normals[i0].z);
			  vec3 n1(_normals[i1].x, _normals[i1].y, _normals[i1].z);
			  vec3 n2(_normals[i2].x, _normals[i2].y, _normals[i2].z);

			  triMesh.addTriangle(p0, p1, p2, n0, n1, n2);

		  }
	  }
    triMesh.setGrid(grid);
    grid->storeTriangleMesh(triMesh);
  }
  
  //Expect the mesh vertices to already be transformed.  This saves a lot of
  //calculation because this function will be called many times
float Mesh::getClosestPoint(
             glm::vec3 p,
             glm::vec3& closestPoint,
             glm::vec3& closestNormal)
{
  int ti0, ti1, ti2;
  ti0 = ti1 = ti2 = 0;
  glm::vec3 tmpClosestPoint,closestPointBarycentric;
  float closestDistanceSqr = 1e4;
  //loop through each triangle and store closest point and triangle (for
  //defferred normal calculation)
  if(_indices.size() > 0)
  {
    for(unsigned int i = 0; i < _indices.size()/3; ++i)
    {
      int i0 = _indices[i*3];
      int i1 = _indices[i*3 + 1];
      int i2 = _indices[i*3 + 2];
      //Now go through each triangle
      vec3 p0(_positions[i0].x, _positions[i0].y, _positions[i0].z);
      vec3 p1(_positions[i1].x, _positions[i1].y, _positions[i1].z);
      vec3 p2(_positions[i2].x, _positions[i2].y, _positions[i2].z);

	  vec3 edge0 = p1 - p0;
	  vec3 edge1 = p2 - p0;

	  //Check for zero area triangle
	  if (glm::dot(edge0, edge0) < 1e-5 || glm::dot(edge1, edge1) < 1e-5)
	  {
		  continue;
	  }
      //glm::vec3 tmpPoint = closestPointOnTriangle(p,p0,p1,p2);
      //glm::vec3 diff = p - tmpPoint;
      //float distSqr = glm::dot(diff,diff);
	  glm::vec3 tmpPoint, tmpBarycentric;
	  float distSqr = distancePointTriangleExact(p, p0, p1, p2, tmpPoint, tmpBarycentric);
      if(distSqr < closestDistanceSqr){
        closestDistanceSqr = distSqr;
        tmpClosestPoint = tmpPoint;
		closestPointBarycentric = tmpBarycentric;
        ti0 = i0; ti1 = i1; ti2 = i2;
      }
    
    }
  }
  else
  {
    for(unsigned int i = 0; i < _positions.size()/3; ++i)
    {
      int i0 = i*3;
      int i1 = i*3 + 1;
      int i2 = i*3 + 2;
      //Now go through each triangle
      vec3 p0(_positions[i0].x, _positions[i0].y, _positions[i0].z);
      vec3 p1(_positions[i1].x, _positions[i1].y, _positions[i1].z);
      vec3 p2(_positions[i2].x, _positions[i2].y, _positions[i2].z);
    
      //glm::vec3 tmpPoint = closestPointOnTriangle(p,p0,p1,p2);
      //glm::vec3 diff = p - tmpPoint;
      //float distSqr = glm::dot(diff,diff);
	  glm::vec3 tmpPoint, tmpBarycentric;
	  float distSqr = distancePointTriangleExact(p, p0, p1, p2, tmpPoint, tmpBarycentric);
      if(distSqr < closestDistanceSqr){
        closestDistanceSqr = distSqr;
        tmpClosestPoint = tmpPoint;
		closestPointBarycentric = tmpBarycentric;
        ti0 = i0; ti1 = i1; ti2 = i2;
      }
    }
  }
  //Retrieve triangle from stored indices... an optimization
  vec3 p0(_positions[ti0].x, _positions[ti0].y, _positions[ti0].z);
  vec3 p1(_positions[ti1].x, _positions[ti1].y, _positions[ti1].z);
  vec3 p2(_positions[ti2].x, _positions[ti2].y, _positions[ti2].z);
  vec3 n0(_normals[ti0].x, _normals[ti0].y, _normals[ti0].z);
  vec3 n1(_normals[ti1].x, _normals[ti1].y, _normals[ti1].z);
  vec3 n2(_normals[ti2].x, _normals[ti2].y, _normals[ti2].z);
  //interpolateNormal(n0, n1,n2, closestPointBarycentric,closestNormal);
  closestNormal = n0;
  closestPoint = tmpClosestPoint;
  //float sign = getSignOfDistanceToPoint(p, p0,p1,p2);
  float dist = sqrtf(closestDistanceSqr);
  return dist;
}
  
} //namespace renderlib

