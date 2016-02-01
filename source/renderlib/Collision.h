#ifndef COLLISION_H
#define COLLISION_H

#include "allmath.h"

namespace renderlib{

  class Plane
  {
    public:
      glm::vec3 n;
      float d;
  };

  class AABB
  {
    public:
      glm::vec3 min;
      glm::vec3 max;
  };

  //Ericson, Christer (2013-05-02). Real-Time Collision Detection (Page 164).
  // Test if AABB b intersects plane p
  static bool TestAABBPlane(AABB const& b, Plane const& p)
  {
    // These two lines not necessary with a (center, extents) AABB representation
    glm::vec3 c = (b.max + b.min) * 0.5f; // Compute AABB center
    glm::vec3 e = b.max - c; // Compute positive extents
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    float r = e[0]*glm::abs(p.n[0]) + e[1]*glm::abs(p.n[1]) + e[2]*glm::abs(p.n[2]);
    // Compute distance of box center from plane
    float s = glm::dot(p.n, c) - p.d;
    // Intersection occurs when distance s falls within [-r,+r] interval
    return glm::abs(s) <= r;
  }

  //Ericson, Christer (2013-05-02). Real-Time Collision Detection (Page 171). 
  static bool TestTriangleAABB(glm::vec3 const& a, glm::vec3 const& b, glm::vec3 const& c, AABB  const& aabb ) {

    float p0, p1, p2, r;
    
    // Compute box center and extents of AABoundingBox (if not already given in that format)
    glm::vec3 center = (aabb.max + aabb.min) *  0.5f;
    glm::vec3 extents = aabb.max - center;

    // Translate triangle as conceptually moving AABB to origin
    glm::vec3 v0 = a - center;
    glm::vec3 v1 = b - center;
    glm::vec3 v2 = c - center;

    // Compute edge vectors for triangle
    glm::vec3 f0 = v1 - v0;
    glm::vec3 f1 = v2 - v1;
    glm::vec3 f2 = v0 - v2;

    // Test axes a00..a22 (category 3)
    glm::vec3 a00( 0, -f0.z, f0.y );
    glm::vec3 a01( 0, -f1.z, f1.y );
    glm::vec3 a02( 0, -f2.z, f2.y );
    glm::vec3 a10( f0.z, 0, -f0.x );
    glm::vec3 a11( f1.z, 0, -f1.x );
    glm::vec3 a12( f2.z, 0, -f2.x );
    glm::vec3 a20( -f0.y, f0.x, 0 );
    glm::vec3 a21( -f1.y, f1.x, 0 );
    glm::vec3 a22( -f2.y, f2.x, 0 );

    // Test axis a00
    p0 = glm::dot(v0, a00);
    p1 = glm::dot(v1, a00);
    p2 = glm::dot(v2, a00);
    r = extents.y * glm::abs( f0.z ) + extents.z * glm::abs( f0.y );

    //if ( glm::max( -glm::max( p0, p1, p2 ), glm::min( p0, p1, p2 ) ) > r ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false; // Axis is a separating axis

    }

    // Test axis a01
    p0 = glm::dot(v0, a01 );
    p1 = glm::dot(v1, a01 );
    p2 = glm::dot(v2, a01 );
    r = extents.y * glm::abs( f1.z ) + extents.z * glm::abs( f1.y );

    //if ( glm::max( -glm::max( p0, p1, p2 ), glm::min( p0, p1, p2 ) ) > r ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false; // Axis is a separating axis

    }

    // Test axis a02
    p0 = glm::dot(v0, a02 );
    p1 = glm::dot(v1, a02 );
    p2 = glm::dot(v2, a02 );
    r = extents.y * glm::abs( f2.z ) + extents.z * glm::abs( f2.y );

    //if ( glm::max( -glm::max( p0, p1, p2 ), glm::min( p0, p1, p2 ) ) > r ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false; // Axis is a separating axis

    }

    // Test axis a10
    p0 = glm::dot(v0, a10 );
    p1 = glm::dot(v1, a10 );
    p2 = glm::dot(v2, a10 );
    r = extents.x * glm::abs( f0.z ) + extents.z * glm::abs( f0.x );
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false; // Axis is a separating axis

    }

    // Test axis a11
    p0 = glm::dot(v0, a11 );
    p1 = glm::dot(v1, a11 );
    p2 = glm::dot(v2, a11 );
    r = extents.x * glm::abs( f1.z ) + extents.z * glm::abs( f1.x );

    //if ( glm::max( -glm::max( p0, p1, p2 ), glm::min( p0, p1, p2 ) ) > r ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false; // Axis is a separating axis

    }

    // Test axis a12
    p0 = glm::dot(v0, a12 );
    p1 = glm::dot(v1, a12 );
    p2 = glm::dot(v2, a12 );
    r = extents.x * glm::abs( f2.z ) + extents.z * glm::abs( f2.x );

    //if ( glm::max( -glm::max( p0, p1, p2 ), glm::min( p0, p1, p2 ) ) > r ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false; // Axis is a separating axis

    }

    // Test axis a20
    p0 = glm::dot(v0, a20 );
    p1 = glm::dot(v1, a20 );
    p2 = glm::dot(v2, a20 );
    r = extents.x * glm::abs( f0.y ) + extents.y * glm::abs( f0.x );

    //if ( glm::max( -glm::max( p0, p1, p2 ), glm::min( p0, p1, p2 ) ) > r ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false; // Axis is a separating axis

    }

    // Test axis a21
    p0 = glm::dot(v0, a21 );
    p1 = glm::dot(v1, a21 );
    p2 = glm::dot(v2, a21 );
    r = extents.x * glm::abs( f1.y ) + extents.y * glm::abs( f1.x );

    //if ( glm::max( -glm::max( p0, p1, p2 ), glm::min( p0, p1, p2 ) ) > r ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false; // Axis is a separating axis

    }

    // Test axis a22
    p0 = glm::dot(v0, a22 );
    p1 = glm::dot(v1, a22 );
    p2 = glm::dot(v2, a22 );
    r = extents.x * glm::abs( f2.y ) + extents.y * glm::abs( f2.x );

    //if ( glm::max( -glm::max( p0, p1, p2 ), glm::min( p0, p1, p2 ) ) > r ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false; // Axis is a separating axis

    }

    // Test the three axes corresponding to the face normals of AABB b (category 1).
    // Exit if...
    // ... [-extents.x, extents.x] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
    //if ( glm::max( v0.x, v1.x, v2.x ) < -extents.x || glm::min( v0.x, v1.x, v2.x ) > extents.x ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false;

    }
    // ... [-extents.y, extents.y] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
    //if ( glm::max( v0.y, v1.y, v2.y ) < -extents.y || glm::min( v0.y, v1.y, v2.y ) > extents.y ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false;

    }
    // ... [-extents.z, extents.z] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
    //if ( glm::max( v0.z, v1.z, v2.z ) < -extents.z || glm::min( v0.z, v1.z, v2.z ) > extents.z ) {
    if ( glm::max( -(glm::max(glm::max( p0, p1), p2)), glm::min(glm::min( p0, p1), p2 ) ) > r ) {

      return false;

    }

    // Test separating axis corresponding to triangle face normal (category 2)
    // Face Normal is -ve as Triangle is clockwise winding (and XNA uses -z for into screen)
    Plane plane;
    plane.n = glm::normalize(glm::cross( f1 , f0 ));
    plane.d = glm::dot(plane.n, a);
    
    return TestAABBPlane( aabb, plane );

  }

  
  static float distancePointTriangleExact(
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
    closestPoint= a + t0 * edge0 + t1 * edge1;
    diff = point - closestPoint;
    float sqrDistance = glm::dot(diff, diff);
    return sqrDistance;
  }
  
  static glm::vec3 closestPointOnTriangle(glm::vec3 const& p,
                                   glm::vec3 const& a,
                                   glm::vec3 const& b,
                                   glm::vec3 const& c)
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
  
  static float getSignOfDistanceToPoint(glm::vec3 const& p,
                                 glm::vec3 const& a,
                                 glm::vec3 const& b,
                                 glm::vec3 const& c)
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
  
}//namespace renderlib

#endif //COLLISION_H