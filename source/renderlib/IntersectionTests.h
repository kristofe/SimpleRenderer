//Ericson, Christer (2013-05-02). Real-Time Collision Detection (Page 171). 

int TestTriangleAABB(Point v0, Point v1, Point v2, AABB b)
{
  float p0, p1, p2, r;
  // Compute box center and extents (if not already given in that format)
  Vector c = (b.min + b.max) * 0.5f;
  float e0 = (b.max.x - b.min.x) * 0.5f;
  float e1 = (b.max.y - b.min.y) * 0.5f;
  float e2 = (b.max.z - b.min.z) * 0.5f;
  // Translate triangle as conceptually moving AABB to origin
  v0 = v0 - c;
  v1 = v1 - c;
  v2 = v2 - c;
  // Compute edge vectors for triangle
  Vector f0 = v1 - v0, f1 = v2 - v1, f2 = v0 - v2;

  // Test axes a00..a22 (category 3)
  // Test axis a00
  p0 = v0.z*v1.y - v0.y*v1.z;
  p2 = v2.z*(v1.y - v0.y) - v2.y*(v1.z - v0.z);
  r = e1 * Abs(f0.z) + e2 * Abs(f0.y);
  if (Max(-Max(p0, p2), Min(p0, p2)) > r) return 0; // Axis is a separating axis
  // Repeat similar tests for remaining axes a01..a22
  ...
    

    // Test the three axes corresponding to the face normals of AABB b (category 1).
    // Exit if...
    // ... [-e0, e0] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
    if (Max(v0.x, v1.x, v2.x) < -e0 || Min(v0.x, v1.x, v2.x) > e0) return 0;
  // ... [-e1, e1] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
  if (Max(v0.y, v1.y, v2.y) < -e1 || Min(v0.y, v1.y, v2.y) > e1) return 0;
  // ... [-e2, e2] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
  if (Max(v0.z, v1.z, v2.z) < -e2 || Min(v0.z, v1.z, v2.z) > e2) return 0;
  // Test separating axis corresponding to triangle face normal (category 2)
  Plane p;
  p.n = Cross(f0, f1);
  p.d = Dot(p.n, v0);
  return TestAABBPlane(b, p);
}


//Ericson, Christer (2013-05-02). Real-Time Collision Detection (Page 164).
// Test if AABB b intersects plane p
int TestAABBPlane(AABB b, Plane p)
{
  // These two lines not necessary with a (center, extents) AABB representation
  Point c = (b.max + b.min) * 0.5f; // Compute AABB center
  Point e = b.max - c; // Compute positive extents
  // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
  float r = e[0]*Abs(p.n[0]) + e[1]*Abs(p.n[1]) + e[2]*Abs(p.n[2]);
  // Compute distance of box center from plane
  float s = Dot(p.n, c) - p.d;
  // Intersection occurs when distance s falls within [-r,+r] interval
  return Abs(s) <= r;
}

// based on http://www.gamedev.net/topic/534655-aabb-triangleplane-intersection--distance-to-plane-is-incorrect-i-have-solved-it/
//
// a: <THREE.Vector3>, // vertex of a triangle
// b: <THREE.Vector3>, // vertex of a triangle
// c: <THREE.Vector3>, // vertex of a triangle
// aabb: <THREE.Box3>
collision.isIntersectionTriangleAABB = function ( a, b, c, aabb ) {

  var p0, p1, p2, r;
  
  // Compute box center and extents of AABoundingBox (if not already given in that format)
  var center = new THREE.Vector3().addVectors( aabb.max, aabb.min ).multiplyScalar( 0.5 ),
      extents = new THREE.Vector3().subVectors( aabb.max, center );

  // Translate triangle as conceptually moving AABB to origin
  var v0 = new THREE.Vector3().subVectors( a, center ),
      v1 = new THREE.Vector3().subVectors( b, center ),
      v2 = new THREE.Vector3().subVectors( c, center );

  // Compute edge vectors for triangle
  var f0 = new THREE.Vector3().subVectors( v1, v0 ),
      f1 = new THREE.Vector3().subVectors( v2, v1 ),
      f2 = new THREE.Vector3().subVectors( v0, v2 );

  // Test axes a00..a22 (category 3)
  var a00 = new THREE.Vector3( 0, -f0.z, f0.y ),
      a01 = new THREE.Vector3( 0, -f1.z, f1.y ),
      a02 = new THREE.Vector3( 0, -f2.z, f2.y ),
      a10 = new THREE.Vector3( f0.z, 0, -f0.x ),
      a11 = new THREE.Vector3( f1.z, 0, -f1.x ),
      a12 = new THREE.Vector3( f2.z, 0, -f2.x ),
      a20 = new THREE.Vector3( -f0.y, f0.x, 0 ),
      a21 = new THREE.Vector3( -f1.y, f1.x, 0 ),
      a22 = new THREE.Vector3( -f2.y, f2.x, 0 );

  // Test axis a00
  p0 = v0.dot( a00 );
  p1 = v1.dot( a00 );
  p2 = v2.dot( a00 );
  r = extents.y * Math.abs( f0.z ) + extents.z * Math.abs( f0.y );

  if ( Math.max( -Math.max( p0, p1, p2 ), Math.min( p0, p1, p2 ) ) > r ) {

    return false; // Axis is a separating axis

  }

  // Test axis a01
  p0 = v0.dot( a01 );
  p1 = v1.dot( a01 );
  p2 = v2.dot( a01 );
  r = extents.y * Math.abs( f1.z ) + extents.z * Math.abs( f1.y );

  if ( Math.max( -Math.max( p0, p1, p2 ), Math.min( p0, p1, p2 ) ) > r ) {

    return false; // Axis is a separating axis

  }

  // Test axis a02
  p0 = v0.dot( a02 );
  p1 = v1.dot( a02 );
  p2 = v2.dot( a02 );
  r = extents.y * Math.abs( f2.z ) + extents.z * Math.abs( f2.y );

  if ( Math.max( -Math.max( p0, p1, p2 ), Math.min( p0, p1, p2 ) ) > r ) {

    return false; // Axis is a separating axis

  }

  // Test axis a10
  p0 = v0.dot( a10 );
  p1 = v1.dot( a10 );
  p2 = v2.dot( a10 );
  r = extents.x * Math.abs( f0.z ) + extents.z * Math.abs( f0.x );
  if ( Math.max( -Math.max( p0, p1, p2 ), Math.min( p0, p1, p2 ) ) > r ) {

    return false; // Axis is a separating axis

  }

  // Test axis a11
  p0 = v0.dot( a11 );
  p1 = v1.dot( a11 );
  p2 = v2.dot( a11 );
  r = extents.x * Math.abs( f1.z ) + extents.z * Math.abs( f1.x );

  if ( Math.max( -Math.max( p0, p1, p2 ), Math.min( p0, p1, p2 ) ) > r ) {

    return false; // Axis is a separating axis

  }

  // Test axis a12
  p0 = v0.dot( a12 );
  p1 = v1.dot( a12 );
  p2 = v2.dot( a12 );
  r = extents.x * Math.abs( f2.z ) + extents.z * Math.abs( f2.x );

  if ( Math.max( -Math.max( p0, p1, p2 ), Math.min( p0, p1, p2 ) ) > r ) {

    return false; // Axis is a separating axis

  }

  // Test axis a20
  p0 = v0.dot( a20 );
  p1 = v1.dot( a20 );
  p2 = v2.dot( a20 );
  r = extents.x * Math.abs( f0.y ) + extents.y * Math.abs( f0.x );

  if ( Math.max( -Math.max( p0, p1, p2 ), Math.min( p0, p1, p2 ) ) > r ) {

    return false; // Axis is a separating axis

  }

  // Test axis a21
  p0 = v0.dot( a21 );
  p1 = v1.dot( a21 );
  p2 = v2.dot( a21 );
  r = extents.x * Math.abs( f1.y ) + extents.y * Math.abs( f1.x );

  if ( Math.max( -Math.max( p0, p1, p2 ), Math.min( p0, p1, p2 ) ) > r ) {

    return false; // Axis is a separating axis

  }

  // Test axis a22
  p0 = v0.dot( a22 );
  p1 = v1.dot( a22 );
  p2 = v2.dot( a22 );
  r = extents.x * Math.abs( f2.y ) + extents.y * Math.abs( f2.x );

  if ( Math.max( -Math.max( p0, p1, p2 ), Math.min( p0, p1, p2 ) ) > r ) {

    return false; // Axis is a separating axis

  }

  // Test the three axes corresponding to the face normals of AABB b (category 1).
  // Exit if...
  // ... [-extents.x, extents.x] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
  if ( Math.max( v0.x, v1.x, v2.x ) < -extents.x || Math.min( v0.x, v1.x, v2.x ) > extents.x ) {

    return false;

  }
  // ... [-extents.y, extents.y] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
  if ( Math.max( v0.y, v1.y, v2.y ) < -extents.y || Math.min( v0.y, v1.y, v2.y ) > extents.y ) {

    return false;

  }
  // ... [-extents.z, extents.z] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
  if ( Math.max( v0.z, v1.z, v2.z ) < -extents.z || Math.min( v0.z, v1.z, v2.z ) > extents.z ) {

    return false;

  }

  // Test separating axis corresponding to triangle face normal (category 2)
  // Face Normal is -ve as Triangle is clockwise winding (and XNA uses -z for into screen)
  var plane = new THREE.Plane();
  plane.normal = new THREE.Vector3().copy( f1 ).cross( f0 ).normalize();
  plane.constant = plane.normal.dot( a );
  
  return collision.isIntersectionAABBPlane( aabb, plane );

}
