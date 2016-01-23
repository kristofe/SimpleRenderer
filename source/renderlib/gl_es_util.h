#ifndef KDSLIB_GLUTIL_H
#define KDSLIB_GLUTIL_H


#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <map>
#include "OpenGLHelper.h"
#include "utils.h"
#include "../glm/glm.hpp"
#include "../glm/gtc/quaternion.hpp"
#include "../glm/gtx/quaternion.hpp"

#include "../vmath.hpp"

//These have to be included after utils.h because of the
// MACRO DISALLOW_COPY_AND_ASSIGN

namespace renderlib
{
  using namespace vmath;

  struct ShaderUniformData
  {
    public:
      std::string name;
      GLenum      type;
      GLint       location;
      void*       data;

    public:
      ShaderUniformData(
                       std::string pname,
                       GLenum ptype
                       ):
                       name(pname),
                       type(ptype),
                       location(-1),
                       data(NULL)
      {
      }
      ShaderUniformData(const ShaderUniformData& other)
      {
        this->name = other.name;
        this->type = other.type;
        this->location = other.location;
        this->data = other.data;
      }

      void operator=(const ShaderUniformData& other)
      {
        this->name = other.name;
        this->type = other.type;
        this->location = other.location;
        this->data = other.data;
      }
  };

  struct ShaderAttributeData
  {
    public:
      std::string name; //Corresponds to a name of a data stream
      GLenum      type;

    public:
      ShaderAttributeData(
                       std::string pname,
                       GLenum ptype
                       ):
                       name(pname),
                       type(ptype)
      {
      }
      ShaderAttributeData(const ShaderAttributeData& other)
      {
        this->name = other.name;
        this->type = other.type;
      }

      void operator=(const ShaderAttributeData& other)
      {
        this->name = other.name;
        this->type = other.type;
      };
};

    class GLUtil
    {
    public:
        GLUtil();
        static std::string getVersionString();
        
        static GLuint compileShader(const std::string& name,
                                  const std::string& source, GLenum shaderType);
        
        static std::string getShaderSource(const std::string& filename);
        
        
         static GLuint complileAndLinkProgram(const std::string& fileName,
                                   const std::string& vsKey,
                                   const std::string& fsKey,
                                   const std::string& gsKey);
         
         static GLuint complileAndLinkProgram(const std::string& vsFileName,
                                   const std::string& fsFileName,
                                   const std::string& gsFileName);
         
        
        static GLuint compileProgram(const std::string& vsSource,
                                     const std::string& fsSource,
                                     const std::string& gsSource);
        
        static GLuint compileProgram(const std::string& fileName,
                                     const std::string& vsKey,
                                     const std::string& fsKey,
                                     const std::string& gsKey);
        
        static GLuint linkAndVerifyProgram(GLuint programHandle);
        
        static void printActiveUniforms(GLuint programHandle);
        static void getActiveUniforms(
                                      GLuint programHandle,
                                      std::map<std::string, ShaderUniformData>* dict
                                      );
        
        static void printActiveAttributes(GLuint programHandle);
        static void getActiveAttributes(
                                        GLuint programHandle,
                                        std::map<std::string, ShaderAttributeData>* dict
                                        );
        
        
        static std::string glEnumToString(GLenum e);
        static std::string getOpenGLInfo();
        static int checkGLErrors();
        
        //private:
        //  std::map<std::string, unsigned int>		mStringToEnumDB;
        
    };


   enum AttributeSlot {
       SlotPosition,
       SlotTexCoord,
   };

   struct ITrackball {
       virtual void MouseDown(int x, int y) = 0;
       virtual void MouseUp(int x, int y) = 0;
       virtual void MouseMove(int x, int y) = 0;
       virtual void ReturnHome() = 0;
       virtual glm::mat3 GetRotation() const = 0;
       virtual float GetZoom() const = 0;
       virtual void Update(unsigned int microseconds) = 0;
   };

   struct TexturePod {
       GLuint Handle;
       GLsizei Width;
       GLsizei Height;
   };

   struct SurfacePod {
       GLuint FboHandle;
       GLuint ColorTexture;
   };

   struct SlabPod {
       SurfacePod Ping;
       SurfacePod Pong;
   };

   void fatalError(const char* pStr, va_list a);
   void checkCondition(int condition, ...);

   void SetUniform(const char* name, int value);
   void SetUniform(const char* name, float value);
   void SetUniform(const char* name, float x, float y);
   void SetUniform(const char* name, glm::mat4x4 value);
   void SetUniform(const char* name, glm::mat3x3 value);
   void SetUniform(const char* name, glm::vec4 value);
   void SetUniform(const char* name, glm::vec3 value);
   void SetUniform(const char* name, vmath::Matrix4 value);
	void SetUniform(const char* name, vmath::Matrix3 value);
	void SetUniform(const char* name, vmath::Vector3 value);
	void SetUniform(const char* name, vmath::Point3 value);
	void SetUniform(const char* name, vmath::Vector4 value);
   //TexturePod LoadTexture(const char* path);
   SurfacePod CreateSurface(int width, int height);
   void CreateTriangleVbo(GLuint * vbo, GLuint * vao);
   void CreatePointVbo(GLuint prog, GLuint * vbo, GLuint * vao);
   GLuint CreatePointVbo(float x, float y, float z);
}

#endif // KDSLIB_GLUTIL_H
