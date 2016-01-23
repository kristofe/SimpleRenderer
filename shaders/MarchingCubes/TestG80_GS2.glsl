#version 410
layout(points) in;
layout(triangle_strip, max_vertices = 16) out;

uniform mat4 mvp;

vec4 sampleFunction( vec3 p )
{
    return vec4(
        16.0*p.y*p.z + 4.0*2.0*p.x,
        16.0*p.x*p.z + 4.0*2.0*p.y,
        16.0*p.x*p.y + 4.0*2.0*p.z,
        16.0*p.x*p.y*p.z + 4.0*p.x*p.x + 4.0*p.y*p.y + 4.0*p.z*p.z - 1.0
        );
} 

//Volume data field texture 
uniform sampler3D dataFieldTex; 
//Edge table texture 
uniform isampler2D edgeTableTex; 
//Triangles table texture 
uniform isampler2D triTableTex; 
 
//Global iso level 
uniform float isolevel; 
//Marching cubes vertices decal 
uniform vec3 vertDecals[8]; 


 
//Vertices position for fragment shader 
out vec3 localPos; 
out vec4 position; 
out vec4 outColor;
 
//Get vertex i position within current marching cube 
vec3 cubePos(int i){ 
	//return gl_PositionIn[0].xyz + vertDecals[i]; 
	return gl_in[0].gl_Position.xyz + vertDecals[i]; 
} 
 
//Get vertex i value within current marching cube 
float cubeVal(int i){ 
	//return sampleFunction(cubePos(i)).a;
	return texture(dataFieldTex, (cubePos(i)+1.0f)/2.0f).r; 
} 
 
 
//Get triangle table value 
int triTableValue(int i, int j){ 
	return texelFetch(triTableTex, ivec2(j, i), 0).r; 
} 
 
//Compute interpolated vertex along an edge 
vec3 vertexInterp(float isolevel, vec3 v0, float l0, vec3 v1, float l1){ 
	return mix(v0, v1, (isolevel-l0)/(l1-l0)); 
} 
 
//Geometry Shader entry point 
void main(void) { 
	int cubeindex=0; 
	  
	float cubeVal0 = cubeVal(0); 
	float cubeVal1 = cubeVal(1); 
	float cubeVal2 = cubeVal(2); 
	float cubeVal3 = cubeVal(3); 
	float cubeVal4 = cubeVal(4); 
	float cubeVal5 = cubeVal(5); 
	float cubeVal6 = cubeVal(6); 
	float cubeVal7 = cubeVal(7); 
	 
	//Determine the index into the edge table which 
	//tells us which vertices are inside of the surface 
	cubeindex = int(cubeVal0 < isolevel); 
	cubeindex += int(cubeVal1 < isolevel)*2; 
	cubeindex += int(cubeVal2 < isolevel)*4; 
	cubeindex += int(cubeVal3 < isolevel)*8; 
	cubeindex += int(cubeVal4 < isolevel)*16; 
	cubeindex += int(cubeVal5 < isolevel)*32; 
	cubeindex += int(cubeVal6 < isolevel)*64; 
	cubeindex += int(cubeVal7 < isolevel)*128; 
	 
	 
	//Cube is entirely in/out of the surface 
	if (cubeindex ==0 || cubeindex == 255) 
		return; 
	 
	vec3 vertlist[12]; 
	 
	//Find the vertices where the surface intersects the cube 
	vertlist[0] = vertexInterp(isolevel, cubePos(0), cubeVal0, cubePos(1), cubeVal1); 
	vertlist[1] = vertexInterp(isolevel, cubePos(1), cubeVal1, cubePos(2), cubeVal2); 
	vertlist[2] = vertexInterp(isolevel, cubePos(2), cubeVal2, cubePos(3), cubeVal3); 
	vertlist[3] = vertexInterp(isolevel, cubePos(3), cubeVal3, cubePos(0), cubeVal0); 
	vertlist[4] = vertexInterp(isolevel, cubePos(4), cubeVal4, cubePos(5), cubeVal5); 
	vertlist[5] = vertexInterp(isolevel, cubePos(5), cubeVal5, cubePos(6), cubeVal6); 
	vertlist[6] = vertexInterp(isolevel, cubePos(6), cubeVal6, cubePos(7), cubeVal7); 
	vertlist[7] = vertexInterp(isolevel, cubePos(7), cubeVal7, cubePos(4), cubeVal4); 
	vertlist[8] = vertexInterp(isolevel, cubePos(0), cubeVal0, cubePos(4), cubeVal4); 
	vertlist[9] = vertexInterp(isolevel, cubePos(1), cubeVal1, cubePos(5), cubeVal5); 
	vertlist[10] = vertexInterp(isolevel, cubePos(2), cubeVal2, cubePos(6), cubeVal6); 
	vertlist[11] = vertexInterp(isolevel, cubePos(3), cubeVal3, cubePos(7), cubeVal7); 
	 
		 
	// Create the triangle 
	//gl_FrontColor=vec4(cos(isolevel*10.0-0.5), sin(isolevel*10.0-0.5), cos(1.0-isolevel),1.0); 
	outColor = vec4(cos(isolevel*10.0-0.5), sin(isolevel*10.0-0.5), cos(1.0-isolevel),1.0); 
	int i=0;  

	//for (i=0; triTableValue(cubeindex, i)!=-1; i+=3) { //Strange bug with this way, uncomment to test 
	while(true){ 
		if(triTableValue(cubeindex, i)!=-1){ 
			//Generate first vertex of triangle// 
			//Fill position varying attribute for fragment shader 
			position= vec4(vertlist[triTableValue(cubeindex, i)], 1); 
			localPos = position.xyz;
			outColor = vec4(1,1,1,1);
			//Fill gl_Position attribute for vertex raster space position 
			//gl_Position = gl_ModelViewProjectionMatrix* position; 
			gl_Position = mvp * position; 
			EmitVertex(); 
			 
			//Generate second vertex of triangle// 
			//Fill position varying attribute for fragment shader 
			position= vec4(vertlist[triTableValue(cubeindex, i+1)], 1); 
			localPos = position.xyz;
			outColor = vec4(1,1,1,1);
			//Fill gl_Position attribute for vertex raster space position 
			//gl_Position = gl_ModelViewProjectionMatrix* position; 
			gl_Position = mvp * position; 
			EmitVertex(); 
			 
			//Generate last vertex of triangle// 
			//Fill position varying attribute for fragment shader 
			position= vec4(vertlist[triTableValue(cubeindex, i+2)], 1); 
			localPos = position.xyz;
			outColor = vec4(1,1,1,1);
			//Fill gl_Position attribute for vertex raster space position 
			//Fill gl_Position attribute for vertex raster space position 
			//gl_Position = gl_ModelViewProjectionMatrix* position; 
			gl_Position = mvp * position; 
			EmitVertex(); 
			 
			//End triangle strip at firts triangle 
			EndPrimitive(); 
		}else{ 
			break; 
		} 
 
		i=i+3; //Comment it to test the strange bug 
	} 
 
} 
