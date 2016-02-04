#version 150
out vec4 outColor;
in vec2 vUV;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uModelInverseMatrix;
uniform float iGlobalTime;
uniform float uGridResolution;
uniform vec2 iResolution;
uniform vec4 uMouse;
uniform sampler3D Density;

#define NUMSAMPLES 1

#define LIGHTCOLOR vec3(16.86, 10.76, 8.2)*1.3
#define WHITECOLOR vec3(.7295, .7355, .729)*0.7
#define GREENCOLOR vec3(.117, .4125, .115)*0.7
#define REDCOLOR vec3(.611, .0555, .062)*0.7

#define LIGHTMAT 0
#define WHITEMAT 1
#define GREENMAT  2
#define REDMAT  3
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
float testRayAgainstDFTexture(in vec3 pos)
{
  //Forcing the box to be at the origin helps with the math when getting started
  vec3 boxOrigin = vec3(0.0, 0.0, 0.0);
  //Fixing the radius to 0.5 helps with the math
  vec3 boxRadius = vec3(0.5, 0.5, 0.5);
  
  pos = (uModelInverseMatrix * vec4(pos,1.0)).xyz;
  vec3 localPos = pos - boxOrigin;
  
  //If I subtract box radius then anything inside will have all 3 components <0
  //vec3 test3 = distPerAxis - boxRadius;
  //float test = max(test3.x, max(test3.y, test3.z));
  //vec3 distPerAxis = abs(localPos);
  
  
  //FIXME: I don't think these tex coords are correct
  //check this is correct
  //vec3 localTexCoords = (localPos+boxRadius)/(boxRadius*2.0);
  vec3 localTexCoords = localPos+boxRadius;
  
  //clamp them to [0,1]
  float cellSize = 1.0/uGridResolution;
  //localTexCoords = clamp(localTexCoords, 0.0, 1.0);
  localTexCoords = clamp(localTexCoords, cellSize, 1.0 - cellSize);
  
  float dist = texture(Density, localTexCoords).r - 0.5*cellSize;
  //Have to add distance of outside box - This is the distance from the
  //localTexCoords plus the distance to the global position that that barycentric
  //coord represents
  vec3 samplePos = localTexCoords - boxRadius;
  dist = dist + length(pos - samplePos);
  
  return dist;
}
//*************************************************************************
//the scene is just the union of all the distance fields in the world
//*************************************************************************
vec2 testRayAgainstScene( in vec3 pos){
  //Is point inside box?
  //No? Return distance to box
  //Yes
  
  vec2 res = vec2(
            testRayAgainstDFTexture(pos), GREENMAT
            );
  
  /*
   res = Union( res, vec2(
            dfBox(pos - vec3(0.0,0.0,0.0), vec3(0.5,0.5,0.01)), REDMAT
            ));
  
  vec2 res = vec2(
  dfCylinder( pos-vec3( 0.1,0.41,-0.0), vec2(0.2,0.4) )
  +
  0.08
  ,
  WHITEMAT
  );
  
  res = Union( res, 
      vec2( dfSphere( pos-vec3(0.2,0.35,-0.5), 0.4) + 0.05 , GREENMAT)
      );
  res = Union( res, 
      vec2( dfSphere( pos-vec3(0.8,0.35,-1.3), 0.4) + 0.05 , GREENMAT)
      );

  res = Union( res,
      vec2(
            dfSphere(
              pos-vec3(1.2,0.35,-0.5),
              0.4
            ) + 0.05 ,
            REDMAT
          )
  );*/
  res = Union( res, vec2( dfPlane(pos - vec3(-0.5)), WHITEMAT ));
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
  vec2 offset = vec2(0.0001, 0.0);
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


vec3 matColor( const in float mat ) {
  vec3 col = vec3(0., 0.95, 0.);
      
  if( mat == REDMAT ) col = REDCOLOR;
  if( mat == GREENMAT ) col = GREENCOLOR;
  if( mat == WHITEMAT ) col = WHITECOLOR;
  if( mat == LIGHTMAT ) col = LIGHTCOLOR;
          
  return col;           
}

//*************************************************************************
//Technically this is Sphere tracing
//*************************************************************************
vec2 rayCast( in Ray ray, in float maxT )
{
  float t = 0.0;
  float objectID = -1.0;
  float cutoff = 1e-6;
  float nextStepSize= cutoff * 2.0;
  float lastStep = 0.0;
  
  for(int i = 0; i < 32; i++)
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
    lastStep = nextStepSize;
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
  float id = rayCast(ray, 100.0).y;
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
    vec3 albedo = matColor(objectID);//vec3(1.0, 1.0, 1.0) * (objectID/maxObjectID);
    float diffuse = max(0.0, dot(normal, light.direction.xyz));
    diffuse *= hardshadow( pos, light.direction.xyz);
    color = albedo * (light.ambient.xyz + (1.0 - light.ambient.xyz)*diffuse);
    
  }
  return vec3( clamp(color,0.0,1.0) );
}



/*
//Original sphere tracing kernel
void main( void )
{
  vec2 normalizedCoord = gl_FragCoord.xy/iResolution.xy;
  
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
  
  //apply gamma correction
  color = sqrt( color );
  
  outColor = vec4( color, 1.0 );
}
*/

//Random functions
float seed = iGlobalTime;

float hash1() { return fract(sin(seed += 0.1)*43758.5453123); }

vec2 hash2() { return fract(sin(vec2(seed+=0.1,seed+=0.1))*vec2(43758.5453123,22578.1459123)); }

vec3 hash3() { return fract(sin(vec3(seed+=0.1,seed+=0.1,seed+=0.1))*vec3(43758.5453123,22578.1459123,19642.3490423)); }

void main( void )
{
  vec2 normalizedCoord = gl_FragCoord.xy/iResolution.xy;
  //Transforms the coord from [0,1] to [-1,1]
  vec2 p = -1.0+2.0*normalizedCoord;
  //Correct aspect ratio
  p.x *= iResolution.x/iResolution.y;

  //animated noise
#ifdef ANIMATENOISE
  seed = p.x + p.y * 3.43121412313 + fract(1.12345314312*iGlobalTime);
#else
  seed = p.x + p.y * 3.43121412313;
#endif

  //FIXME: THIS MAY FLIP THE X AXIS
  //vec3 ro = vec3(2.78, 2.73, -8.00);
  //vec3 ta = vec3(2.78, 2.73,  0.00);//what does "ta" stand for?
  vec3 ro = vec3(0.0, 1.0, 3.00);
  vec3 ta = vec3(0.0, -0.5,  -1.00);//target point
  vec3 ww = normalize( ta - ro );
  vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
  vec3 vv = normalize( cross(uu,ww));

  //-----------------------------------------------------
  // render
  //-----------------------------------------------------

  vec3 col = vec3(0.0);
  vec3 tot = vec3(0.0);
  vec3 uvw = vec3(0.0);

  /*
  float shutterAperture = 0.6;
  float fov = 2.5;
  //float focusDistance = 1.3;
  float focusDistance = 2.0;
  float blurAmount = 0.0015;
  int numLevels = 5;
  vec2 pixel = gl_FragCoord.xy/iResolution.xy;
  */

  // 256 paths per pixel
  for( int i=0; i<NUMSAMPLES; i++ )
  {
    /*
    // screen coords with antialiasing
    vec2 p = (-iResolution + 2.0*(pixel + hash2())) / iResolution.y;
    
    // motion blur
    float ctime = frameTime + shutterAperture*(1.0/24.0)*hash();
    
    // move objects
    worldMoveObjects( ctime );
    
    // get camera position, and right/up/front axis
    vec3 (ro, uu, vv, ww) = worldMoveCamera( ctime );
    
    // create ray with depth of field
    vec3 er = normalize( vec3( p.xy, fov ) );
    vec3 rd = er.x*uu + er.y*vv + er.z*ww;
    
    vec3 go = blurAmount*vec3( -1.0 + 2.0*random2f(), 0.0 );
    vec3 gd = normalize( er*focusDistance - go );
    ro += go.x*uu + go.y*vv;
    rd += gd.x*uu + gd.y*vv;
    */
#if NUMSAMPLES == 1
    vec2 rpof = 4.*vec2(0.5) / iResolution.xy;
    vec3 rd = normalize( (p.x+rpof.x)*uu + (p.y+rpof.y)*vv + 3.0*ww );
#else
    vec2 rpof = 4.*(hash2()-vec2(0.5)) / iResolution.xy;
    vec3 rd = normalize( (p.x+rpof.x)*uu + (p.y+rpof.y)*vv + 3.0*ww );
#endif

    vec3 rof = ro;
    
    Ray ray = Ray(ro, rd);
    
    Light light;
    light.direction = normalize(vec3(0.675,1.0, 1.0));
    light.ambient = vec4(0.1, 0.1, 0.1, 1.0);
    col += calculateColor(ray, light);
    // accumulate path
    //col += rendererCalculateColor( ro, normalize(rd), numLevels );
  }
  //TODO: Put back
  col = col / NUMSAMPLES;

  // apply gamma correction
  //col = pow( col, 0.45 );
  col = sqrt( col );

  outColor = vec4(col,1.0);




  /*
  vec2 normalizedCoord = gl_FragCoord.xy/iResolution.xy;

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
  
  //apply gamma correction
  color = sqrt( color );
  
  outColor = vec4( color, 1.0 );
  */
}

