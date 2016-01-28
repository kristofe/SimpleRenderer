#ifndef TRIANGLE_MESH_H
#define TRIANGLE_MESH_H
#include "OpenGLHelper.h"
#include "shader.h"
#include <vector>
#include "vertex.h"
#include "renderlib/Texture.h"
#include "UniformGrid.h"

namespace renderlib {

	class TriangleMeshTriangle 
	{ 
	  public: 
		  glm::vec3 p0; 
		  glm::vec3 p1; 
		  glm::vec3 p2; 
		  glm::vec3 min; 
		  glm::vec3 max; 
	};

	class TriangleMesh
	{
	public:
		//Methods
	public:
		TriangleMesh();
		uint32_t  getTriangleCount() { return (uint32_t)_triangles.size(); };
		std::vector<TriangleMeshTriangle>& getTriangleVector() { return _triangles; }

		void clearArrays()
		{
			_triangles.clear();
		}

		void addTriangle(glm::vec3 p0,glm::vec3 p1,glm::vec3 p2)
		{
			TriangleMeshTriangle t;
			t.p0 = p0;
			t.p1 = p1;
			t.p2 = p2;

			t.min.x = t.min.y = t.min.z = 99999999.0f;
			t.max.x = t.max.y = t.max.z = -99999999.0f;
			std::vector<glm::vec3> verts{ t.p0, t.p1, t.p2 };
			for (glm::vec3 v : verts)
			{
				if (v.x < t.min.x)
					t.min.x = v.x;
				if (v.y < t.min.y)
					t.min.y = v.y;
				if (v.z < t.min.z)
					t.min.z = v.z;

				if (v.x > t.max.x)
					t.max.x = v.x;
				if (v.y > t.max.y)
					t.max.y = v.y;
				if (v.z > t.max.z)
					t.max.z = v.z;
			}
			_triangles.push_back(t);
		}

    void calculateBoundingBox(vec3& min, vec3& max);

	float getClosestPoint(UniformGrid& grid, glm::vec3 p, glm::vec3& closestPoint,
		glm::vec3& closestNormal);
	//TODO: These are copies from mesh.h
	//TODO: Move the following into a GEOMETRY class as static members.  
    glm::vec3 closestPointOnTriangle(const glm::vec3& p,
                                           const glm::vec3& a,
                                           const glm::vec3& b,
                                           const glm::vec3& c);

	void interpolateNormal(glm::vec3 const& n0, glm::vec3 const& n1, glm::vec3 const& n2,
		glm::vec3 const& barycentricCoord, glm::vec3& interpolatedNormal);

	float getSignOfDistanceToPoint(const glm::vec3 p,
		const glm::vec3 a,
		const glm::vec3 b,
		const glm::vec3 c);

	float distancePointTriangleExact(
		glm::vec3 const& point, glm::vec3 const& a, glm::vec3 const& b, glm::vec3 const& c,
		glm::vec3& closestPoint, glm::vec3& barycentricCoords);


	private:
		DISALLOW_COPY_AND_ASSIGN(TriangleMesh);

	protected:
		std::vector<TriangleMeshTriangle> _triangles;
		glm::vec3 _min;
		glm::vec3 _max;

	};
  
  inline void TriangleMesh::interpolateNormal(glm::vec3 const& n0, glm::vec3 const& n1, glm::vec3 const& n2,
                                glm::vec3 const& barycentricCoord,
                                glm::vec3& interpolatedNormal)
  {
    //For now I am not interpolating... this will need consideration
    interpolatedNormal = n0;
  }
  
  inline glm::vec3 TriangleMesh::closestPointOnTriangle(const glm::vec3& p,
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
  
  inline float TriangleMesh::getSignOfDistanceToPoint(const glm::vec3 p,
                                const glm::vec3 a,
                                const glm::vec3 b,
                                const glm::vec3 c)
  {
	//FIXME: BROKEN SOMEHOW weird distance or sign flips possible?
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
  


inline float TriangleMesh::distancePointTriangleExact(
	glm::vec3 const& point, glm::vec3 const& a, glm::vec3 const& b, glm::vec3 const& c,
	glm::vec3& closestPoint, glm::vec3& barycentricCoords)
{
	glm::vec3 diff = point - a;
	glm::vec3 edge0 = b - a;
	glm::vec3 edge1 = c - a;
	float a00 = glm::dot(edge0, edge0);
	float a01 = glm::dot(edge0, edge1);
	float a11 = glm::dot(edge1, edge1);
	float b0 = -glm::dot(diff, edge0);
	float b1 = -glm::dot(diff, edge1);
	float const zero = (float)0;
	float const one = (float)1;
	float det = a00 * a11 - a01 * a01;
	float t0 = a01 * b1 - a11 * b0;
	float t1 = a01 * b0 - a00 * b1;

	if (t0 + t1 <= det)
	{
		if (t0 < zero)
		{
			if (t1 < zero)  // region 4
			{
				if (b0 < zero)
				{
					t1 = zero;
					if (-b0 >= a00)  // V0
					{
						t0 = one;
					}
					else  // E01
					{
						t0 = -b0 / a00;
					}
				}
				else
				{
					t0 = zero;
					if (b1 >= zero)  // V0
					{
						t1 = zero;
					}
					else if (-b1 >= a11)  // V2
					{
						t1 = one;
					}
					else  // E20
					{
						t1 = -b1 / a11;
					}
				}
			}
			else  // region 3
			{
				t0 = zero;
				if (b1 >= zero)  // V0
				{
					t1 = zero;
				}
				else if (-b1 >= a11)  // V2
				{
					t1 = one;
				}
				else  // E20
				{
					t1 = -b1 / a11;
				}
			}
		}
		else if (t1 < zero)  // region 5
		{
			t1 = zero;
			if (b0 >= zero)  // V0
			{
				t0 = zero;
			}
			else if (-b0 >= a00)  // V1
			{
				t0 = one;
			}
			else  // E01
			{
				t0 = -b0 / a00;
			}
		}
		else  // region 0, interior
		{
			float invDet = one / det;
			t0 *= invDet;
			t1 *= invDet;
		}
	}
	else
	{
		float tmp0, tmp1, numer, denom;

		if (t0 < zero)  // region 2
		{
			tmp0 = a01 + b0;
			tmp1 = a11 + b1;
			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a00 - ((float)2)*a01 + a11;
				if (numer >= denom)  // V1
				{
					t0 = one;
					t1 = zero;
				}
				else  // E12
				{
					t0 = numer / denom;
					t1 = one - t0;
				}
			}
			else
			{
				t0 = zero;
				if (tmp1 <= zero)  // V2
				{
					t1 = one;
				}
				else if (b1 >= zero)  // V0
				{
					t1 = zero;
				}
				else  // E20
				{
					t1 = -b1 / a11;
				}
			}
		}
		else if (t1 < zero)  // region 6
		{
			tmp0 = a01 + b1;
			tmp1 = a00 + b0;
			if (tmp1 > tmp0)
			{
				numer = tmp1 - tmp0;
				denom = a00 - ((float)2)*a01 + a11;
				if (numer >= denom)  // V2
				{
					t1 = one;
					t0 = zero;
				}
				else  // E12
				{
					t1 = numer / denom;
					t0 = one - t1;
				}
			}
			else
			{
				t1 = zero;
				if (tmp1 <= zero)  // V1
				{
					t0 = one;
				}
				else if (b0 >= zero)  // V0
				{
					t0 = zero;
				}
				else  // E01
				{
					t0 = -b0 / a00;
				}
			}
		}
		else  // region 1
		{
			numer = a11 + b1 - a01 - b0;
			if (numer <= zero)  // V2
			{
				t0 = zero;
				t1 = one;
			}
			else
			{
				denom = a00 - ((float)2)*a01 + a11;
				if (numer >= denom)  // V1
				{
					t0 = one;
					t1 = zero;
				}
				else  // 12
				{
					t0 = numer / denom;
					t1 = one - t0;
				}
			}
		}
	}

	barycentricCoords.x = one - t0 - t1;
	barycentricCoords.y = t0;
	barycentricCoords.z = t1;
	closestPoint = a + t0 * edge0 + t1 * edge1;
	diff = point - closestPoint;
	float sqrDistance = glm::dot(diff, diff);
	return sqrDistance;
}

}
#endif // MESH_H
