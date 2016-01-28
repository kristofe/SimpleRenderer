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
	  UniformGrid& grid,
	  glm::vec3 p,
	  glm::vec3& closestPoint,
	  glm::vec3& closestNormal)
  {
	  int ti0, ti1, ti2;
	  ti0 = ti1 = ti2 = 0;
	  glm::vec3 tmpClosestPoint, closestPointBarycentric;
	  float closestDistanceSqr = 1e4;

	  //FIXME: Just making distance to closest point no closer than the next cell which is wrong 
	  //if there are multiple empty cells.
	  float closestDistanceSqr = grid.getCellRadius();
	  closestDistanceSqr *= closestDistanceSqr;
	  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  //FIXME: If there is no triangle in the grid cell then there won't be any test and
	  //it won't find the closest triangle that is in another cell.
	  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	  std::vector<uint32_t>& triIndices = grid.getTrianglesNearPosition(p);
	  for(uint32_t idx :triIndices)
	  {
		  //Now go through each triangle
		  TriangleMeshTriangle& tri = _triangles[idx];

		  /*
		  glm::vec3 tmpPoint = closestPointOnTriangle(p,p0,p1,p2);
		  glm::vec3 diff = p - tmpPoint;
		  float distSqr = glm::dot(diff,diff);
		  */
		  glm::vec3 tmpPoint, tmpBarycentric;
		  float distSqr = distancePointTriangleExact(p, tri.p0, tri.p1, tri.p2, tmpPoint, tmpBarycentric);
		  if (distSqr < closestDistanceSqr) {
			  closestDistanceSqr = distSqr;
			  tmpClosestPoint = tmpPoint;
			  closestPointBarycentric = tmpBarycentric;
		  }
	  }
	  //Retrieve triangle from stored indices... an optimization
	  vec3 p0(_positions[ti0].x, _positions[ti0].y, _positions[ti0].z);
	  vec3 p1(_positions[ti1].x, _positions[ti1].y, _positions[ti1].z);
	  vec3 p2(_positions[ti2].x, _positions[ti2].y, _positions[ti2].z);

	  closestNormal = glm::vec3(1,0,0);
	  closestPoint = tmpClosestPoint;
	  //float sign = getSignOfDistanceToPoint(p, p0,p1,p2);
	  float dist = sqrtf(closestDistanceSqr);
	  return dist;
  }
  
} //namespace renderlib

