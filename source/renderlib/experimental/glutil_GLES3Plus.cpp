#include <vector>
#include <stdio.h>
#if WIN32
#include <stdarg.h>
#include <varargs.h>
#endif

#include "include/OpenGLHelper.h"
//#include "glm/glm.hpp"
//#include "glm/gtc/type_ptr.hpp"
//#include "glm/gtc/quaternion.hpp"
//#include "glm/gtx/quaternion.hpp"
//#include "glm/gtx/euler_angles.hpp"
//#include "glm/gtx/norm.hpp"
#include "shadersource.h"
#include "vmath.hpp"

namespace renderlib{
//using namespace glm;
using namespace vmath;

GLUtil::GLUtil()
{
}
    
std::string GLUtil::getVersionString()
{
    float  glLanguageVersion;
    
    sscanf((char *)glGetString(GL_SHADING_LANGUAGE_VERSION), "OpenGL ES GLSL ES %f", &glLanguageVersion);
    
    // GL_SHADING_LANGUAGE_VERSION returns the version standard version form
    //  with decimals, but the GLSL version preprocessor directive simply
    //  uses integers (thus 1.10 should 110 and 1.40 should be 140, etc.)
    //  We multiply the floating point number by 100 to get a proper
    //  number for the GLSL preprocessor directive
    GLuint version = 100 * glLanguageVersion;
    std::ostringstream s;
    s << "#version " << version << " es" << std::endl;
    
    return s.str();
}

GLuint GLUtil::complileAndLinkProgram(
                   const std::string& fileName,
                   const std::string& vsKey,
                   const std::string& fsKey,
                   const std::string& gsKey)
{
    GLint prg = compileProgram(fileName, vsKey, fsKey, gsKey);
    linkAndVerifyProgram(prg);
    return prg;
}


GLuint GLUtil::complileAndLinkProgram(const std::string& vsFileName,
                   const std::string& fsFileName,
                   const std::string& gsFileName)
{

    std::string vsSource = getShaderSource(vsFileName);
    std::string fsSource = getShaderSource(fsFileName);
    
    GLint prg = compileProgram(vsSource, fsSource, "");
    linkAndVerifyProgram(prg);
    return prg;


}

GLuint GLUtil::compileShader(const std::string& name,
                   const std::string& source,
                   GLenum shaderType)
{
    const char* src = source.c_str();
    GLuint shaderHandle = glCreateShader(shaderType);
    glShaderSource(shaderHandle,1, &src, 0);
    glCompileShader(shaderHandle);

    GLint compileSuccess;
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);

    if(compileSuccess == GL_FALSE)
    {
        printf("Error(s) in shader %s:\n", name.c_str());
        GLchar messages[256];
        glGetShaderInfoLog(shaderHandle,sizeof(messages), 0, &messages[0]);
        printf("%s\n",messages);
        //exit(1);
    }
    else
    {
        printf("Successfully created shader %s!\n", name.c_str());
    }
    return shaderHandle;
}
    
GLuint GLUtil::compileProgram(
                               const std::string& fileName,
                               const std::string& vsKey,
                               const std::string& fsKey,
                               const std::string& gsKey)
{
    
    ShaderSource ss;
    ss.parseFile(fileName, "--");
    
    std::string vsSource = ss.getShader(vsKey);
    std::string fsSource = ss.getShader(fsKey);
    std::string gsSource = ss.getShader(gsKey);

    
    return compileProgram(vsSource, fsSource, gsSource);
}

GLuint GLUtil::compileProgram(const std::string& vsSource,
                                const std::string& fsSource,
                                const std::string& gsSource)
{
    std::string verString = getVersionString();
    //Insert the version into the source
    std::string vsSourceVersioned = verString + vsSource;
    std::string fsSourceVersioned = verString + fsSource;
    
    GLuint vertexShader = compileShader("vertex shader",vsSourceVersioned,
                                      GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader("fragment shader",fsSourceVersioned,
                                        GL_FRAGMENT_SHADER);
    
    GLuint programHandle = glCreateProgram();
    glAttachShader(programHandle, vertexShader);
    glAttachShader(programHandle, fragmentShader);
    
    //        if(gsSource.length() > 0)
    //        {
    //            GLuint geometryShader = buildShader("geometry shader",gsSource,
    //                                                GL_GEOMETRY_SHADER);
    //            glAttachShader(programHandle, geometryShader);
    //        }
    
    
    return programHandle;
}
    
GLuint GLUtil::linkAndVerifyProgram(GLuint programHandle)
{
    glLinkProgram(programHandle);
    
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if(linkSuccess == GL_FALSE)
    {
        printf("Error(s) in program:\n");
        GLchar messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        printf("%s\n", messages);
        //exit(1);
    }
    else
    {
        printf("Successfully created vertexprogram!\n");
    }
    
	glLinkProgram(programHandle);
	
	glValidateProgram(programHandle);
	
    GLint status;
	glGetProgramiv(programHandle, GL_VALIDATE_STATUS, &status);
	if (status == 0)
	{
        GLint logLength;
		printf("Failed to validate program\n");
        glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 0)
        {
            GLchar messages[256];
            glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
            printf("%s\n", messages);
        }
		return 0;
	}
    
    
    printActiveAttributes(programHandle);
    printActiveUniforms(programHandle);
    return programHandle;
}
    
    
void GLUtil::printActiveUniforms(GLuint programHandle)
{
  GLint uniformCount;
  glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &uniformCount);

  printf("Active Uniform Count: %d\n", uniformCount);
  if(uniformCount > 0)
  {
      GLint maxUniformNameLength = 0;
      glGetProgramiv(programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH,
                     &maxUniformNameLength);
      std::vector<GLchar> nameData(maxUniformNameLength);;
      for(int uniformID = 0; uniformID < uniformCount; ++uniformID)
      {
          GLint arraySize = 0;
          GLenum  type = 0;
          GLsizei actualLength = 0;
          glGetActiveUniform(
                          programHandle,
                          uniformID,
                          nameData.size(),
                          &actualLength,
                          &arraySize,
                          &type,
                          &nameData[0]
                          );
         std::string name((char*)&nameData[0], actualLength);
         printf("Uniform %d name: %s, type:%s\n", uniformID, name.c_str(),
                        GLUtil::glEnumToString(type).c_str());
        }
  }
}

void GLUtil::getActiveUniforms(
                               GLuint programHandle,
                               std::map<std::string, ShaderUniformData>* dict
                               )
{
  GLint uniformCount;
  glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &uniformCount);

  printf("Active Uniform Count: %d\n", uniformCount);
  if(uniformCount > 0)
  {
      GLint maxUniformNameLength = 0;
      glGetProgramiv(programHandle, GL_ACTIVE_UNIFORM_MAX_LENGTH,
                     &maxUniformNameLength);
      std::vector<GLchar> nameData(maxUniformNameLength);;
      for(int uniformID = 0; uniformID < uniformCount; ++uniformID)
      {
          GLint arraySize = 0;
          GLenum  type = 0;
          GLsizei actualLength = 0;
          glGetActiveUniform(
                          programHandle,
                          uniformID,
                          nameData.size(),
                          &actualLength,
                          &arraySize,
                          &type,
                          &nameData[0]
                          );
          std::string name((char*)&nameData[0], actualLength);
          ShaderUniformData sud(name, type);
          std::pair<
                    std::map< std::string, ShaderUniformData>::iterator,
                    bool
                   > res;
          res = dict->insert(std::make_pair(name, sud));
          if(res.second == false)
          {
           fprintf(
                 stderr,
                 "Can't insert key '%s' into uniform dict, it already exists.\n",
                 name.c_str()
                 );
          }
        }
  }
}

void GLUtil::printActiveAttributes(GLuint programHandle)
{
  GLint attributeCount;
  glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &attributeCount);
  printf("Active Attribute Count: %d\n", attributeCount);
  if(attributeCount > 0)
  {
      GLint maxAttributeNameLength = 0;
      glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
                     &maxAttributeNameLength);
      std::vector<GLchar> nameData(maxAttributeNameLength);;
      for(int attrib = 0; attrib < attributeCount; ++attrib)
      {
          GLint arraySize = 0;
          GLenum type = 0;
          GLsizei actualLength = 0;
          glGetActiveAttrib(
                          programHandle,
                          attrib,
                          nameData.size(),
                          &actualLength,
                          &arraySize,
                          &type,
                          &nameData[0]
                          );
         std::string name((char*)&nameData[0], actualLength);
         printf("Attribute %d name: %s, type:%s\n", attrib, name.c_str(),
                        GLUtil::glEnumToString(type).c_str());
        }
  }
}

void GLUtil::getActiveAttributes(
                                  GLuint programHandle,
                                  std::map<std::string, ShaderAttributeData>* dict
                                  )
{
  GLint attributeCount;
  glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &attributeCount);
  printf("Active Attribute Count: %d\n", attributeCount);
  if(attributeCount > 0)
  {
      GLint maxAttributeNameLength = 0;
      glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
                     &maxAttributeNameLength);
      std::vector<GLchar> nameData(maxAttributeNameLength);;
      for(int attrib = 0; attrib < attributeCount; ++attrib)
      {
          GLint arraySize = 0;
          GLenum type = 0;
          GLsizei actualLength = 0;
          glGetActiveAttrib(
                          programHandle,
                          attrib,
                          nameData.size(),
                          &actualLength,
                          &arraySize,
                          &type,
                          &nameData[0]
                          );
         std::string name((char*)&nameData[0], actualLength);
         ShaderAttributeData sd(name, type);
         std::pair<
                   std::map< std::string, ShaderAttributeData>::iterator,
                   bool
                  > res;
         res = dict->insert(std::make_pair(name, sd));
         if(res.second == false)
         {
          fprintf(
                stderr,
                "Can't insert key '%s' into attribute dict, it already exists.\n",
                name.c_str()
                );
         }
        }
  }
}

//WebGL version
/*
    GLint linkSuccess;
    glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
    if(linkSuccess == GL_FALSE)
    {
        printf("Error(s) in program:\n");
        GLchar messages[256];
        glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
        printf("%s\n", messages);
        //exit(1);
    }
    else
    {
        printf("Successfully created vertexprogram!\n");
    }
*/

std::string GLUtil::getShaderSource(const std::string& filename)
{
  std::ifstream in(filename.c_str(), std::ios::in | std::ios::binary);
  if (in)
  {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    std::cout << filename << std::endl << contents << std::endl; std::cout.flush();
    return(contents);
  }
  return "";
}

std::string GLUtil::getOpenGLInfo()
{
   std::stringstream s;
   s << "GL_VENDOR: " << (const char*)glGetString(GL_VENDOR) << std::endl;
   s << "GL_RENDERER: " << glGetString(GL_RENDERER) << std::endl;
   s << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
   s << "GL_SHADING_LANGUAGE_VERSION: " <<
        glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

   return s.str();
}

int GLUtil::checkGLErrors()
{
  int errCount = 0;
  for(GLenum currError = glGetError();
      currError != GL_NO_ERROR;
      currError = glGetError())
  {
    //Do something with `currError`.
    std::cout << "GL ERROR: " << GLUtil::glEnumToString(currError) << std::endl;
    std::cout.flush();

    ++errCount;
  }

  return errCount;
}

std::string GLUtil::glEnumToString(GLenum e)
{
  std::string str = "UNKNOWN";
  switch(e)
    {

      //Data Types
      case GL_FLOAT:
        str =  "GL_FLOAT";
        break;
      case GL_FLOAT_VEC2:
        str =  "GL_FLOAT_VEC2";
        break;
      case GL_FLOAT_VEC3:
        str =  "GL_FLOAT_VEC3";
        break;
      case GL_FLOAT_VEC4:
        str =  "GL_FLOAT_VEC4";
        break;
      case GL_FLOAT_MAT2:
        str =  "GL_FLOAT_MAT2";
        break;
      case GL_FLOAT_MAT3:
        str =  "GL_FLOAT_MAT3";
        break;
      case GL_FLOAT_MAT4:
        str =  "GL_FLOAT_MAT4";
        break;
      case GL_FLOAT_MAT2x3:
        str =  "GL_FLOAT_MAT2x3";
        break;
      case GL_FLOAT_MAT2x4:
        str =  "GL_FLOAT_MAT2x4";
        break;
      case GL_FLOAT_MAT3x2:
        str =  "GL_FLOAT_MAT3x2";
        break;
      case GL_FLOAT_MAT3x4:
        str =  "GL_FLOAT_MAT3x4";
        break;
      case GL_FLOAT_MAT4x2:
        str =  "GL_FLOAT_MAT4x2";
        break;
      case GL_FLOAT_MAT4x3:
        str =  "GL_FLOAT_MAT4x3";
        break;
      case GL_INT:
        str =  "GL_INT";
        break;
      case GL_INT_VEC2:
        str =  "GL_INT_VEC2";
        break;
      case GL_INT_VEC3:
        str =  "GL_INT_VEC3";
        break;
      case GL_INT_VEC4:
        str =  "GL_INT_VEC4";
        break;
      case GL_UNSIGNED_INT:
        str =  "GL_UNSIGNED_INT";
        break;
      case GL_UNSIGNED_INT_VEC2:
        str =  "GL_UNSIGNED_INT_VEC2";
        break;
      case GL_UNSIGNED_INT_VEC3:
        str =  "GL_UNSIGNED_INT_VEC3";
        break;
      case GL_UNSIGNED_INT_VEC4:
        str =  "GL_UNSIGNED_INT_VEC4";
        break;

      //Error Codes
      case GL_NO_ERROR:
        str =  "GL_NO_ERROR";
        break;
      case GL_INVALID_ENUM:
        str =  "GL_INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        str =  "GL_INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        str =  "GL_INVALID_OPERATION";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        str =  "GL_INVALID_FRAMEBUFFER_OPERATION";
        break;
      case GL_OUT_OF_MEMORY:
        str =  "GL_OUT_OF_MEMORY";
        break;




    case GL_SAMPLER_2D:
      str = "GL_SAMPLER_2D";
      break;

    case GL_SAMPLER_3D:
      str = "GL_SAMPLER_3D";
      break;

    case GL_SAMPLER_CUBE:
      str = "GL_SAMPLER_CUBE";
      break;

    case GL_SAMPLER_2D_SHADOW:
      str = "GL_SAMPLER_2D_SHADOW";
      break;

    case GL_SAMPLER_2D_ARRAY:
          str = "GL_SAMPLER_2D_ARRAY";
      break;

    case GL_SAMPLER_2D_ARRAY_SHADOW:
      str = "GL_SAMPLER_2D_ARRAY_SHADOW";
      break;

    case GL_INT_SAMPLER_2D:
      str = "GL_INT_SAMPLER_2D";
      break;

    case GL_INT_SAMPLER_3D:
      str = "GL_INT_SAMPLER_3D";
      break;

    case GL_INT_SAMPLER_CUBE:
      str = "GL_INT_SAMPLER_CUBE";
      break;

    case GL_INT_SAMPLER_2D_ARRAY:
      str = "GL_INT_SAMPLER_2D_ARRAY";
      break;

    case GL_UNSIGNED_INT_SAMPLER_2D:
      str = "GL_UNSIGNED_INT_SAMPLER_2D";
      break;

    case GL_UNSIGNED_INT_SAMPLER_3D:
      str = "GL_UNSIGNED_INT_SAMPLER_3D";
      break;

    case GL_UNSIGNED_INT_SAMPLER_CUBE:
          str = "GL_UNSIGNED_INT_SAMPLER_CUBE";
      break;
    }

    return str;
}


struct ProgramsRec {
    GLuint Advect;
    GLuint Jacobi;
    GLuint SubtractGradient;
    GLuint ComputeDivergence;
    GLuint ApplyImpulse;
    GLuint ApplyBuoyancy;
} Programs;

/*
void CreateObstacles(SurfacePod dest, int width, int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint program = LoadProgram("Fluid.Vertex", 0, "Fluid.Fill");
    glUseProgram(program);

    const int DrawBorder = 1;
    if (DrawBorder) {
        #define T 0.9999f
        float positions[] = { -T, -T, T, -T, T,  T, -T,  T, -T, -T };
        #undef T
        GLuint vbo;
        GLsizeiptr size = sizeof(positions);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
        GLsizeiptr stride = 2 * sizeof(positions[0]);
        glEnableVertexAttribArray(SlotPosition);
        glVertexAttribPointer(SlotPosition, 2, GL_FLOAT, GL_FALSE, stride, 0);
        glDrawArrays(GL_LINE_STRIP, 0, 5);
        glDeleteBuffers(1, &vbo);
    }

    const int DrawCircle = 1;
    if (DrawCircle) {
        const int slices = 64;
        float positions[slices*2*3];
        float twopi = 8*atan(1.0f);
        float theta = 0;
        float dtheta = twopi / (float) (slices - 1);
        float* pPositions = &positions[0];
        for (int i = 0; i < slices; i++) {
            *pPositions++ = 0;
            *pPositions++ = 0;

            *pPositions++ = 0.25f * cos(theta) * height / width;
            *pPositions++ = 0.25f * sin(theta);
            theta += dtheta;

            *pPositions++ = 0.25f * cos(theta) * height / width;
            *pPositions++ = 0.25f * sin(theta);
        }
        GLuint vbo;
        GLsizeiptr size = sizeof(positions);
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
        GLsizeiptr stride = 2 * sizeof(positions[0]);
        glEnableVertexAttribArray(SlotPosition);
        glVertexAttribPointer(SlotPosition, 2, GL_FLOAT, GL_FALSE, stride, 0);
        glDrawArrays(GL_TRIANGLES, 0, slices * 3);
        glDeleteBuffers(1, &vbo);
    }

    // Cleanup
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);

}
*/
void fatalError(const char* pStr, va_list a)
{
#if WIN32
    char msg[1024] = {0};
    vsnprintf(msg, _countof(msg), pStr, a);
    fputs(msg, stderr);
#endif
    //__builtin_trap();
    exit(1);
}


void checkCondition(int condition, ...)
{
    va_list a;
    const char* pStr;

    if (condition)
        return;

    va_start(a, condition);
    pStr = va_arg(a, const char*);
    fatalError(pStr, a);
}

SlabPod CreateSlab(GLsizei width, GLsizei height)
{
    SlabPod slab;
    slab.Ping = CreateSurface(width, height);
    slab.Pong = CreateSurface(width, height);
    return slab;
}

SurfacePod CreateSurface(GLsizei width, GLsizei height)
{
    GLuint fboHandle;
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, 0);
    checkCondition(GL_NO_ERROR == glGetError(), "Unable to create normals texture");

    GLuint colorbuffer;
    glGenRenderbuffers(1, &colorbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHandle, 0);
    checkCondition(GL_NO_ERROR == glGetError(), "Unable to attach color buffer");

    checkCondition(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER), "Unable to create FBO.");
    SurfacePod surface = { fboHandle, textureHandle };

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return surface;
}

static void ResetState()
{
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
}

/*
void InitSlabOps()
{
    Programs.Advect = LoadProgram("Fluid.Vertex", 0, "Fluid.Advect");
    Programs.Jacobi = LoadProgram("Fluid.Vertex", 0, "Fluid.Jacobi");
    Programs.SubtractGradient = LoadProgram("Fluid.Vertex", 0, "Fluid.SubtractGradient");
    Programs.ComputeDivergence = LoadProgram("Fluid.Vertex", 0, "Fluid.ComputeDivergence");
    Programs.ApplyImpulse = LoadProgram("Fluid.Vertex", 0, "Fluid.Splat");
    Programs.ApplyBuoyancy = LoadProgram("Fluid.Vertex", 0, "Fluid.Buoyancy");
}
*/

void SwapSurfaces(SlabPod* slab)
{
    SurfacePod temp = slab->Ping;
    slab->Ping = slab->Pong;
    slab->Pong = temp;
}

void ClearSurface(SurfacePod s, float v)
{
    glBindFramebuffer(GL_FRAMEBUFFER, s.FboHandle);
    glClearColor(v, v, v, v);
    glClear(GL_COLOR_BUFFER_BIT);
}
/*
void Advect(SurfacePod velocity, SurfacePod source, SurfacePod obstacles, SurfacePod dest, float dissipation)
{
    GLuint p = Programs.Advect;
    glUseProgram(p);

    GLint inverseSize = glGetUniformLocation(p, "InverseSize");
    GLint timeStep = glGetUniformLocation(p, "TimeStep");
    GLint dissLoc = glGetUniformLocation(p, "Dissipation");
    GLint sourceTexture = glGetUniformLocation(p, "SourceTexture");
    GLint obstaclesTexture = glGetUniformLocation(p, "Obstacles");

    glUniform2f(inverseSize, 1.0f / GridWidth, 1.0f / GridHeight);
    glUniform1f(timeStep, TimeStep);
    glUniform1f(dissLoc, dissipation);
    glUniform1i(sourceTexture, 1);
    glUniform1i(obstaclesTexture, 2);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source.TextureHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, obstacles.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
}

void Jacobi(SurfacePod pressure, SurfacePod divergence, SurfacePod obstacles, SurfacePod dest)
{
    GLuint p = Programs.Jacobi;
    glUseProgram(p);

    GLint alpha = glGetUniformLocation(p, "Alpha");
    GLint inverseBeta = glGetUniformLocation(p, "InverseBeta");
    GLint dSampler = glGetUniformLocation(p, "Divergence");
    GLint oSampler = glGetUniformLocation(p, "Obstacles");

    glUniform1f(alpha, -CellSize * CellSize);
    glUniform1f(inverseBeta, 0.25f);
    glUniform1i(dSampler, 1);
    glUniform1i(oSampler, 2);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pressure.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, divergence.TextureHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, obstacles.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
}

void SubtractGradient(SurfacePod velocity, SurfacePod pressure, SurfacePod obstacles, SurfacePod dest)
{
    GLuint p = Programs.SubtractGradient;
    glUseProgram(p);

    GLint gradientScale = glGetUniformLocation(p, "GradientScale");
    glUniform1f(gradientScale, GradientScale);
    GLint halfCell = glGetUniformLocation(p, "HalfInverseCellSize");
    glUniform1f(halfCell, 0.5f / CellSize);
    GLint sampler = glGetUniformLocation(p, "Pressure");
    glUniform1i(sampler, 1);
    sampler = glGetUniformLocation(p, "Obstacles");
    glUniform1i(sampler, 2);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pressure.TextureHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, obstacles.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
}

void ComputeDivergence(SurfacePod velocity, SurfacePod obstacles, SurfacePod dest)
{
    GLuint p = Programs.ComputeDivergence;
    glUseProgram(p);

    GLint halfCell = glGetUniformLocation(p, "HalfInverseCellSize");
    glUniform1f(halfCell, 0.5f / CellSize);
    GLint sampler = glGetUniformLocation(p, "Obstacles");
    glUniform1i(sampler, 1);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, obstacles.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
}

void ApplyImpulse(SurfacePod dest, Vector2 position, float value)
{
    GLuint p = Programs.ApplyImpulse;
    glUseProgram(p);

    GLint pointLoc = glGetUniformLocation(p, "Point");
    GLint radiusLoc = glGetUniformLocation(p, "Radius");
    GLint fillColorLoc = glGetUniformLocation(p, "FillColor");

    glUniform2f(pointLoc, (float) position.X, (float) position.Y);
    glUniform1f(radiusLoc, SplatRadius);
    glUniform3f(fillColorLoc, value, value, value);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glEnable(GL_BLEND);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
}

void ApplyBuoyancy(SurfacePod velocity, SurfacePod temperature, SurfacePod density, SurfacePod dest)
{
    GLuint p = Programs.ApplyBuoyancy;
    glUseProgram(p);

    GLint tempSampler = glGetUniformLocation(p, "Temperature");
    GLint inkSampler = glGetUniformLocation(p, "Density");
    GLint ambTemp = glGetUniformLocation(p, "AmbientTemperature");
    GLint timeStep = glGetUniformLocation(p, "TimeStep");
    GLint sigma = glGetUniformLocation(p, "Sigma");
    GLint kappa = glGetUniformLocation(p, "Kappa");

    glUniform1i(tempSampler, 1);
    glUniform1i(inkSampler, 2);
    glUniform1f(ambTemp, AmbientTemperature);
    glUniform1f(timeStep, TimeStep);
    glUniform1f(sigma, SmokeBuoyancy);
    glUniform1f(kappa, SmokeWeight);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, temperature.TextureHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, density.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
}
*/
void CreatePointVbo(GLuint prog, GLuint * vbo, GLuint *vao)
{
    float p[] = { 0.0, 0.0, 0.0};
	/*
	   float p[] = {
				 -1.0, -1.0, 0.0, 
				 1.0, -1.0, 0.0,
				 0.0, 1.0, 0.0
	};
	*/
	
	glGenVertexArrays(1,vao);
	glBindVertexArray(*vao);
		
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(p), p, GL_STATIC_DRAW);

	GLint vertLoc = glGetAttribLocation(prog, "Position");
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glEnableVertexAttribArray(vertLoc);
}
    
void CreateTriangleVbo(GLuint * vbo, GLuint *vao)
{
    float p[] = {
				 -1.0, -1.0, 0.0, 
				 1.0, -1.0, 0.0,
				 0.0, 1.0, 0.0
	};
	glGenVertexArrays(1,vao);
	glBindVertexArray(*vao);

    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(p), p, GL_STATIC_DRAW);
}

void CreateCubeVbo(GLuint * vbo, GLuint *vao)
{
    float p[] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        0.0, 1.0, 0.0
    };
    glGenVertexArrays(1,vao);
    glBindVertexArray(*vao);
    
    glGenBuffers(1, vbo);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(p), p, GL_STATIC_DRAW);
}
    
    
GLuint CreatePointVbo(float x, float y, float z)
{
    float p[] = {x, y, z};
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(p), &p[0], GL_STATIC_DRAW);
    return vbo;
}

void SetUniform(const char* name, int value)
{
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform1i(location, value);
}

void SetUniform(const char* name, float value)
{
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform1f(location, value);
}

//void SetUniform(const char* name, glm::mat4 value)
//{
//    GLuint program;
//    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
//    GLint location = glGetUniformLocation(program, name);
//	glUniformMatrix4fv(location, 1, 0, glm::value_ptr(value));
//}

//void SetUniform(const char* name, glm::mat3 nm)
//{
//    GLuint program;
//    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
//    GLint location = glGetUniformLocation(program, name);
//    /*
//    float packed[9] = {
//        nm.getRow(0).getX(), nm.getRow(1).getX(), nm.getRow(2).getX(),
//        nm.getRow(0).getY(), nm.getRow(1).getY(), nm.getRow(2).getY(),
//        nm.getRow(0).getZ(), nm.getRow(1).getZ(), nm.getRow(2).getZ() };
//        */
//	glUniformMatrix3fv(location, 1, 0, glm::value_ptr(nm));
//}

//void SetUniform(const char* name, glm::vec3 value)
//{
//    GLuint program;
//    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
//    GLint location = glGetUniformLocation(program, name);
//    glUniform3f(location, value.x, value.y, value.z);
//}

void SetUniform(const char* name, float x, float y)
{
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform2f(location, x, y);
}

//void SetUniform(const char* name, glm::vec4 value)
//{
//    GLuint program;
//    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
//    GLint location = glGetUniformLocation(program, name);
//    glUniform4f(location, value.x, value.y, value.z, value.w);
//}

void SetUniform(const char* name, Matrix4 value)
{
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(location, 1, 0, (float*) &value);
}

void SetUniform(const char* name, Matrix3 nm)
{
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    float packed[9] = {
        nm.getRow(0).getX(), nm.getRow(1).getX(), nm.getRow(2).getX(),
        nm.getRow(0).getY(), nm.getRow(1).getY(), nm.getRow(2).getY(),
        nm.getRow(0).getZ(), nm.getRow(1).getZ(), nm.getRow(2).getZ() };
    glUniformMatrix3fv(location, 1, 0, &packed[0]);
}

void SetUniform(const char* name, Vector3 value)
{
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform3f(location, value.getX(), value.getY(), value.getZ());
}

void SetUniform(const char* name, Vector4 value)
{
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform4f(location, value.getX(), value.getY(), value.getZ(), value.getW());
}

void SetUniform(const char* name, Point3 value)
{
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform3f(location, value.getX(), value.getY(), value.getZ());
}

/*
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

#include "quaternion_utils.hpp"


// Returns a quaternion such that q*start = dest
quat RotationBetweenVectors(vec3 start, vec3 dest){
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f){
		// special case when vectors in opposite directions :
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		// This implementation favors a rotation around the Up axis,
		// since it's often what you want to do.
		rotationAxis = cross(vec3(0.0f, 0.0f, 1.0f), start);
		if (length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
			rotationAxis = cross(vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return angleAxis(180.0f, rotationAxis);
	}

	// Implementation from Stan Melax's Game Programming Gems 1 article
	rotationAxis = cross(start, dest);

	float s = sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;

	return quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);


}



// Returns a quaternion that will make your object looking towards 'direction'.
// Similar to RotationBetweenVectors, but also controls the vertical orientation.
// This assumes that at rest, the object faces +Z.
// Beware, the first parameter is a direction, not the target point !
quat LookAt(vec3 direction, vec3 desiredUp){

	if (length2(direction) < 0.0001f )
		return quat();

	// Recompute desiredUp so that it's perpendicular to the direction
	// You can skip that part if you really want to force desiredUp
	vec3 right = cross(direction, desiredUp);
	desiredUp = cross(right, direction);

	// Find the rotation between the front of the object (that we assume towards +Z,
	// but this depends on your model) and the desired direction
	quat rot1 = RotationBetweenVectors(vec3(0.0f, 0.0f, 1.0f), direction);
	// Because of the 1rst rotation, the up is probably completely screwed up.
	// Find the rotation between the "up" of the rotated object, and the desired up
	vec3 newUp = rot1 * vec3(0.0f, 1.0f, 0.0f);
	quat rot2 = RotationBetweenVectors(newUp, desiredUp);

	// Apply them
	return rot2 * rot1; // remember, in reverse order.
}



// Like SLERP, but forbids rotation greater than maxAngle (in radians)
// In conjunction to LookAt, can make your characters
quat RotateTowards(quat q1, quat q2, float maxAngle){

	if( maxAngle < 0.001f ){
		// No rotation allowed. Prevent dividing by 0 later.
		return q1;
	}

	float cosTheta = dot(q1, q2);

	// q1 and q2 are already equal.
	// Force q2 just to be sure
	if(cosTheta > 0.9999f){
		return q2;
	}

	// Avoid taking the long path around the sphere
	if (cosTheta < 0){
		q1 = q1*-1.0f;
		cosTheta *= -1.0f;
	}

	float angle = acos(cosTheta);

	// If there is only a 2° difference, and we are allowed 5°,
	// then we arrived.
	if (angle < maxAngle){
		return q2;
	}

	// This is just like slerp(), but with a custom t
	float t = maxAngle / angle;
	angle = maxAngle;

	quat res = (sin((1.0f - t) * angle) * q1 + sin(t * angle) * q2) / sin(angle);
	res = normalize(res);
	return res;

}
*/

} //namespace renderlib
