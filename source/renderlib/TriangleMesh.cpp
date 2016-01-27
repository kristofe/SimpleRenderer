#include <cstring>
#include "OpenGLHelper.h"
#include "vmath.hpp"
#include "TriangleMesh.h"
#include "vertex.h"

namespace renderlib
{
  ///////////////////////////////////////////////////////////////////////////////
  // TriangleMesh Class Methods
  ///////////////////////////////////////////////////////////////////////////////
  TriangleMesh::TriangleMesh() 
  {
  }

  void TriangleMesh::getBoundingBox(vec3& min, vec3& max)
  {
    min.x = _min.x;
    min.y = _min.y;
    min.z = _min.z;
    max.x = _max.x;
    max.y = _max.y;
    max.z = _max.z;
  }

  void TriangleMesh::calculateBoundingBox(vec3& min, vec3& max)
  {
    _min.x = _min.y = _min.z = 99999999.0f;
    _max.x = _max.y = _max.z = -99999999.0f;
    for (glm::vec3 v : _positions)
    {
      if (v.x < _min.x)
        _min.x = v.x;
      if (v.y < _min.y)
        _min.y = v.y;
      if (v.z < _min.z)
        _min.z = v.z;

      if (v.x > _max.x)
        _max.x = v.x;
      if (v.y > _max.y)
        _max.y = v.y;
      if (v.z > _max.z)
        _max.z = v.z;
    }

    getBoundingBox(min, max);
  }


  
} //namespace renderlib

