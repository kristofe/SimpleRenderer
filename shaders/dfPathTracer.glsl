#version 150
out vec4 outColor;
in vec2 vUV;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform float uTime;
uniform vec3 uResoltion;
uniform vec4 uMouse;

///////////////////////////////////////////////////////////////////////////////
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

// End attributed block
///////////////////////////////////////////////////////////////////////////////

//*************************************************************************
//the scene is just the union of all the distance fields in the world
//*************************************************************************
vec2 testRayAgainstScene( in vec3 pos){
  vec2 res = vec2(
  dfCylinder( pos-vec3( 0.1,0.41,-0.0), vec2(0.2,0.4) )
  +
  0.08
  ,
  4.0
  );
  
  res = Union( res, 
      vec2( dfSphere( pos-vec3(0.2,0.35,-0.5), 0.4) + 0.05 , 2.0)
      );
  res = Union( res, 
      vec2( dfSphere( pos-vec3(0.8,0.35,-1.3), 0.4) + 0.05 , 2.0)
      );

  res = Union( res,
      vec2(
            dfSphere(
              pos-vec3(1.2,0.35,-0.5),
              0.4
            ) + 0.05 ,
            3.0
          )
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
  
  for(int i = 0; i < 80; i++)
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
  light.direction = normalize(vec3(0.675,1.0, 1.0));
  light.ambient = vec4(0.1, 0.1, 0.1, 1.0);
  
  //making the viewpoint to be above origin and looking down
  float focalLength = 3.0;
  vec3 viewPoint = vec3(0.0, 2.0, focalLength);
  vec3 rayDirection = normalize(vec3(0.5 - p.x, p.y - 1.6, -focalLength));
  
  Ray ray = Ray(viewPoint, rayDirection);
  
  vec3 color = calculateColor(ray, light);
  
  color = sqrt( color );
  
  outColor = vec4( color, 1.0 );
}

