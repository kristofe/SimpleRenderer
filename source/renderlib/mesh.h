#ifndef MESH_H
#define MESH_H
#include "OpenGLHelper.h"
#include "shader.h"
#include <vector>
#include "vertex.h"
#include "renderlib/Texture.h"

namespace renderlib{

  //class Vertex;
  //struct VertexAttributeInfo;

  /*
  //TODO:Templatize this to handle different vertex types!
  struct MeshBufferInfo
  {

    GLint vao;
    GLint vbo;
    GLenum bufferType;
    GLenum memTransferBehavior;
    GLenum primitiveType;

    MeshBufferInfo(): vao(-1), vbo(-1), bufferType(GL_ARRAY_BUFFER),
                                        memTransferBehavior(GL_STATIC_DRAW),
                                        primitiveType(GL_TRIANGLES)
    {
    };

    MeshBufferInfo(GLenum pBufferType,
                   GLenum pMemTranferType,
                   GLenum pPrimitiveType):
                                        vao(-1),
                                        vbo(-1),
                                        bufferType(GL_ARRAY_BUFFER),
                                        memTransferBehavior(GL_STATIC_DRAW),
                                        primitiveType(GL_TRIANGLES)
    {
    };
    bool constructBuffer(Mesh& mesh);
    bool updateBuffer(Mesh& mesh);
    bool enableVertexAttributes(Mesh& mesh, Shader& program);

  };
  */

//TODO:Templatize this to handle different vertex types!
class Mesh
{
  friend MeshBufferInfo;
//Methods
public:
  Mesh();
  Mesh(MeshBufferInfo* bi):bufferInfo(bi){};

  void setBufferInfo(MeshBufferInfo* bi) { bufferInfo = bi; };

  int getVertexListSizeInBytes();
  int getIndexListSizeInBytes();

  void drawBuffers();
  void drawBuffersRange(uint32_t startOffset, uint32_t numVertices);
  void setPrimitiveTypeToPoints();
  void setPrimitiveTypeToTriangles();
  bool constructBuffer();
  bool bindAttributesToVAO(Shader& program);
  bool bindAttributesToVAO();
  void enableVertexAttributes();

  uint32_t  getVertexCount() { return (uint32_t)_verts.size(); };

  void createPoint();
  void createTriangle();
  void createPointGrid(Vector3 size);
  void createScreenQuad(Vector2 min,Vector2 max);
  void createScreenQuadIBO(Vector2 min,Vector2 max);
  void createSphereMeshData(int slices, int stacks);
  void createTriStripGridMeshData(int n, int m, bool tileUVs, float aspect = 1.0f);
  void createCube(Vector3 size = Vector3(1.0,1.0,1.0),
                  Vector3 uvScale = Vector3(1.0, 1.0, 1.0));
  void createOpenTopCube(Vector3 size = Vector3(1.0, 1.0, 1.0),
                         Vector3 uvScale = Vector3(1.0, 1.0, 1.0));
  void calculateTangentArrayNonIndexedTriangles();

  static const std::vector<ShaderAttributeData> getShaderAttributeLocations()
  {
    //This is done this way so if this class gets templated then this will
    //still work with the new type.
    Vertex v;
    return v.getShaderAttributes();
  }

  std::vector<Vector3>& getPositionVector() { return _positions; }
  std::vector<Vector3>& getNormalVector() { return _normals; }
  std::vector<Vector2>& getUVVector() { return _uvs; }
  std::vector<Vector4>& getTangentVector() { return _tangents; }
  std::vector<Color>& getColorVector() { return _colors; }
  std::vector<Vertex>& getVertexVector() { return _verts; }
  std::vector<uint32_t>& getIndexVector() { return _indices; }
  uint32_t getMaterialIndex() { return _materialIndex; }
  void setMaterialIndex(uint32_t idx) { _materialIndex = idx; }

  void clearArrays()
  {
    _positions.clear();
    _normals.clear();
    _uvs.clear();
    _tangents.clear();
    _colors.clear();
    _indices.clear();
  }
  
  void addPosition(Vector3 p)
  {
    _positions.push_back(p);
  }
  void addPosition(vec3 p)
  {
    Vector3 i(p.x,p.y,p.z);
    _positions.push_back(i);
  }
  
  void addNormal(Vector3 p)
  {
    _normals.push_back(p);
  }
  void addNormal(vec3 p)
  {
    Vector3 i(p.x,p.y,p.z);
    _normals.push_back(i);
  }
  
  void addUV(Vector2 p)
  {
    _uvs.push_back(p);
  }
  void addUV(vec2 p)
  {
    Vector2 i(p.x,p.y);
    _uvs.push_back(i);
  }
  
  void addTangent(Vector4 p)
  {
    _tangents.push_back(p);
  }
  void addTangent(vec4 p)
  {
    Vector4 i(p.x,p.y,p.z,p.w);
    _tangents.push_back(i);
  }
  
  void addColor(Color p)
  {
    _colors.push_back(p);
  }
  void addColor(vec4 p)
  {
    Color i(p.x,p.y,p.z,p.w);
    _colors.push_back(i);
  }
  
  void addIndex(uint32_t p)
  {
    _indices.push_back(p);
  }
  
  void calculateBoundingBox(vec3& min, vec3& max);
  void fitIntoUnitCube();
  void transformMesh(glm::mat4 xform);
  inline glm::vec3 closestPointOnTriangle(const glm::vec3& p,
                                           const glm::vec3& a,
                                           const glm::vec3& b,
                                           const glm::vec3& c);
  void interpolateNormal(glm::vec3 n0, glm::vec3 n1, glm::vec3 n2,
                         glm::vec3& interpolatedNormal);
  
  float getSignOfDistanceToPoint(const glm::vec3 p,
                                const glm::vec3 a,
                                const glm::vec3 b,
                                const glm::vec3 c);
  
  float getClosestPoint(glm::vec3 p, glm::vec3& closestPoint,
                              glm::vec3& closestNormal);


private:
  DISALLOW_COPY_AND_ASSIGN(Mesh);
  void getBoundingBox(vec3& min, vec3& max);

//Data Members
protected:

  std::vector<Vector3> _positions;
  std::vector<Vector3> _normals;
  std::vector<Vector2> _uvs;
  std::vector<Vector4> _tangents;
  std::vector<Color> _colors;
  std::vector<Vertex>  _verts;
  std::vector<uint32_t> _indices;

  //std::vector<VertexAttributeInfo> _attributes;

  MeshBufferInfo* bufferInfo;
  uint32_t _maxVertexCount{ 0 };

  uint32_t  _vertexCount;
  bool _bufferConstructed{ false };

  vec3 _min;
  vec3 _max;
  uint32_t _materialIndex;

};

  inline void Mesh::interpolateNormal(glm::vec3 n0, glm::vec3 n1, glm::vec3 n2,
                                glm::vec3& interpolatedNormal)
  {
    //For now I am not interpolating... this will need consideration
    interpolatedNormal = n0;
  }

  inline glm::vec3 Mesh::closestPointOnTriangle(const glm::vec3& p,
                                                const glm::vec3& a,
                                                const glm::vec3& b,
                                                const glm::vec3& c)
  {
    // Check if P in vertex region outside A
    glm::vec3 ab = b -a;
    glm::vec3 ac = c -a;
    glm::vec3 ap = p -a;
  
    float d1 = glm::dot(ab, ap);
    float d2 = glm::dot(ac, ap);
  
    if (d1 <= 0.0f && d2 <= 0.0f) return a; // barycentric coordinates (1,0,0)
  
    // Check if P in vertex region outside B
    glm::vec3 bp = p -b;
    float d3 = glm::dot(ab, bp);
    float d4 = glm::dot(ac, bp);
  
    if (d3 >= 0.0f && d4 <= d3) return b; // barycentric coordinates (0,1,0)
  
    // Check if P in edge region of AB, if so return projection of P onto AB
    float vc = d1*d4 -d3*d2;
    if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f)
      {
        float v = d1 / (d1 -d3);
        return a + v * ab; // barycentric coordinates (1-v,v,0)
      }
  
    // Check if P in vertex region outside C
    glm::vec3 cp = p -c;
    float d5 = glm::dot(ab, cp);
    float d6 = glm::dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return c; // barycentric coordinates (0,0,1)
  
    // Check if P in edge region of AC, if so return projection of P onto AC
    float vb = d5*d2 -d1*d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f)
      {
        float w = d2 / (d2 -d6);
        return a + w * ac; // barycentric coordinates (1-w,0,w)
      }
  
    // Check if P in edge region of BC, if so return projection of P onto BC
    float va = d3*d6 -d5*d4;
    if (va <= 0.0f && (d4 -d3) >= 0.0f && (d5 -d6) >= 0.0f)
      {
        float w = (d4 -d3) / ((d4 -d3) + (d5 -d6));
        return b + w * (c -b); // barycentric coordinates (0,1-w,w)
      }
  
    // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom; 
    float w = vc * denom; 
    return a + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f -v -w 
  }
  
  inline float Mesh::getSignOfDistanceToPoint(const glm::vec3 p,
                                const glm::vec3 a,
                                const glm::vec3 b,
                                const glm::vec3 c)
  {
    //TODO: Don't know if this orients the plane/triangle properly
    //I can assume that the triangle's vertices are in CCW order... but i am not
    //using that fact yet
    glm::vec3 normal = glm::cross(c-a,b-a);
    normal = glm::normalize(normal);
  
    //Using average of all vertices to set plane equations point
    //Can I just use a vertex?
    glm::vec3 avg = a+b+c;
    float len = glm::length(avg);
    if(len > 0.0f)
    {
      avg = avg / len;
    }
    //because normal is normalized I can use the formula below.
    float sign = glm::dot(p-avg, normal);
    if(sign < -0.0000001f) return -1.0f;
    return 1.0f;
  }
  
  //Expect the mesh vertices to already be transformed.  This saves a lot of
  //calculation because this function will be called many times
inline float Mesh::getClosestPoint(
             glm::vec3 p,
             glm::vec3& closestPoint,
             glm::vec3& closestNormal)
{
  int ti0, ti1, ti2;
  float closestDistanceSqr = 1e9;
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
    
      glm::vec3 tmpPoint = closestPointOnTriangle(p,p0,p1,p2);
      glm::vec3 diff = p - tmpPoint;
      float distSqr = glm::dot(diff,diff);
      if(distSqr < closestDistanceSqr){
        closestDistanceSqr = distSqr;
        closestPoint = tmpPoint;
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
    
      glm::vec3 tmpPoint = closestPointOnTriangle(p,p0,p1,p2);
      glm::vec3 diff = p - tmpPoint;
      float distSqr = glm::dot(diff,diff);
      if(distSqr < closestDistanceSqr){
        closestDistanceSqr = distSqr;
        closestPoint = tmpPoint;
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
  interpolateNormal(n0, n1,n2, closestNormal);
  float sign = getSignOfDistanceToPoint(p, p0,p1,p2);
  return sqrtf(closestDistanceSqr)*sign;
}


}

#endif // MESH_H
