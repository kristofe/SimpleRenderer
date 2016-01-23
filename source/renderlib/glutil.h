#ifndef KDSLIB_GLUTIL_H
#define KDSLIB_GLUTIL_H


#include <string>
#include <sstream>
#include <iostream>
#include <stdint.h> //win32 uint32_t
#include <fstream>
#include <map>
#include <memory>
#include <vector>
#include "OpenGLHelper.h"
#include "utils.h"
#include "vmath.hpp"

namespace renderlib{
  class Shader;
}

//These have to be included after utils.h because of the
// MACRO DISALLOW_COPY_AND_ASSIGN
static inline const char * GetGLErrorString(GLenum error)
{
  const char *str;
  switch( error )
  {
    case GL_NO_ERROR:
      str = "GL_NO_ERROR";
      break;
    case GL_INVALID_ENUM:
      str = "GL_INVALID_ENUM";
      break;
    case GL_INVALID_VALUE:
      str = "GL_INVALID_VALUE";
      break;
    case GL_INVALID_OPERATION:
      str = "GL_INVALID_OPERATION";
      break;
#if defined __gl_h_ || defined __gl3_h_
    case GL_OUT_OF_MEMORY:
      str = "GL_OUT_OF_MEMORY";
      break;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      str = "GL_INVALID_FRAMEBUFFER_OPERATION";
      break;
#endif
#if defined __gl_h_
    case GL_STACK_OVERFLOW:
      str = "GL_STACK_OVERFLOW";
      break;
    case GL_STACK_UNDERFLOW:
      str = "GL_STACK_UNDERFLOW";
      break;
    case GL_TABLE_TOO_LARGE:
      str = "GL_TABLE_TOO_LARGE";
      break;
#endif
    default:
      str = "(ERROR: Unknown Error Enum)";
      break;
  }
  return str;
}
/*
#define GetGLError()									\
{														\
GLenum err = glGetError();							\
  while (err != GL_NO_ERROR) {						\
fprintf(stderr,"GLError %s set in File:%s Line:%d\n",	\
GetGLErrorString(err),					\
__FILE__,								\
__LINE__);								\
err = glGetError();								\
  }													\
}
*/

#define GetGLError()									\
{														\
GLenum err = glGetError();							\
    while (err != GL_NO_ERROR) {						\
std::cerr << "GLError " << \
GetGLErrorString(err) << \
" set in File: " << __FILE__  << \
" Line: " << __LINE__ << std::endl;	\
err = glGetError();								\
  }													\
}
namespace renderlib
{
  using namespace vmath;

  enum BufferStorage{
    ARRAY_BUFFER = GL_ARRAY_BUFFER, //Vertext Attributes
    //ATOMIC_COUNTER_BUFFER = GL_ATOMIC_COUNTER_BUFFER, // Atomic Counter Storage//>= GL 4.2
    COPY_READ_BUFFER = GL_COPY_READ_BUFFER, //Buffer Copy Source
    COPY_WRITE_BUFFER = GL_COPY_WRITE_BUFFER,//Buffer Copy Destination
    //DISPATCH_INDIRECT_BUFFER = GL_DISPATCH_INDIRECT_BUFFER,//Indirect dispatch commands//>= GL 4.3
    DRAW_INDIRECT_BUFFER = GL_DRAW_INDIRECT_BUFFER,//Indirect Draw Commands
    ELEMENT_ARRAY_BUFFER = GL_ELEMENT_ARRAY_BUFFER,//Vertex Array Indices
    PIXEL_PACK_BUFFER = GL_PIXEL_PACK_BUFFER,//Pixel Read Target
    PIXEL_UNPACK_BUFFER = GL_PIXEL_UNPACK_BUFFER,//Texture Data Source
    //QUERY_BUFFER = GL_QUERY_BUFFER,//Query Result Buffer //>= GL 4.4
    //SHADER_STORAGE_BUFFER = GL_SHADER_STORAGE_BUFFER,//Read Write Storage for shaders //>=GL 4.3
    TEXTURE_BUFFER = GL_TEXTURE_BUFFER,//Texture Data Buffer
    TRANSFORM_FEEDBACK_BUFFER = GL_TRANSFORM_FEEDBACK_BUFFER,//Transform Feedback Buffer
    UNIFORM_BUFFER = GL_UNIFORM_BUFFER//Uniform Block Storage
  };

  enum BufferUsage{
    STREAM_DRAW = GL_STREAM_DRAW,
    STREAM_READ = GL_STREAM_READ, 
    STREAM_COPY = GL_STREAM_COPY, 
    STATIC_DRAW = GL_STATIC_DRAW, 
    STATIC_READ = GL_STATIC_READ, 
    STATIC_COPY = GL_STATIC_COPY, 
    DYNAMIC_DRAW = GL_DYNAMIC_DRAW, 
    DYNAMIC_READ = GL_DYNAMIC_READ, 
    DYNAMIC_COPY = GL_DYNAMIC_COPY
  };

  enum TextureFilterMode{
	  LINEAR = GL_LINEAR,
	  NEAREST = GL_NEAREST,
	  NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	  LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	  NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	  LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
  };

  enum TextureClampMode{
	  CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	  CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER,
	  MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
	  REPEAT = GL_REPEAT
  };

  enum TextureDataType{
    TDT_UBYTE = GL_UNSIGNED_BYTE,
    TDT_USHORT = GL_UNSIGNED_SHORT,
    TDT_FLOAT = GL_FLOAT
  };
  
  class TextureProxy
  {
  public:
    GLuint        glID;
    GLenum        target;//GL_TEXTURE_2D/3D Cube map etc.
    GLint         internalFormat; //GL_RGBA, GL_R16,GL_RGBA8
    GLsizei       width;
    GLsizei       height;
    GLsizei       depth;
    GLenum        format;//GL_RED, GL_RGBA
    GLenum        type;//GL_UNSIGNED_BYTE, GL_FLOAT,GL_UNSIGNED_SHORT
    std::shared_ptr<GLvoid> pixels;
    GLuint        numColorChannels;
    bool          hasAlpha;
    GLint         clampMode;
    GLint         filterMode;
    float         anisoValue;
    
  public:
    TextureProxy()
    {
      glID = 0;
      target = GL_TEXTURE_2D;
      internalFormat = GL_RGBA;
      width = 0;
      height = 0;
      depth = 0;
      format = GL_RGBA;
      type = GL_UNSIGNED_BYTE;
      //pixels = nullptr;
      numColorChannels = 4;
      hasAlpha = false;
      anisoValue = 0.0f;
      clampMode = GL_CLAMP_TO_EDGE;
      filterMode = GL_LINEAR;
 
    };
    
    ~TextureProxy()
    {
      pixels.reset();
      if (glID > 0)
      {
        glDeleteTextures(1, &glID);
      }
    }
    void uploadData();
    void updateData();
    void bindToChannel(int textureChannel);
    void unbindFromChannel(int textureChannel);
    void loadCubemap(std::string basefilename, std::string extension);
    void loadRaw3DData(const std::string& filename, int dx, int dy, int dz, TextureDataType textureDataType);
    void createPyroclasticVolume(int n, float r);
    void createPyroclasticDistanceField(int n, float r, float strength = 0.0f);
    
    void copyFrom(const TextureProxy& other)
    {
      this->glID = other.glID;
      this->target = other.target;
      this->internalFormat = other.internalFormat;
      this->width = other.width;
      this->height = other.height;
      this->depth = other.depth;
      this->format = other.format;
      this->type = other.type;
      this->pixels = other.pixels;
      this->numColorChannels = other.numColorChannels;
      this->hasAlpha = other.hasAlpha;
      this->anisoValue = other.anisoValue;
      this->clampMode = other.clampMode;
      this->filterMode = other.filterMode;
    }
    
    TextureProxy(const TextureProxy& other)
    {
      copyFrom(other);
    }
    
    void operator=(const TextureProxy& other)
    {
      copyFrom(other);
    };
  };  

  class FBOProxy
  {
  public:
    enum ATTACHMENT {
      COLOR_ATTACHMENT0 = 0,
      COLOR_ATTACHMENT1 = 1,
      COLOR_ATTACHMENT2 = 2,
      COLOR_ATTACHMENT3 = 3,
      COLOR = 0,
      NORMAL = 1,
      DEPTH = 2,
      MATERIAL = 3,
      MAX_COLOR_ATTACHMENTS = 4
    };
  public:
    GLuint      width;
    GLuint      height;
    GLuint      renderbufferID;
    GLuint      framebufferID;
    GLuint      colorTextureID[ATTACHMENT::MAX_COLOR_ATTACHMENTS];
    GLuint      depthTextureID;
    GLuint      stencilTextureID;
    GLenum      colorDataType;
    GLenum      colorInternalFormat;
    GLenum      colorFormat;
    GLenum      depthDataType;
    GLenum      stencilDataType;
    bool        hasDepth;
    bool        hasStencil;
    bool        ready;
    int         cachedViewport[4];
    int         numColorAttachments;
 
  public:
    FBOProxy():
      hasDepth(false), hasStencil(false), ready(false), numColorAttachments(1)
    {
    
    };

    void setupFBO(int pwidth, int pheight, bool create,
                  GLenum internalFormat = GL_RGBA8,
                  GLenum format = GL_RGBA,
                  TextureDataType dataType=TDT_UBYTE,
                  int numTargets = 1
                  );
    void setupDepthFBO(int width, int height);
    void bindFBO();
    void unbindFBO();
    void bindToChannel(int textureChannel,
                       int attachment = ATTACHMENT::COLOR_ATTACHMENT0);
    void unbindFromChannel(int textureChannel);
    void matchFBOSizeToViewport();
    void clear(Color c);
    

    void copyFrom(const FBOProxy& other)
    {
      this->width = other.width;
      this->height = other.height;
      this->renderbufferID = other.renderbufferID;
      for(int i = 0; i < ATTACHMENT::MAX_COLOR_ATTACHMENTS; i++)
      {
        this->colorTextureID[i] = other.colorTextureID[i];
        
      }
      this->depthTextureID = other.depthTextureID;
      this->stencilTextureID = other.stencilTextureID;
      this->colorDataType = other.colorDataType;
      this->colorInternalFormat = other.colorInternalFormat;
      this->colorFormat = other.colorFormat;
      this->depthDataType = other.depthDataType;
      this->stencilDataType = other.stencilDataType;
      this->hasDepth= other.hasDepth;
      this->hasStencil = other.hasStencil;
      this->ready = other.ready;
    }
    
    FBOProxy(const FBOProxy& other)
    {
      copyFrom(other);
    }
    
    void operator=(const FBOProxy& other)
    {
      copyFrom(other);
    };
    
    
  };
  
  struct ShaderUniformData
  {
    public:
      std::string name;
      GLenum      type;
      GLint       location;
      void*       data;

    public:
      ShaderUniformData(
                       ):
                       name(""),
                       type(GL_FLOAT),
                       location(-1),
                       data(NULL)
      {
      }
      ShaderUniformData(
                       std::string pname,
                       GLenum ptype,
                       GLint ploc
                       ):
                       name(pname),
                       type(ptype),
                       location(ploc),
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
      GLuint      location;

    public:
      ShaderAttributeData(
                       ):
                       name(""),
                       type(GL_FLOAT),
                       location(0)
      {
      }
      ShaderAttributeData(
                       std::string pname,
                       GLenum ptype
                       ):
                       name(pname),
                       type(ptype),
                       location(0)
      {
      }
      ShaderAttributeData(
                       std::string pname,
                       GLenum ptype,
                       GLuint plocation
                       ):
                       name(pname),
                       type(ptype),
                       location(plocation)
      {
      }
      ShaderAttributeData(const ShaderAttributeData& other)
      {
        this->name = other.name;
        this->type = other.type;
        this->location = other.location;
      }

      void operator=(const ShaderAttributeData& other)
      {
        this->name = other.name;
        this->type = other.type;
        this->location = other.location;
      };

};

  struct ShaderColorData
  {
  public:
    std::string name; //Corresponds to a name of a data stream
    GLuint      location;
    
  public:
    ShaderColorData(): name(""), location(0)
    {
    }
    ShaderColorData( std::string pname, int loc): name(pname), location(loc)
    {
    }
    
    ShaderColorData(const ShaderColorData& other)
    {
      this->name = other.name;
      this->location = other.location;
    }
    
    void operator=(const ShaderColorData& other)
    {
      this->name = other.name;
      this->location = other.location;
    };
    
  };
    enum ShaderType{
      VERTEX = GL_VERTEX_SHADER,
      TESSELATION_CONTROL = GL_TESS_CONTROL_SHADER,
      TESSELATION_EVALUATION = GL_TESS_EVALUATION_SHADER,
      GEOMETRY = GL_GEOMETRY_SHADER,
      FRAGMENT = GL_FRAGMENT_SHADER,
#ifndef GL_COMPUTE_SHADER
      COMPUTE = 0
#else
      COMPUTE = GL_COMPUTE_SHADER
#endif
    };

    struct ShaderInfo{
      std::string filename;
      ShaderType  shaderType;
      std::string source;
      GLuint      glID;
      std::string hash;
      std::string name;
      bool        changed;
      bool        immutable;
      bool        compiled;
      
      ShaderInfo():filename(""),shaderType(ShaderType::VERTEX),source(""),
                   glID(0), hash(""),name(""), changed(false), immutable(false), compiled(false)
      {
      };
      
      ShaderInfo(const ShaderInfo& other)
      {
        this->filename = other.filename;
        this->shaderType = other.shaderType;
        this->source = other.source;
        this->glID = other.glID;
        this->hash = other.hash;
        this->name = other.name;
        this->changed = other.changed;
        this->immutable = other.immutable;
        this->compiled = other.compiled;
      };
      
      void operator=(const ShaderInfo& other)
      {
        this->filename = other.filename;
        this->shaderType = other.shaderType;
        this->source = other.source;
        this->glID = other.glID;
        this->hash = other.hash;
        this->name = other.name;
        this->changed = other.changed;
        this->immutable = other.immutable;
        this->compiled = other.compiled;
      };
    };

    struct ProgramInfo{
      std::string  name;
      std::vector<ShaderInfo*> shaders;
      renderlib::Shader*      owner;
      //std::map<std::string, ShaderUniformData>* uniforms;
      //std::map<std::string, ShaderAttributeData>* attributes;
      GLuint                   glID;
      
      ProgramInfo():name(""),owner(nullptr), glID(0)
      {
      }
      
      ProgramInfo(const ProgramInfo& other)
      {
        this->name = other.name;
        this->shaders = other.shaders;
        this->owner = other.owner;
        this->glID = other.glID;
      }
      
      void operator=(const ProgramInfo& other)
      {
        this->name = other.name;
        this->shaders = other.shaders;
        this->owner = other.owner;
        this->glID = other.glID;
      };
    };
  struct VertexAttributeInfo
  {
    VertexAttributeInfo(const std::string pname,
                        void* pbuffer_offset,
                        int pmemory_type,
                        int pcount_of_memory_type,
                        int plocation,
                        size_t pSizeOfVertex
                        ) :
    name(pname),
    buffer_offset(pbuffer_offset),
    memory_type(pmemory_type),
    count_of_memory_type(pcount_of_memory_type),
    location(plocation),
    sizeOfVertex(pSizeOfVertex)
    {
    }
    
    VertexAttributeInfo(const VertexAttributeInfo& other)
    {
      this->name = other.name;
      this->buffer_offset = other.buffer_offset;
      this->memory_type = other.memory_type;
      this->count_of_memory_type = other.count_of_memory_type;
      this->location = other.location;
      this->sizeOfVertex = other.sizeOfVertex;
    }
    
    void operator=(const VertexAttributeInfo& other)
    {
      this->name = other.name;
      this->buffer_offset = other.buffer_offset;
      this->memory_type = other.memory_type;
      this->count_of_memory_type = other.count_of_memory_type;
      this->location = other.location;
      this->sizeOfVertex = other.sizeOfVertex;
    }
    std::string name;
    void* buffer_offset;
    int memory_type;
    int count_of_memory_type;
    int location;
    size_t sizeOfVertex;
  };
  
  struct MeshBufferInfo
  {
    
    GLuint vao;
    GLuint vbo;
    GLenum bufferType;
    GLenum memTransferBehavior;
    GLenum primitiveType;
    GLuint ibo;
    GLenum indiceType;
    GLuint numIndices;
    std::vector<VertexAttributeInfo> cachedAttributes;
    GLuint sizeOfVertex;
    bool hasIndices;
    
    MeshBufferInfo(): vao(-1), vbo(-1), bufferType(GL_ARRAY_BUFFER),
    memTransferBehavior(GL_STATIC_DRAW),
    primitiveType(GL_TRIANGLES),
    ibo(-1),
    indiceType(GL_UNSIGNED_INT),
    numIndices(0),
    hasIndices(false)
    {
    };
    
    MeshBufferInfo(GLenum pBufferType,
                   GLenum pMemTranferType,
                   GLenum pPrimitiveType):
    vao(-1),
    vbo(-1),
    bufferType(pBufferType),
    memTransferBehavior(pMemTranferType),
    primitiveType(pPrimitiveType),
    ibo(-1),
    indiceType(GL_UNSIGNED_INT),
    numIndices(0),
    hasIndices(false)
    {
    };
    
    ~MeshBufferInfo()
    {
      glDeleteBuffers(1,&vbo);
      glDeleteBuffers(1,&ibo);
      
    }
    //bool constructBuffer(Mesh& mesh);
    //bool updateBuffer(Mesh& mesh);
    //bool enableVertexAttributes(Mesh& mesh, Shader& program);
    bool constructBuffer(void* pointerToBuffer, int numBytes);
    bool constructBuffer(void* pointerToBuffer, int numBytes,
                         void* pointerToIndices, int numBytesIndices);
    bool bindAttributesToVAO(const std::vector<VertexAttributeInfo> & attributes, Shader& program);
    void bindAttributes(const std::vector<VertexAttributeInfo> & attributes, Shader& program);
    bool bindAttributesToVAO(const std::vector<VertexAttributeInfo> & attributes);
    void bindAttributes(const std::vector<VertexAttributeInfo> & attributes);
    void enableVertexAttributes();
    void disableVertexAttributes();
    bool updateBuffer(void* pointerToBuffer, int numBytes, bool expand = false);
    bool updateBuffer(void* pointerToBuffer, int numBytes,
                      void* pointerToIndices, int numBytesIndices, bool expand = false);
    void drawBuffer(uint32_t startIndex, uint32_t numVertices);
    
    
  };

   class GLUtil
   {
   public:
     GLUtil();
     static std::string getVersionString();

     static GLuint compileShader(const std::string& name,
                               const std::string& source, GLenum shaderType);

     static bool recompileShader(ShaderInfo& shaderInfo);

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

    static bool relinkAndVerifyProgram(ProgramInfo& programInfo);
    static bool recreateProgramAndAttachShaders(ProgramInfo& programInfo);

    static GLuint linkAndVerifyProgram(GLuint programHandle, ProgramInfo* programInfo = nullptr);

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
     static void bindAttributeLocation(GLuint program, GLuint index, const char* name);
     static void bindColorLocation(GLuint program, GLuint index, const char* name);


     static std::string glEnumToString(GLenum e);
     static std::string getOpenGLInfo();
     static int checkGLErrors();
     
     static void resizeFBO(FBOProxy& fbo);
     static void setupFBO(FBOProxy& fbo,bool create);
     static void setupMRT(FBOProxy& fbo,bool create);
     static void setupDepthFBO(FBOProxy& fbo);
     static void setDepthTest(bool test) { test?glEnable(GL_DEPTH_TEST):glDisable(GL_DEPTH_TEST);}
     static void setDepthWrite(bool test) {glDepthMask(test);}
     static void cullOff() { glDisable(GL_CULL_FACE);}
     static void cullOn() { glEnable(GL_CULL_FACE);}
     static void cullBackFaces() { glCullFace(GL_BACK);}
     static void cullFrontFaces() { glCullFace(GL_FRONT);}
     static void cullFrontAndBackFaces() { glCullFace(GL_FRONT_AND_BACK);}
     static void setBlend(bool test) {
       test ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
     }
     static void setBlendFuncToComposite(){
       glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
     }


   //private:
   //  std::map<std::string, unsigned int>		mStringToEnumDB;
     static GLuint loadRaw3DData(const std::string& filename, int dx, int dy, int dz, TextureProxy& proxy);
     static GLuint createPyroclasticVolume(int n, float r, TextureProxy& proxy);
     static GLuint createPyroclasticDistanceField(int n, float r, float strength, TextureProxy& proxy);

     static std::string getCubeMapFilenameForTarget(
                                                    std::string basename,
                                                    std::string fileExtension,
                                                    GLenum target
                                                    );
     static GLuint createCubemap(std::string filenameBase, std::string fileExtension,
                                 TextureProxy& proxy);
     static void enableAnisotropicFiltering(float pctMax);
     static void clear(bool color, bool depth, bool stencil);
   };


   enum AttributeSlot {
       SlotPosition,
       SlotTexCoord,
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
//   void SetUniform(const char* name, glm::mat4x4 value);
//   void SetUniform(const char* name, glm::mat3x3 value);
//   void SetUniform(const char* name, glm::vec4 value);
//   void SetUniform(const char* name, glm::vec3 value);
   void SetUniform(const char* name, vmath::Matrix4 value);
   void SetUniform(const char* name, vmath::Matrix3 value);
   void SetUniform(const char* name, vmath::Vector3 value);
   void SetUniform(const char* name, vmath::Vector2 value);
   void SetUniform(const char* name, vmath::Point3 value);
   void SetUniform(const char* name, vmath::Vector4 value);
   //TexturePod LoadTexture(const char* path);
   SurfacePod CreateSurface(int width, int height);
   void CreateTriangleVbo(GLuint * vbo, GLuint * vao);
   void CreateCubeVbo(GLuint * vbo, GLuint * vao);
   void CreatePointVbo(GLuint prog, GLuint * vbo, GLuint * vao);
   GLuint CreatePointVbo(float x, float y, float z);
  
}



#endif // KDSLIB_GLUTIL_H
