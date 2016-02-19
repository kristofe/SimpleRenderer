// Simple path tracer. Created by Reinder Nijhoff 2014
// @reindernijhoff
//
// https://www.shadertoy.com/view/4tl3z4
//
#version 410

//KDS
out vec4 fragColor;
uniform vec2 iResolution;
uniform vec2 iMouse;
uniform float iGlobalTime;

uniform mat4 uMVMatrix;
uniform mat4 uPMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uModelInverseMatrix;
uniform float uGridResolution;
uniform sampler3D Density;

uniform vec3 uCameraPosition;


#define LIGHTCOUNT 4
#define DFSCALING 0.6
#define eps 0.0001
#define EYEPATHLENGTH 4
#define SAMPLES 8 


#define FULLBOX

//#define DOF
#define ANIMATENOISE
//#define MOTIONBLUR

#define MOTIONBLURFPS 12.

//#define LIGHTCOLOR vec3(16.86, 10.76, 8.2)*1.3
#define LIGHTCOLOR vec3(16.86, 16.76, 16.2)*0.5
#define WHITECOLOR vec3(.7295, .7355, .729)*0.7
#define GREENCOLOR vec3(.117, .4125, .115)*0.7
#define REDCOLOR vec3(.611, .0555, .062)*0.7

#define LIGHTMAT 0
#define WHITEMAT 1
#define GREENMAT  2
#define REDMAT  3

//TOP MEDIUM + LEFT = L0
//TOP BRIGHT + FRONT = L1
//FRONT = L2
//FRONT + TOP MEDIUM = L3
// front + right = L4
//TOP BRIGHT = L5

uniform float lightSwitches[4];

const vec4 lights[4]=vec4[4](
	vec4( -0.0, 4.0, -0.2, 1.5), //TOP BRIGHT: looks correct for l5
	vec4( -0.0, 4.0, -0.2, 1.5), //TOP MEDIUM: looks correct for l5
  vec4( -0.0, 3.0, 2.0, 0.65), //FRONT: looks correct for l2
  vec4( 0.2, 4.0, -2.0, 2.0) //Behind camera to its right. slightly above
);

const vec3 lightColors[4]=vec3[4](
  vec3(16.86, 16.76, 16.2)*0.65, 
  vec3(16.86, 16.76, 16.2)*0.1, 
  vec3(16.86, 16.76, 16.2)*0.1, 
  vec3(16.86, 16.76, 16.2)*0.25  
);

float seed = iGlobalTime;

float hash1() {
    return fract(sin(seed += 0.1)*43758.5453123);
}

vec2 hash2() {
    return fract(sin(vec2(seed+=0.1,seed+=0.1))*vec2(43758.5453123,22578.1459123));
}

vec3 hash3() {
    return fract(sin(vec3(seed+=0.1,seed+=0.1,seed+=0.1))*vec3(43758.5453123,22578.1459123,19642.3490423));
}
struct Ray {
  vec3 origin;
  vec3 dir;
};

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

float SubtractFields( float d1, float d2 )
{
  return max(-d2,d1);
}

vec2 Union( vec2 d1, vec2 d2 )
{
  return (d1.x<d2.x) ? d1 : d2;
}

float testRayAgainstDFTexture(in vec3 pos, out vec3 oNormal)
{
  //TODO:make these parameters
  vec3 boxOrigin = vec3(0.0, 0.0, 0.0);
  vec3 boxRadius = vec3(0.5, 0.5, 0.5);
  
  vec3 invpos = (uModelInverseMatrix * vec4(pos,1.0)).xyz;
  vec3 localPos = invpos - boxOrigin;
  vec3 localTexCoords = localPos+boxRadius;
  
  float cellSize = 1.0/uGridResolution;
  //localTexCoords = clamp(localTexCoords, 0.0, 1.0);
  localTexCoords = clamp(localTexCoords, 0.5*cellSize, 1.0 - cellSize*0.5);
  
  vec4 distField = texture(Density, localTexCoords);
  float dist = distField.w - DFSCALING*cellSize;
  oNormal = distField.xyz;
  oNormal = mat3(uNormalMatrix) * oNormal;
  oNormal = normalize(oNormal);
  //Have to add distance of outside box - This is the distance from the
  //localTexCoords plus the distance to the global position that that barycentric
  //coord represents
  vec3 samplePos = localTexCoords - boxRadius + boxOrigin;
  dist = dist + length(invpos - samplePos);
  
  return dist;
}
//*************************************************************************
//the scene is just the union of all the distance fields in the world
//*************************************************************************
vec2 testRayAgainstScene( in vec3 pos, out vec3 oNormal){
  //Is point inside box?
  //No? Return distance to box
  //Yes
  float nearestT = 9e9;
  
  vec2 res = vec2(
            testRayAgainstDFTexture(pos, oNormal), WHITEMAT
            );
  nearestT = res.x;
  /*
   res = Union( res, vec2(
            //dfBox(pos - vec3(0.0,0.0,0.0), vec3(0.5,0.5,0.5)), REDMAT
            dfBox(pos - vec3(0.0,0.0,0.0), vec3(1,1,1)), REDMAT
            ));
  res = Union(res, vec2(
  dfCylinder( pos, vec2(0.1,0.5) )
  ,
  WHITEMAT
  ));
 */ 
  
      /*
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
  if(res.x < nearestT)
  {
  	oNormal = vec3(0,1,0);
  }
  return res;
}


vec3 calcNormal( in vec3 pos )
{
  vec3 ignored;
  vec2 offset = vec2(0.0001, 0.0);
  vec3 normal = vec3(
  //Finite Diff on x - axis. Remember that the test returns a vec2.
  testRayAgainstScene(pos+offset.xyy, ignored).x-testRayAgainstScene(pos-offset.xyy, ignored).x,
  
  //Finite Diff on y - axis. Remember that the test returns a vec2
  testRayAgainstScene(pos+offset.yxy, ignored).x-testRayAgainstScene(pos-offset.yxy, ignored).x,
  
  //Finite Diff on z - axis. Remember that the test returns a vec2
  testRayAgainstScene(pos+offset.yyx, ignored).x-testRayAgainstScene(pos-offset.yyx, ignored).x
  );
  return normalize(normal);
}


//*************************************************************************
//Technically this is Sphere tracing
//*************************************************************************
vec2 rayCast( in Ray ray, in float maxT, out vec3 oNormal )
{
  float t = 0.0;
  float objectID = -1.0;
  float cutoff = 1e-6;
  float nextStepSize= cutoff * 2.0;
  
  for(int i = 0; i < 128; i++)
  {
    //Exit if we get close to something or if we go too far
    if(abs(nextStepSize) <= cutoff || t >= maxT)
    {
      //If we went too far force a result that intersected nothing
      if(t > maxT)
        objectID = -1.0;
      //continue;
      break;
    }
    
    t += nextStepSize * 0.85;//FIXME: This is a hack because I shoot through thin fields.
    vec2 result = testRayAgainstScene( ray.origin+ray.dir*t, oNormal);
    
    //result will have the distance to the closest object as its first val
    nextStepSize = result.x;
    
    //save the closest object id
    objectID = result.y;
  }
  
  return vec2( t, objectID );
}

//-----------------------------------------------------
// Intersection functions (by iq)
//-----------------------------------------------------

vec3 sphereNormal( in vec3 pos, in vec4 sph ) {
    return (pos-sph.xyz)/sph.w;
}

float sphereIntersect( in vec3 ro, in vec3 rd, in vec4 sph ) {
    vec3 oc = ro - sph.xyz;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - sph.w * sph.w;
    float h = b * b - c;
    if (h < 0.0) return -1.0;

	float s = sqrt(h);
	float t1 = -b - s;
	float t2 = -b + s;
	
	return t1 < 0.0 ? t2 : t1;
}

vec3 nPlane( in vec3 ro, in vec4 obj ) {
    return obj.xyz;
}

float planeIntersect( in vec3 ro, in vec3 rd, in vec4 pla ) {
    return (-pla.w - dot(pla.xyz,ro)) / dot( pla.xyz, rd );
}

//-----------------------------------------------------
// scene
//-----------------------------------------------------

vec3 cosWeightedRandomHemisphereDirection( const vec3 n ) {
  	vec2 r = hash2();
    
	vec3  uu = normalize( cross( n, vec3(0.0,1.0,1.0) ) );
	vec3  vv = cross( uu, n );
	
	float ra = sqrt(r.y);
	float rx = ra*cos(6.2831*r.x); 
	float ry = ra*sin(6.2831*r.x);
	float rz = sqrt( 1.0-r.y );
	vec3  rr = vec3( rx*uu + ry*vv + rz*n );
    
    return normalize( rr );
}

vec3 randomSphereDirection() {
    vec2 r = hash2()*6.2831;
	vec3 dr=vec3(sin(r.x)*vec2(sin(r.y),cos(r.y)),cos(r.x));
	return dr;
}

vec3 randomHemisphereDirection( const vec3 n ) {
	vec3 dr = randomSphereDirection();
	return dot(dr,n) * dr;
}

//-----------------------------------------------------
// light
//-----------------------------------------------------


/*
void initLights( float time ) {
	lights[0] = vec4( 1.0+2.*sin(time),2.8+2.*sin(time*0.9),0.0+0.*cos(time*0.7), .5 );
	light1 = vec4( -1.0, 1.0, 1.0, 0.5);
	light2 = vec4( 1.0, 1.0, 1.0, 0.5);
	light3 = vec4( -1.0, 1.0, -1.0, 0.5);
}
*/

vec3 sampleLight( const in vec3 ro, const int lightID ) {
  vec3 n = randomSphereDirection() * lights[lightID].w;
  return lights[lightID].xyz + n;
}

//-----------------------------------------------------
// scene
//-----------------------------------------------------

vec2 intersect( in vec3 ro, in vec3 rd, inout vec3 normal ) {
	vec2 res = vec2( 1e20, -1.0 );
  float t;
	
  /*
  //Box
  t = planeIntersect( ro, rd, vec4( 0.0, 1.0, 0.0,0.0 ) ); if( t>eps && t<res.x ) { res = vec2( t, 1. ); normal = vec3( 0., 1., 0.); }
  t = planeIntersect( ro, rd, vec4( 0.0, 0.0,-1.0,8.0 ) ); if( t>eps && t<res.x ) { res = vec2( t, 1. ); normal = vec3( 0., 0.,-1.); }
  t = planeIntersect( ro, rd, vec4( 1.0, 0.0, 0.0,0.0 ) ); if( t>eps && t<res.x ) { res = vec2( t, 2. ); normal = vec3( 1., 0., 0.); }
#ifdef FULLBOX
  t = planeIntersect( ro, rd, vec4( 0.0,-1.0, 0.0,5.49) ); if( t>eps && t<res.x ) { res = vec2( t, 1. ); normal = vec3( 0., -1., 0.); }
  t = planeIntersect( ro, rd, vec4(-1.0, 0.0, 0.0,5.59) ); if( t>eps && t<res.x ) { res = vec2( t, 3. ); normal = vec3(-1., 0., 0.); }
#endif

  //White diffuse sphere
  t = sphereIntersect( ro, rd, vec4( 1.5,1.0, 2.7, 1.0) ); if( t>eps && t<res.x ) { res = vec2( t, 1. ); normal = sphereNormal( ro+t*rd, vec4( 1.5,1.0, 2.7,1.0) ); }
  //This is the green transparent sphere
  //t = sphereIntersect( ro, rd, vec4( 4.0,1.0, 4.0, 1.0) ); if( t>eps && t<res.x ) { res = vec2( t, 6. ); normal = sphereNormal( ro+t*rd, vec4( 4.0,1.0, 4.0,1.0) ); }

*/
  //This is the light source
  t = sphereIntersect( ro, rd, lights[0] ); if( t>eps && t<res.x ) { res = vec2( t, 0.0 );  normal = sphereNormal( ro+t*rd, lights[0] ); }
  t = sphereIntersect( ro, rd, lights[1] ); if( t>eps && t<res.x ) { res = vec2( t, 0.0 );  normal = sphereNormal( ro+t*rd, lights[1] ); }
  t = sphereIntersect( ro, rd, lights[2] ); if( t>eps && t<res.x ) { res = vec2( t, 0.0 );  normal = sphereNormal( ro+t*rd, lights[2] ); }
  t = sphereIntersect( ro, rd, lights[3] ); if( t>eps && t<res.x ) { res = vec2( t, 0.0 );  normal = sphereNormal( ro+t*rd, lights[3] ); }
  //Sphere trace!!!!
  Ray ray;
  ray.origin = ro;
  ray.dir = rd;
  float maxT = 100.0;

  vec3 tmpNormal;
  vec2 res2 = rayCast(ray,maxT, tmpNormal);
  if(res2.x > eps && res2.x < res.x){
    res = res2;
    //normal = calcNormal(ray.origin + (ray.dir*res.x));
    normal = tmpNormal;
  }
					  
  return res;
}

bool intersectShadow( in vec3 ro, in vec3 rd, in float dist ) {
    float t;
	
  //t = sphereIntersect( ro, rd, vec4( 1.5,1.0, 2.7,1.0) );  if( t>eps && t<dist ) { return true; }
  //green transparent sphere
  //t = sphereIntersect( ro, rd, vec4( 4.0,1.0, 4.0,1.0) );  if( t>eps && t<dist ) { return true; }

  //Test against distance field
  Ray ray;
  ray.origin = ro + (rd * eps);
  ray.dir = rd;
  float maxT = 100.0;

  vec3 norm;
  vec2 res = rayCast(ray,maxT, norm);
  if(res.x > eps && res.x < dist) return true;

    return false; // optimisation: planes don't cast shadows in this scene
}

//-----------------------------------------------------
// materials
//-----------------------------------------------------
/*
vec3 matColor( const in float mat ) {
	vec3 nor = vec3(0., 0.95, 0.);
	
	if( mat<3.5 ) nor = REDCOLOR;
  if( mat<2.5 ) nor = GREENCOLOR;
	if( mat<1.5 ) nor = WHITECOLOR;
	if( mat<0.5 ) nor = LIGHTCOLOR;
					  
  return nor;
}*/
vec3 matColor( const in float mat ) {
  vec3 col = vec3(0., 0.95, 0.);
      
  if( mat == REDMAT ) col = REDCOLOR;
  if( mat == GREENMAT ) col = GREENCOLOR;
  if( mat == WHITEMAT ) col = WHITECOLOR;
  if( mat == LIGHTMAT ) col = LIGHTCOLOR;
          
  return col;           
}bool matIsSpecular( const in float mat ) {
    return mat > 4.5;
}

bool matIsLight( const in float mat ) {
    return mat < 0.5;
}

//-----------------------------------------------------
// brdf
//-----------------------------------------------------

vec3 getBRDFRay( in vec3 n, const in vec3 rd, const in float m, inout bool specularBounce ) {
    specularBounce = false;
    
    vec3 r = cosWeightedRandomHemisphereDirection( n );
    if(  !matIsSpecular( m ) ) {

      return r;

    } else {
      specularBounce = true;

      float n1, n2, ndotr = dot(rd,n);

      if( ndotr > 0. ) {
        n1 = 1./1.5; n2 = 1.;
        n = -n;
      } else {
        n2 = 1./1.5; n1 = 1.;
      }

      float r0 = (n1-n2)/(n1+n2); r0 *= r0;
      float fresnel = r0 + (1.-r0) * pow(1.0-abs(ndotr),5.);

      vec3 ref;

      if( hash1() < fresnel ) {
        ref = reflect( rd, n );
      } else {
        ref = refract( rd, n, n2/n1 );
      }

      return ref; // normalize( ref + 0.1 * r );
	}
}

//-----------------------------------------------------
// eyepath
//-----------------------------------------------------

vec3 traceEyePath( in vec3 ro, in vec3 rd) {
    vec3 tcol = vec3(0.);
    vec3 fcol  = vec3(1.);
    
    bool specularBounce = true;
    
    for( int j=0; j<EYEPATHLENGTH; ++j ) {
      vec3 normal;

      vec2 res = intersect( ro, rd, normal );
      if( res.y < -0.5 ) {
        return tcol;
      }

      if( matIsLight( res.y ) ) {
        if( specularBounce ) tcol += fcol*LIGHTCOLOR;
        //basecol = vec3(0.);	// the light has no diffuse component, therefore we can return col
        return tcol;
      }

      ro = ro + res.x * rd;
      rd = getBRDFRay( normal, rd, res.y, specularBounce );

      fcol *= matColor( res.y );

      for(int lightID = 0; lightID < LIGHTCOUNT; lightID++)
      {
        vec3 ld = sampleLight( ro , lightID) - ro;

        vec3 nld = normalize(ld);
        if( !specularBounce && j < EYEPATHLENGTH-1 && !intersectShadow( ro, nld, length(ld)) ) {

          float cos_a_max = sqrt(1. - clamp(lights[lightID].w * lights[lightID].w / dot(lights[lightID].xyz-ro, lights[lightID].xyz-ro), 0., 1.));
          float weight = 2. * (1. - cos_a_max);
          tcol += (fcol * lightColors[lightID] * lightSwitches[lightID]) * (weight * clamp(dot( nld, normal ), 0., 1.));
          //tcol += normal;
        }
      }
    }    
    return tcol;
}

//-----------------------------------------------------
// main
//-----------------------------------------------------
void main() {
  vec2 fragCoord = gl_FragCoord.xy;
	vec2 q = fragCoord.xy / iResolution.xy;
    
  //-----------------------------------------------------
  // camera
  //-----------------------------------------------------
  vec2 p = -1.0 + 2.0 * (fragCoord.xy) / iResolution.xy;
  p.x *= iResolution.x/iResolution.y;

#ifdef ANIMATENOISE
  seed = p.x + p.y * 3.43121412313 + fract(1.12345314312*iGlobalTime);
#else
  seed = p.x + p.y * 3.43121412313;
#endif
  vec3 ro = uCameraPosition;
  //vec3 ro = vec3(0.0, 0.0, 2.00);
  vec3 ta = vec3(0.0, -0.5,  0.0);//target point
  //vec3 ro = vec3(0.0, 0.7, 2.00);
  //vec3 ta = vec3(0.0, -0.75,  -1.00);//target point
  vec3 ww = normalize( ta - ro );
  vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
  vec3 vv = normalize( cross(uu,ww));

  //-----------------------------------------------------
  // render
  //-----------------------------------------------------

  vec3 col = vec3(0.0);
  vec3 tot = vec3(0.0);
  vec3 uvw = vec3(0.0);

  for( int a=0; a<SAMPLES; a++ ) {
    vec2 rpof = 4.*(hash2()-vec2(0.5)) / iResolution.xy;
    vec3 rd = normalize( (p.x+rpof.x)*uu + (p.y+rpof.y)*vv + 3.0*ww );

#ifdef DOF
    vec3 fp = ro + rd * 12.0;
    vec3 rof = ro + (uu*(hash1()-0.5) + vv*(hash1()-0.5))*0.125;
    rd = normalize( fp - rof );
#else
    vec3 rof = ro;
#endif        

#ifdef MOTIONBLUR
    //initLights( iGlobalTime + hash1() / MOTIONBLURFPS );
#else
    //initLights( iGlobalTime );
#endif

    col = traceEyePath( rof, rd);
    tot += col;
    seed = mod( seed*1.1234567893490423, 13. );
  }

  tot /= float(SAMPLES);
    
	tot = pow( clamp(tot,0.0,1.0), vec3(0.45) );
  fragColor = vec4( tot, 1.0 );
}


