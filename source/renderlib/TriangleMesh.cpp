#include <cstring>
#include <limits>
#include "OpenGLHelper.h"
#include "vmath.hpp"
#include "TriangleMesh.h"
#include "vertex.h"
#include "Collision.h"
#include "UniformHGrid.h"

namespace renderlib
{
  ///////////////////////////////////////////////////////////////////////////////
  // TriangleMesh Class Methods
  ///////////////////////////////////////////////////////////////////////////////
  TriangleMesh::TriangleMesh() :_grid(nullptr)
  {
  }


  void TriangleMesh::calculateBoundingBox(vec3& min, vec3& max)
  {
    _min.x = _min.y = _min.z = 99999999.0f;
    _max.x = _max.y = _max.z = -99999999.0f;
    for(TriangleMeshTriangle& t : _triangles)
    {
  		std::vector<glm::vec3> verts{ t.p0, t.p1, t.p2 };
      for (glm::vec3 v : verts)
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
    }
    min = _min;
    max = _max;
  }

  //Expect the mesh vertices to already be transformed.  This saves a lot of
  //calculation because this function will be called many times
  float TriangleMesh::getClosestPoint(
	  glm::vec3 p,
	  glm::vec3& closestPoint,
	  glm::vec3& closestNormal) const
  {
    assert(_grid != nullptr);
    
	  int ti;
	  glm::vec3 tmpClosestPoint, closestPointBarycentric;

    float closestDistanceSqr = std::numeric_limits<float>::max();  //rid.getCellRadius();
	  std::vector<uint32_t> triIndices = _grid->getTrianglesNearPosition(p);
	  //for(uint32_t idx :triIndices)
	  for(uint32_t idx : _indices)
	  {
		  //Now go through each triangle
		  const TriangleMeshTriangle& tri = _triangles[idx];

		 
		  glm::vec3 tmpPoint, tmpBarycentric;
      /*
		  tmpPoint = closestPointOnTriangle(p,tri.p0,tri.p1,tri.p2);
		  glm::vec3 diff = p - tmpPoint;
		  float distSqr = glm::dot(diff,diff);
       */
		
		  float distSqr = distancePointTriangleExact(p, tri.p0, tri.p1, tri.p2, tmpPoint, tmpBarycentric);
		  if (distSqr < closestDistanceSqr) {
			  closestDistanceSqr = distSqr;
			  tmpClosestPoint = tmpPoint;
			  closestPointBarycentric = tmpBarycentric;
			  ti = idx;
		  }
	  }
	  //Retrieve triangle from stored indices... an optimization
	  const TriangleMeshTriangle& tri = _triangles[ti];
	  //glm::vec3 nw = closestPointBarycentric;

	  closestNormal =  interpolateNormals(closestPointBarycentric, tri);// glm::normalize(nw.x*tri.n0 + nw.y*tri.n1 + nw.z*tri.n2);
	  closestPoint = tmpClosestPoint;
  //float sign = getSignOfDistanceToPoint(p, tri.p0,tri.p1,tri.p2);
  
    float dist = sqrtf(closestDistanceSqr);
	  return dist;
  }
  
  glm::vec3 TriangleMesh::interpolateNormals(
	  const glm::vec3& barycentricCoord,
	  const TriangleMeshTriangle& tri
	  ) const 

  {

	  assert(length(tri.n0) > 0.0 && length(tri.n0) <= 1.0f);
	  assert(length(tri.n1) > 0.0 && length(tri.n1) <= 1.0f);
	  assert(length(tri.n2) > 0.0 && length(tri.n2) <= 1.0f);

	  glm::vec3 closestNormal;

	  closestNormal = glm::normalize(
		  barycentricCoord.x*tri.n0 + barycentricCoord.y*tri.n1 + barycentricCoord.z*tri.n2);

	  /*
	  if (glm::length(closestNormal) <= 0.0001)
	  {
		  glm::vec3 e0 = tri.p1 - tri.p0;
		  glm::vec3 e1 = tri.p2 - tri.p0;
		  closestNormal = glm::normalize(glm::cross(e1, e0));
	  }
	  */

	  assert(length(closestNormal) > 0.0 && length(closestNormal) <= 1.0f);
	  return closestNormal;
  }
  
  
} //namespace renderlib

