#version 150
out vec4 outColor;
in vec2 vUV;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform float uTime;
uniform vec3 uResoltion;
uniform vec4 uMouse;

///////////////////////////////////////////////////////////////////////////////

//PERLIN NOISE - GIVEN BY KEN PERLIN
vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 mod289(vec4 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec4 permute(vec4 x) { return mod289(((x*34.0)+1.0)*x); }
vec4 taylorInvSqrt(vec4 r) { return 1.79284291400159 - 0.85373472095314 * r; }
vec3 fade(vec3 t) { return t*t*t*(t*(t*6.0-15.0)+10.0); }
float noise(vec3 P) {
  vec3 i0 = mod289(floor(P)), i1 = mod289(i0 + vec3(1.0));
  vec3 f0 = fract(P), f1 = f0 - vec3(1.0), f = fade(f0);
  vec4 ix = vec4(i0.x, i1.x, i0.x, i1.x), iy = vec4(i0.yy, i1.yy);
  vec4 iz0 = i0.zzzz, iz1 = i1.zzzz;
  vec4 ixy = permute(permute(ix) + iy), ixy0 = permute(ixy + iz0), ixy1 = permute(ixy + iz1);
  vec4 gx0 = ixy0 * (1.0 / 7.0), gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
  vec4 gx1 = ixy1 * (1.0 / 7.0), gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
  gx0 = fract(gx0); gx1 = fract(gx1);
  vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0), sz0 = step(gz0, vec4(0.0));
  vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1), sz1 = step(gz1, vec4(0.0));
  gx0 -= sz0 * (step(0.0, gx0) - 0.5); gy0 -= sz0 * (step(0.0, gy0) - 0.5);
  gx1 -= sz1 * (step(0.0, gx1) - 0.5); gy1 -= sz1 * (step(0.0, gy1) - 0.5);
  vec3 g0 = vec3(gx0.x,gy0.x,gz0.x), g1 = vec3(gx0.y,gy0.y,gz0.y),
       g2 = vec3(gx0.z,gy0.z,gz0.z), g3 = vec3(gx0.w,gy0.w,gz0.w),
       g4 = vec3(gx1.x,gy1.x,gz1.x), g5 = vec3(gx1.y,gy1.y,gz1.y),
       g6 = vec3(gx1.z,gy1.z,gz1.z), g7 = vec3(gx1.w,gy1.w,gz1.w);
  vec4 norm0 = taylorInvSqrt(vec4(dot(g0,g0), dot(g2,g2), dot(g1,g1), dot(g3,g3)));
  vec4 norm1 = taylorInvSqrt(vec4(dot(g4,g4), dot(g6,g6), dot(g5,g5), dot(g7,g7)));
  g0 *= norm0.x; g2 *= norm0.y; g1 *= norm0.z; g3 *= norm0.w;
  g4 *= norm1.x; g6 *= norm1.y; g5 *= norm1.z; g7 *= norm1.w;
  vec4 nz = mix(vec4(dot(g0, vec3(f0.x, f0.y, f0.z)), dot(g1, vec3(f1.x, f0.y, f0.z)),
        dot(g2, vec3(f0.x, f1.y, f0.z)), dot(g3, vec3(f1.x, f1.y, f0.z))),
      vec4(dot(g4, vec3(f0.x, f0.y, f1.z)), dot(g5, vec3(f1.x, f0.y, f1.z)),
        dot(g6, vec3(f0.x, f1.y, f1.z)), dot(g7, vec3(f1.x, f1.y, f1.z))), f.z);
  return 2.2 * mix(mix(nz.x,nz.z,f.y), mix(nz.y,nz.w,f.y), f.x);
}
float noise(vec2 P) { return noise(vec3(P, 0.0)); }

float turbulence2(vec3 P) {
  float f = 0., s = 1.;
  for (int i = 0 ; i < 2 ; i++) {
    f += abs(noise(s * P)) / s;
    s *= 2.;
    P = vec3(.866 * P.x + .5 * P.z, P.y, -.5 * P.x + .866 * P.z);
  }
  return f;
}

float turbulenceHighLacuniarity(vec3 P) {
  float f = 0., s = 1.5;
  for (int i = 0 ; i < 4 ; i++) {
    f += abs(noise(s * P)) / s;
    s *= 2.6;
    P = vec3(.866 * P.x + .5 * P.z, P.y, -.5 * P.x + .866 * P.z);
  }
  return f;
}

float turbulence6(vec3 P) {
  float f = 0., s = 1.;
  for (int i = 0 ; i < 6 ; i++) {
    f += abs(noise(s * P)) / s;
    s *= 2.;
    P = vec3(.866 * P.x + .5 * P.z, P.y, -.5 * P.x + .866 * P.z);
  }
  return f;
}

///////////////////////////////////////////////////////////////////////////////

struct Light {
  vec3 direction;
  vec4 ambient;
  vec4 c0;
  vec4 c1;
};

struct Ray {
  vec3 origin;
  vec3 dir;
};

///////////////////////////////////////////////////////////////////////////////
// Start attributed block
//-----------------------------------------------------------------------------
// The functions in this block are in some part based upon code found at:
// Authored: Inigo Quilez
// Date: 3/25/2013
// Title: Raymarching Primitives
// URL: https://www.shadertoy.com/view/Xds3zN
// Used citing format from: http://uark.libguides.com/content.php?pid=155080&sid=1780817
//-----------------------------------------------------------------------------

//*************************************************************************
//Distance functions of different kinds of primitives
//*************************************************************************
float dfPlane( vec3 p )
{
  return p.y;
}

float dfSphere( vec3 p, float s )
{
  return length(p)-s;
}

float dfBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) +
    length(max(d,0.0));
}

float dfTorus( vec3 p, vec2 t )
{
  vec2 q = vec2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

float dfCylinder( vec3 p, vec2 h )
{
  return max( length(p.xz)-h.x, abs(p.y)-h.y );
}


//*************************************************************************
//Operations on distance fields
//*************************************************************************
float SubtractFields( float d1, float d2 )
{
  return max(-d2,d1);
}

vec2 Union( vec2 d1, vec2 d2 )
{
  return (d1.x<d2.x) ? d1 : d2;
}

vec3 Repeat( vec3 p, vec3 c )
{
  return mod(p,c)-0.5*c;
}
// End attributed block
///////////////////////////////////////////////////////////////////////////////

//*************************************************************************
//the scene is just the union of all the distance fields in the world
//*************************************************************************
vec2 testRayAgainstScene( in vec3 pos){

  //Making a repeated cube in a grid pattern with distortions caused by
  //noise
  const float xspacing = 0.42;
  const float yspacing = 0.0;
  const float zspacing = 0.62;
  const vec3 cubeOffset = -vec3(0.0, 0.17, 0.0);
  vec3 repeatPos = Repeat(pos, vec3(xspacing, yspacing, zspacing))+cubeOffset;
  vec3 noiseOffset = vec3(pos.x*0.1,pos.y*0.3, pos.z*-0.15) *10.0;
  vec2 res = vec2( 
      dfBox(
        repeatPos,
        vec3(0.125)
        )
      +0.05 * noise((repeatPos+noiseOffset)* 1.6 + vec3(uTime))
      ,
      3.0 
      ); 

  res = Union( res, vec2( dfPlane(pos), 6.0 ));
  return res;
}
///////////////////////////////////////////////////////////////////////////////
// Start attributed block
//-----------------------------------------------------------------------------
// The functions in this block are in some part based upon code found at:
// Authored: Inigo Quilez
// Date: 3/25/2013
// Title: Raymarching Primitives
// URL: https://www.shadertoy.com/view/Xds3zN
// Used citing format from: http://uark.libguides.com/content.php?pid=155080&sid=1780817
//-----------------------------------------------------------------------------
//*************************************************************************
//Because we are using distance fields normals are calculated using
//finite differences of the distances at the sample point
//*************************************************************************
vec3 calcNormal( in vec3 pos )
{
  vec2 offset = vec2(0.001, 0.0);
  vec3 normal = vec3(
      //Finite Diff on x - axis. Remember that the test returns a vec2.
      testRayAgainstScene(pos+offset.xyy).x-testRayAgainstScene(pos-offset.xyy).x,

      //Finite Diff on y - axis. Remember that the test returns a vec2
      testRayAgainstScene(pos+offset.yxy).x-testRayAgainstScene(pos-offset.yxy).x,

      //Finite Diff on z - axis. Remember that the test returns a vec2
      testRayAgainstScene(pos+offset.yyx).x-testRayAgainstScene(pos-offset.yyx).x 
      );
  return normalize(normal);
}


//*************************************************************************
//Technically this is Sphere tracing 
//*************************************************************************
vec2 rayCast( in Ray ray, in float maxT )
{
  float t = 0.0;
  float objectID = -1.0;
  float cutoff = 0.001;
  float nextStepSize= cutoff * 2.0; 

  for(int i = 0; i < 30; i++)
  {
    //Exit if we get close to something or if we go too far
    if(abs(nextStepSize) < cutoff || t >= maxT){
      //If we went too far force a result that intersected nothing
      if(t > maxT)
        objectID = -1.0;
      continue;
    }

    t += nextStepSize;
    vec2 result = testRayAgainstScene( ray.origin+ray.dir*t );

    //result will have the distance to the closest object as its first val
    nextStepSize = result.x;

    //save the closest object id
    objectID = result.y;
  }

  return vec2( t, objectID );
}

// End attributed block
///////////////////////////////////////////////////////////////////////////////
float hardshadow( in vec3 pos, in vec3 lightDir)
{
  Ray ray = Ray(pos + lightDir*0.03, lightDir);
  float id = rayCast(ray, 10.0).y;
  return 1.0 - step(0.0, id);

}

vec3 calculateColor(in Ray ray, in Light light)
{ 
  const float maxObjectID = 6.0;
  vec3 color = vec3(0.0);
  vec2 result = rayCast(ray,20.0);
  vec3 pos = ray.origin + result.x*ray.dir;
  float objectID = result.y;

  if( objectID > 0.0 )
  {
    vec3 normal = calcNormal( pos );
    vec3 albedo = vec3(1.0, 1.0, 1.0) * (objectID/maxObjectID);
    float diffuse = max(0.0, dot(normal, light.direction.xyz));
    diffuse *= hardshadow( pos, light.direction.xyz);
    color = albedo * (light.ambient.xyz + (1.0 - light.ambient.xyz)*diffuse);

  } 
  return vec3( clamp(color,0.0,1.0) );
}


void main( void )
{
  vec2 normalizedCoord = gl_FragCoord.xy/uResoltion.xy;

  //Transforms the coord from [0,1] to [-1,1]
  vec2 p = -1.0+2.0*normalizedCoord; 

  Light light;
  light.direction = normalize(vec3(-1.1,1.0, 1.0));
  light.ambient = vec4(0.1, 0.1, 0.1, 1.0);

  //making the viewpoint to be above origin and looking down
  float focalLength = 3.0;
  vec3 viewPoint = vec3(0.0, 2.0, focalLength);
  vec3 rayDirection = normalize(vec3(0.5 - p.x, p.y - 1.6, -focalLength));

  Ray ray = Ray(viewPoint, rayDirection);

  vec3 color = calculateColor(ray, light);

  color = sqrt( color );

  //gl_FragColor=vec4( color, 1.0 );
  outColor =vec4( color, 1.0 );
}
