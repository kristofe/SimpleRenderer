#include <vector>
#include <stdio.h>
#if WIN32
#include <stdarg.h>
#include <varargs.h>
#endif

#include <sstream>
#include "OpenGLHelper.h"
//#include "glutil.h"
#include "ShaderManager.h"
#include "mesh.h"
#include "shadersource.h"
#include "vmath.hpp"
#include "ImageUtils.h"

extern "C" {
//#include "renderlib/perlin.h"
#include "perlin.h"
}
namespace renderlib{
    using namespace vmath;

    void TextureProxy::loadRaw3DData(const std::string& filename, int dx, int dy, int dz, TextureDataType textureDataType)
    {
      this->type = textureDataType;
      GLUtil::loadRaw3DData(filename,dx,dy,dz, *this);
    }

    void TextureProxy::createPyroclasticVolume(int n, float r)
    {
      GLUtil::createPyroclasticVolume(n, r, *this);
    }

    void TextureProxy::createPyroclasticDistanceField(int n, float r, float strength)
    {
      GLUtil::createPyroclasticDistanceField(n, r, strength, *this);
    }
  
    void TextureProxy::bindToChannel(int textureChannel)
    {
      glActiveTexture(GL_TEXTURE0 + textureChannel);
      GetGLError();
      glBindTexture(target, glID);
      GetGLError();
    }

    void TextureProxy::unbindFromChannel(int textureChannel)
    {
      glActiveTexture(GL_TEXTURE0 + textureChannel);
      GetGLError();
      glBindTexture(target, 0);
      GetGLError();
    }

    void TextureProxy::loadCubemap(std::string basefilename,
                                   std::string extension)
    {
      GLUtil::createCubemap(basefilename, extension, *this);
    }

    void TextureProxy::uploadData()
    {
      GetGLError();
      if(glID == 0)
      {
        glGenTextures(1, &glID);
      }
      glBindTexture(target, glID);
      GetGLError();
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, filterMode);
      GetGLError();
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, filterMode);
      GetGLError();
      glTexParameteri(target, GL_TEXTURE_WRAP_S, clampMode);
      GetGLError();
      glTexParameteri(target, GL_TEXTURE_WRAP_T, clampMode);
      GetGLError();
      if (target == GL_TEXTURE_3D)
      {
        glTexParameteri(target, GL_TEXTURE_WRAP_R, clampMode);
        GetGLError();
      }
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      GetGLError();

      if(anisoValue != 0.0f)
      {
        GLUtil::enableAnisotropicFiltering(anisoValue);
      }
      if (target == GL_TEXTURE_2D)
      {
        glTexImage2D(target,0, internalFormat, width, height, 0, format, type, pixels.get());
        GetGLError();
      }
      if (target == GL_TEXTURE_3D)
      {
        glTexImage3D(target,0, internalFormat, width, height,depth, 0, format, type, pixels.get());
        GetGLError();
      }

      glGenerateMipmap(target);
      GetGLError();
    }

    void TextureProxy::updateData()
    {
      glBindTexture(GL_TEXTURE_2D, glID);
      glTexSubImage2D(target,0, 0, 0,  width, height, format, type, pixels.get());
      GetGLError();

    }


    void FBOProxy::bindToChannel(int textureChannel,int attachment)
    {
      glActiveTexture(GL_TEXTURE0 + textureChannel);
      GetGLError();
      glBindTexture(GL_TEXTURE_2D, colorTextureID[attachment]);
      GetGLError();
    }

    void FBOProxy::unbindFromChannel(int textureChannel)
    {
      glActiveTexture(GL_TEXTURE0 + textureChannel);
      GetGLError();
      glBindTexture(GL_TEXTURE_2D, 0);
      GetGLError();
    }

    void FBOProxy::setFBOSize(int w, int h)
    {
      if(hasDepth || !ready)
        return;


      if(w != width || h != height)
      {
        width = w;
        height = h;

        //Avoid large FBO's - crashes intel drivers
        if (width > 2048 || height > 2048)
        {
          std::cout << "Requested FBO size is greater than 2048.  Halving the dimensions to prevent crash on intel Drivers" << std::endl;
          width /= 2;
          height /= 2;
        }
        GLUtil::resizeFBO(*this);
//        setupFBO(vp[2], vp[3], false, (TextureDataType)colorDataType);
      }
    }

    void FBOProxy::matchFBOSizeToViewport()
    {
      if(hasDepth || !ready)
        return;

      //TODO: Find out if this will cause a pipeline flush?  It is reading state
      //but not necessarily stuff on the GPU.
      GLint vp[4];
      glGetIntegerv(GL_VIEWPORT, vp);

      if((GLuint)vp[2] != width || (GLuint)vp[3] != height)
      {
        width = vp[2];
        height = vp[3];

        //Avoid large FBO's - crashes intel drivers
        if (width > 2048 || height > 2048)
        {
          std::cout << "Requested FBO size is greater than 2048.  Halving the dimensions to prevent crash on intel Drivers" << std::endl;
          width /= 2;
          height /= 2;
        }
        GLUtil::resizeFBO(*this);
//        setupFBO(vp[2], vp[3], false, (TextureDataType)colorDataType);
      }
    }

    void FBOProxy::bindFBO()
    {
      glGetIntegerv(GL_VIEWPORT, cachedViewport);
      glBindFramebuffer(GL_FRAMEBUFFER, framebufferID);
      GetGLError();
      glViewport(0,0, width, height);
      GetGLError();
      //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      //glActiveTexture(GL_TEXTURE0);
      //glEnable(GL_TEXTURE_2D);
      //GetGLError();
//      if(numColorAttachments > 1)
//      {
//       uint32_t buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
//       glDrawBuffers(4,(GLenum*)buffers);
//        GetGLError();
//      }

    }

    void FBOProxy::unbindFBO()
    {
      //Go back to default framebuffer
      GetGLError();
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      GetGLError();
      glViewport(cachedViewport[0], cachedViewport[1],
                 cachedViewport[2], cachedViewport[3]);
      GetGLError();
      //is this needed? - NO
      /*
      if(numColorAttachments > 1)
      {
        //return to default state
        GLenum buffers[] = { GL_BACK_LEFT };
        glDrawBuffers(1, buffers);
        GetGLError();
      }
       */
    }

    void FBOProxy::setupFBO(int pwidth, int pheight, bool create,
                            GLenum internalFormat,
                            GLenum format,
                            TextureDataType dataType,
                            int numTargets)
    {
      numColorAttachments = numTargets;
      width = pwidth;
      height = pheight;
      colorDataType = dataType;
      colorInternalFormat = internalFormat;
      colorFormat = format;
      hasDepth = false;
      if(numColorAttachments == 1)
      {
        GLUtil::setupFBO(*this, create);
      }
      else if(numColorAttachments > 1 &&
              numColorAttachments <= ATTACHMENT::MAX_COLOR_ATTACHMENTS)
      {
        GLUtil::setupMRT(*this, create);
      }
      ready = true;
    }

    void FBOProxy::setupDepthFBO(int pwidth, int pheight)
    {
      width = pwidth;
      height = pheight;
      hasDepth = true;
      GLUtil::setupDepthFBO(*this);
    }

    void FBOProxy::clear(Color c)
    {
      bindFBO();
      float cc[4] = {0, 0, 0, 1}; // defaults
      glGetFloatv(GL_COLOR_CLEAR_VALUE, cc);
      glClearColor(c.r, c.g, c.b, c.a);
      glViewport(0,0,width, height);
      glDepthMask(GL_TRUE);
      glClearDepth(1.0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(cc[0], cc[1], cc[2], cc[3]);
      unbindFBO();
    }

    bool MeshBufferInfo::constructBuffer(void* pointerToBuffer, int numBytes)
    {
      // make and bind the VAO
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);
      GetGLError();

      // make and bind the VBO
      glGenBuffers(1, &vbo);
      glBindBuffer(bufferType, vbo);

      GetGLError();
      // Put the verticies into the VBO
      glBufferData(bufferType,
                   numBytes,//mesh.getVertexListSizeInBytes(),
                   pointerToBuffer,//&mesh._verts[0].position.x,
                   memTransferBehavior);

      GetGLError();
//      std::cout << "Done setting up buffers for mesh" << std::endl;
      return true;
    }

    bool MeshBufferInfo::updateBuffer(void* pointerToBuffer, int numBytes, bool expand)
    {
      glBindBuffer(bufferType, vbo);
      if (expand == false){
        glBufferSubData(bufferType,
          0,
          numBytes,
          pointerToBuffer
          );
      }
      else{
        glBufferData(bufferType,
          numBytes,//mesh.getVertexListSizeInBytes(),
          pointerToBuffer,//&mesh._verts[0].position.x,
          memTransferBehavior);
      }
      return true;
    }

    bool MeshBufferInfo::constructBuffer(void* pointerToBuffer, int numBytes,
                                         void* pointerToIndices, int numBytesIndices)
    {
      hasIndices = true;
      // make and bind the VAO
      glGenVertexArrays(1, &vao);
      glBindVertexArray(vao);
      GetGLError();

      // make and bind the VBO
      glGenBuffers(1, &vbo);
      glBindBuffer(bufferType, vbo);

      GetGLError();
      // Put the verticies into the VBO
      glBufferData(bufferType,
                   numBytes,//mesh.getVertexListSizeInBytes(),
                   pointerToBuffer,//&mesh._verts[0].position.x,
                   memTransferBehavior);

      GetGLError();
      //      std::cout << "Done setting up buffers for mesh" << std::endl;

      glGenBuffers(1, &ibo);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

      GetGLError();
      // Put the indices into the IBO
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   numBytesIndices,//mesh.getVertexListSizeInBytes(),
                   pointerToIndices,//&mesh._verts[0].position.x,
                   memTransferBehavior);

      GetGLError();
      return true;
    }

    bool MeshBufferInfo::updateBuffer(void* pointerToBuffer, int numBytes,
                                      void* pointerToIndices, int numBytesIndices, bool expand)
    {
      glBindBuffer(bufferType, vbo);
      if (expand == false){
        glBufferSubData(bufferType,
          0,
          numBytes,
          pointerToBuffer
          );
      }
      else{
        glBufferData(bufferType,
          numBytes,
          pointerToBuffer,
          memTransferBehavior);
      }

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      if (expand == false){
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,
          0,
          numBytesIndices,
          pointerToIndices
          );
      }
      else{
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
          numBytesIndices,
          pointerToIndices,
          memTransferBehavior);
      }
      return true;
    }

    bool MeshBufferInfo::bindAttributesToVAO(const std::vector<VertexAttributeInfo> & attributes, Shader& program)
    {
      cachedAttributes.clear();
      glBindVertexArray(vao);
      glBindBuffer( bufferType, vbo);

      bindAttributes(attributes,program);

      glBindVertexArray(0);
      //disableVertexAttributes();

      return true;
    }

    bool MeshBufferInfo::bindAttributesToVAO(const std::vector<VertexAttributeInfo> & attributes)
    {
      cachedAttributes.clear();
      glBindVertexArray(vao);
      glBindBuffer(bufferType, vbo);

      bindAttributes(attributes);

      glBindVertexArray(0);
      //disableVertexAttributes();

      return true;
    }

    void MeshBufferInfo::bindAttributes(const std::vector<VertexAttributeInfo> & attributes,
                                            Shader& program)
    {

      for(VertexAttributeInfo vai :attributes)
      {
        if(vai.location < 0 || !program.isSpecifyingAttributeLocations())
        {
          std::cout << vai.name << " attribute location wasn't pre-bound.... looking it up"
                      << std::endl;
          vai.location  = program.getAttributeLocation(vai.name);
          if(vai.location == -1)
          {
            std::cout << vai.name << " attribute isn't used or declared. (location == -1)" << std::endl;
            continue;
          }
          else
          {
            std::cout << vai.name << " attribute location: " << vai.location << std::endl;
          }
        }
        cachedAttributes.push_back(vai);
        GetGLError();
        glEnableVertexAttribArray(vai.location);
        GetGLError();
        glVertexAttribPointer(vai.location,
                              vai.count_of_memory_type,
                              vai.memory_type,
                              GL_FALSE,
                              (GLsizei)vai.sizeOfVertex,
                              vai.buffer_offset);
        GetGLError();
                std::cout << "Setup " << vai.name << " Slot = "
                << vai.location << std::endl;
      }

    }

    void MeshBufferInfo::bindAttributes(const std::vector<VertexAttributeInfo> & attributes)
    {

      for (VertexAttributeInfo vai : attributes)
      {

        std::cout << "prebinding attribute " << vai.name << " to location: " << vai.location << std::endl;
        cachedAttributes.push_back(vai);
        GetGLError();
        glEnableVertexAttribArray(vai.location);
        GetGLError();
        glVertexAttribPointer(vai.location,
          vai.count_of_memory_type,
          vai.memory_type,
          GL_FALSE,
          (GLsizei)vai.sizeOfVertex,
          vai.buffer_offset);
        GetGLError();
      }
    }

    void MeshBufferInfo::disableVertexAttributes()
    {
      for (VertexAttributeInfo vai : cachedAttributes)
      {
        if (vai.location == -1) continue;
        glDisableVertexAttribArray(vai.location);
        GetGLError();
      }
    }

    void MeshBufferInfo::enableVertexAttributes()
    {
      for(VertexAttributeInfo vai : cachedAttributes)
      {
        if(vai.location == -1) continue;
        glEnableVertexAttribArray(vai.location);
        GetGLError();
        glVertexAttribPointer(vai.location,
                              vai.count_of_memory_type,
                              vai.memory_type,
                              GL_FALSE,
                              sizeOfVertex,//sizeof(Vertex),
                              vai.buffer_offset);
        GetGLError();
      }
    }

    void MeshBufferInfo::drawBuffer(uint32_t startIndex, uint32_t numVertices)
    {

      if(hasIndices)
      {
        glBindVertexArray(vao);
        //glBindBuffer( bufferType, vbo);
        //glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo); //Might need to actually call this on mac...
        //enableVertexAttributes();
        glDrawElements(primitiveType, numIndices, indiceType, BUFFER_OFFSET(0));
      }
      else
      {
        glBindVertexArray(vao);
        //glBindBuffer( bufferType, vbo);
        //enableVertexAttributes();
        glDrawArrays( primitiveType, startIndex, numVertices);
      }
    }

    GLUtil::GLUtil()
    {
    }
    std::string GLUtil::getVersionString()
    {
        float  glLanguageVersion;

        printf("GL_SHADING_LANGUAGE_VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
        sscanf((char *)glGetString(GL_SHADING_LANGUAGE_VERSION), "%f", &glLanguageVersion);
        if(glLanguageVersion == 0.0f)
        {
          sscanf((char *)glGetString(GL_SHADING_LANGUAGE_VERSION), "OpenGL GLSL %f", &glLanguageVersion);
        }

        // GL_SHADING_LANGUAGE_VERSION returns the version standard version form
        //  with decimals, but the GLSL version preprocessor directive simply
        //  uses integers (thus 1.10 should 110 and 1.40 should be 140, etc.)
        //  We multiply the floating point number by 100 to get a proper
        //  number for the GLSL preprocessor directive
        GLuint version = (GLuint)(100 * glLanguageVersion);
        std::stringstream versionString;
        versionString << "#version " << version << " \n";

        return versionString.str();
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


        GLint logLength;
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength > 1)
        {
          std::cout << "Compiler Message(s) for " << name << std::endl;
          GLchar* messages = new GLchar[logLength + 1];
          glGetShaderInfoLog(shaderHandle, logLength, 0, messages);
          std::cout << messages << std::endl;
          delete [] messages;
        }

        GLint compileSuccess;
        glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &compileSuccess);
        if(compileSuccess == GL_FALSE)
        {
            std::cout << "Shader " << name << " failed to compile" << std::endl;
            return -1;
        }
        else
        {
//            printf("Successfully compiled shader (%d) %s!\n",shaderHandle, name.c_str());
            std::cout << "Successfully compiled shader (" << shaderHandle << ") " << name << std::endl;
        }
        return shaderHandle;
    }

    bool GLUtil::recompileShader(ShaderInfo & shaderInfo)
    {
      GLuint tempShader = glCreateShader(shaderInfo.shaderType);
      const char* src = shaderInfo.source.c_str();
      glShaderSource(tempShader,1, &src, 0);
      glCompileShader(tempShader);


      GLint logLength;
      glGetShaderiv(tempShader, GL_INFO_LOG_LENGTH, &logLength);
      if (logLength > 1)
      {
        std::cout << "Compiler Message(s) for " << shaderInfo.filename << std::endl;
        GLchar* messages = new GLchar[logLength + 1];
        glGetShaderInfoLog(tempShader, logLength, 0, messages);
        std::cout << messages << std::endl;
        delete [] messages;
      }

      GLint compileSuccess;
      glGetShaderiv(tempShader, GL_COMPILE_STATUS, &compileSuccess);

      if(compileSuccess == GL_FALSE)
      {
          std::cout << "Shader " << shaderInfo.filename << " failed to compile" << std::endl;
          return false;
      }
      else
      {
        glDeleteShader(shaderInfo.glID);
        shaderInfo.glID = tempShader;
        //printf("Successfully re-compiled shader (%d) %s!\n",shaderInfo.glID, shaderInfo.filename.c_str());
        std::cout << "Successfully re-compiled shader (" << shaderInfo.glID << ") " << shaderInfo.filename << std::endl;
      }
      return true;
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

        if(gsSource.length() > 0)
        {
            std::string gsSourceVersioned = verString + gsSource;
            GLuint geometryShader = compileShader("geometry shader",gsSourceVersioned,
                                                GL_GEOMETRY_SHADER);
            glAttachShader(programHandle, geometryShader);
        }


        return programHandle;
    }

    bool GLUtil::relinkAndVerifyProgram(ProgramInfo& programInfo)
    {

        glDeleteProgram(programInfo.glID);
        programInfo.glID = glCreateProgram();

        for(ShaderInfo* shader : programInfo.shaders)
        {
          glAttachShader(programInfo.glID, shader->glID);
        }

        return (linkAndVerifyProgram(programInfo.glID, &programInfo) > 0);
    }

    bool GLUtil::recreateProgramAndAttachShaders(ProgramInfo& programInfo)
    {

        glDeleteProgram(programInfo.glID);
        programInfo.glID = glCreateProgram();

        for(ShaderInfo* shader : programInfo.shaders)
        {
          glAttachShader(programInfo.glID, shader->glID);
        }

        return true;
    }

    GLuint GLUtil::linkAndVerifyProgram(GLuint programHandle, ProgramInfo* programInfo)
    {
        glLinkProgram(programHandle);

        GLint linkSuccess;
        glGetProgramiv(programHandle, GL_LINK_STATUS, &linkSuccess);
        if(linkSuccess == GL_FALSE)
        {
            //printf("Error(s) in program:\n");
            std::cerr << "Error(s) in program:" << std::endl;
            GLchar messages[256];
            glGetProgramInfoLog(programHandle, sizeof(messages), 0, &messages[0]);
            if(programInfo != nullptr)
            {
//              printf("shaders in program: \n");
              std::cerr << "shaders in program:" << std::endl;
              for(ShaderInfo* si : programInfo->shaders)
              {
                //printf("glID: %d, name: %s, source length: %d \n", si->glID, si->filename.c_str(), (int)si->source.size());
                std::cerr << "glID: " << si->glID << ", name: " << si->filename << ", source length: " << si->source.size() << std::endl;
              }
            }
//            printf("%s\n", messages);
            std::cerr << "" << messages << std::endl;
            //exit(1);
        }
        else
        {
            //printf("Successfully created shader program!\n");
        }


        /*
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
        */


        printActiveAttributes(programHandle);
        printActiveUniforms(programHandle);
        return programHandle;
    }

    void GLUtil::printActiveUniforms(GLuint programHandle)
    {
        GLint uniformCount;
        glGetProgramiv(programHandle, GL_ACTIVE_UNIFORMS, &uniformCount);

        //printf("Active Uniform Count: %d\n", uniformCount);
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
                                   (GLsizei)nameData.size(),
                                   &actualLength,
                                   &arraySize,
                                   &type,
                                   &nameData[0]
                                   );
                std::string name((char*)&nameData[0], actualLength);
//                printf("Uniform %d name: %s, type:%s\n", uniformID, name.c_str(),
//                       GLUtil::glEnumToString(type).c_str());
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

//        printf("Active Uniform Count: %d\n", uniformCount);
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
                                   (GLsizei)nameData.size(),
                                   &actualLength,
                                   &arraySize,
                                   &type,
                                   &nameData[0]
                                   );
                std::string name((char*)&nameData[0], actualLength);
                ShaderUniformData sud(name, type, uniformID);
                std::pair<
                std::map< std::string, ShaderUniformData>::iterator,
                bool
                > res;
                res = dict->insert(std::make_pair(name, sud));
                if(res.second == false)
                {
//                    fprintf(
//                            stderr,
//                            "Can't insert key '%s' into uniform dict, it already exists.\n",
//                            name.c_str()
//                            );
                    std::cerr << "Can't insert key " << name << " into uniform dict, it already exists." << std::endl;
                }
            }
        }
    }

    void GLUtil::printActiveAttributes(GLuint programHandle)
    {
        GLint attributeCount;
        glGetProgramiv(programHandle, GL_ACTIVE_ATTRIBUTES, &attributeCount);
//        printf("Active Attribute Count: %d\n", attributeCount);
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
                                  (GLsizei)nameData.size(),
                                  &actualLength,
                                  &arraySize,
                                  &type,
                                  &nameData[0]
                                  );
                std::string name((char*)&nameData[0], actualLength);
//                printf("Attribute %d name: %s, type:%s\n", attrib, name.c_str(),
//                       GLUtil::glEnumToString(type).c_str());
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
//        printf("Active Attribute Count: %d\n", attributeCount);
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
                                  (GLsizei)nameData.size(),
                                  &actualLength,
                                  &arraySize,
                                  &type,
                                  &nameData[0]
                                  );
                std::string name((char*)&nameData[0], actualLength);
                ShaderAttributeData sd(name, type, attrib);
//                printf("Attribute %d name: %s, type:%s\n", attrib, name.c_str(),
//                       GLUtil::glEnumToString(type).c_str());
                std::pair<
                std::map< std::string, ShaderAttributeData>::iterator,
                bool
                > res;
                res = dict->insert(std::make_pair(name, sd));
                if(res.second == false)
                {
//                    fprintf(
//                            stderr,
//                            "Can't insert key '%s' into attribute dict, it already exists.\n",
//                            name.c_str()
//                            );
                    std::cerr << "Can't insert key " << name << " into attribute dict, it already exists." << std::endl;
                }
            }
        }
    }

    void GLUtil::bindAttributeLocation(GLuint program, GLuint index, const char* name)
    {
      glBindAttribLocation(program, index, name);
      GetGLError();
    }

    void GLUtil::bindColorLocation(GLuint program, GLuint index, const char* name)
    {
      glBindFragDataLocation(program, index, name);
      GetGLError();
    }

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
            //std::cout << filename << std::endl << contents << std::endl; std::cout.flush();
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

        //Buffer enums
        case GL_ARRAY_BUFFER:
          str = "GL_ARRAY_BUFFER";
          break;
        case GL_ELEMENT_ARRAY_BUFFER:
          str = "GL_ELEMENT_ARRAY_BUFFER";
          break;
        case GL_STATIC_DRAW:
          str = "GL_STATIC_DRAW";
          break;
        case GL_DYNAMIC_DRAW:
          str = "GL_DYNAMIC_DRAW";
          break;
        case GL_STREAM_DRAW:
          str = "GL_STREAM_DRAW";
          break;
      }

      return str;
    }



  void GLUtil::setupMRT(FBOProxy& fbo, bool create)
  {
    fbo.numColorAttachments = (int)FBOProxy::ATTACHMENT::MAX_COLOR_ATTACHMENTS;
    fbo.hasDepth = false;
    fbo.hasStencil = false;
    // 1. Init Color Texture
    if(create){
      glGenTextures(fbo.numColorAttachments,fbo.colorTextureID);
    }

    for(int i = 0; i < fbo.numColorAttachments; i++)
    {
      glBindTexture( GL_TEXTURE_2D, fbo.colorTextureID[i]);
      GetGLError();
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexImage2D( GL_TEXTURE_2D, 0, fbo.colorInternalFormat, fbo.width, fbo.height, 0, fbo.colorFormat,
                   fbo.colorDataType, NULL);
      GetGLError();
    }
    // 2. Init Render Buffer
    if(create){
      glGenRenderbuffers(1, &fbo.renderbufferID);
    }
    glBindRenderbuffer( GL_RENDERBUFFER, fbo.renderbufferID);
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, fbo.width, fbo.height);
    GetGLError();

    // 3. Init Frame Buffer
    if(create){
      glGenFramebuffers(1, &fbo.framebufferID);
    }
    glBindFramebuffer( GL_FRAMEBUFFER, fbo.framebufferID);

    for(int i = 0; i < fbo.numColorAttachments; i++)
    {
      glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D,
                              fbo.colorTextureID[i], 0);
    }
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_RENDERBUFFER, fbo.renderbufferID);
    uint32_t buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
    glDrawBuffers(4,(GLenum*)buffers);

    GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(stat != GL_FRAMEBUFFER_COMPLETE){
      std::cerr<< "FBO with MRT Not Complete" << std::endl;
    }

    //4. zero it out! (Sometimes the buffer won't have all zeroes)
    float clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    // 5. Cleanup
    glBindTexture(GL_TEXTURE_2D, NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, NULL);
    glBindFramebuffer( GL_FRAMEBUFFER, NULL);
    GetGLError();

  };

  void GLUtil::resizeFBO(FBOProxy& fbo)
  {
    glBindTexture( GL_TEXTURE_2D, fbo.colorTextureID[0]);
    glTexImage2D( GL_TEXTURE_2D, 0, fbo.colorInternalFormat, fbo.width, fbo.height, 0, fbo.colorFormat,
                 fbo.colorDataType, NULL);
    GetGLError();

    glBindRenderbuffer(GL_RENDERBUFFER, fbo.renderbufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, fbo.width, fbo.height);
    GetGLError();
    //4. zero it out! (Sometimes the buffer won't have all zeroes)
    float clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    // 5. Cleanup
    glBindTexture(GL_TEXTURE_2D, NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, NULL);
    glBindFramebuffer( GL_FRAMEBUFFER, NULL);
  };

  void GLUtil::setupFBO(FBOProxy& fbo, bool create)
  {
    fbo.numColorAttachments = 1;
    fbo.hasDepth = false;
    fbo.hasStencil = false;
    // 1. Init Color Texture
    if(create){
      glGenTextures(1,&fbo.colorTextureID[0]);
    }
    glBindTexture( GL_TEXTURE_2D, fbo.colorTextureID[0]);
    GetGLError();
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D( GL_TEXTURE_2D, 0, fbo.colorInternalFormat, fbo.width, fbo.height, 0, fbo.colorFormat,
                 fbo.colorDataType, NULL);
    GetGLError();

    // 2. Init Render Buffer
    if(create){
      glGenRenderbuffers(1, &fbo.renderbufferID);
    }
    glBindRenderbuffer( GL_RENDERBUFFER, fbo.renderbufferID);
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, fbo.width, fbo.height);
    GetGLError();

    // 3. Init Frame Buffer
    if(create){
      glGenFramebuffers(1, &fbo.framebufferID);
    }
    glBindFramebuffer( GL_FRAMEBUFFER, fbo.framebufferID);
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                            fbo.colorTextureID[0], 0);
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_RENDERBUFFER, fbo.renderbufferID);

    GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(stat != GL_FRAMEBUFFER_COMPLETE){
      std::cerr<< "FBO Not Complete" << std::endl;
    }

    //4. zero it out! (Sometimes the buffer won't have all zeroes)
    float clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    // 5. Cleanup
    glBindTexture(GL_TEXTURE_2D, NULL);
    glBindRenderbuffer(GL_RENDERBUFFER, NULL);
    glBindFramebuffer( GL_FRAMEBUFFER, NULL);

  };

  void GLUtil::setupDepthFBO(FBOProxy& fbo)
  {
    fbo.numColorAttachments = 1;
    fbo.hasDepth = true;
    fbo.hasStencil = false;
    // 1. Init Color Texture
    glGenTextures(1,&fbo.colorTextureID[0]);
    glBindTexture( GL_TEXTURE_2D, fbo.colorTextureID[0]);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, fbo.width, fbo.height, 0, GL_RGBA,
                 fbo.colorDataType, NULL);

    // 2. Create depth texture
    glGenTextures(1,&fbo.depthTextureID);
    glBindTexture( GL_TEXTURE_2D, fbo.depthTextureID);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //TODO: is GL_UNSIGNED_SHORT correct? Check render to depth examples!
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fbo.width, fbo.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);

    // 3. Init Frame Buffer
    glGenFramebuffers(1, &fbo.framebufferID);
    glBindFramebuffer( GL_FRAMEBUFFER, fbo.framebufferID);
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           fbo.colorTextureID[0], 0);
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, fbo.depthTextureID, 0);

    GLenum stat = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(stat != GL_FRAMEBUFFER_COMPLETE){
      std::cerr<< "FBO Not Complete" << std::endl;
    }

    //4. zero it out! (Sometimes the buffer won't have all zeroes)
    float clearColor[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, clearColor);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

    // 5. Cleanup
    glBindTexture(GL_TEXTURE_2D, NULL);
    glBindFramebuffer( GL_FRAMEBUFFER, NULL);

  };

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

#if __APPLE__
        va_start(a, condition);
        pStr = va_arg(a, const char*);
        fatalError(pStr, a);
#endif
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
        GetGLError();
        glBindVertexArray(*vao);
        GetGLError();

        glGenBuffers(1, vbo);
        glBindBuffer(GL_ARRAY_BUFFER, *vbo);
        GetGLError();
        glBufferData(GL_ARRAY_BUFFER, sizeof(p), p, GL_STATIC_DRAW);
        GetGLError();

        GLint vertLoc = glGetAttribLocation(prog, "Position");
//        printf("position attribute at: %d\n", vertLoc);
        GetGLError();
        glBindBuffer(GL_ARRAY_BUFFER, *vbo);
        GetGLError();
        glVertexAttribPointer(vertLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
        GetGLError();
        glEnableVertexAttribArray(vertLoc);
        GetGLError();
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

    /*
    void SetUniform(const char* name, glm::mat4 value)
    {
        GLuint program;
        glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
        GLint location = glGetUniformLocation(program, name);
        glUniformMatrix4fv(location, 1, 0, glm::value_ptr(value));
    }

    void SetUniform(const char* name, glm::mat3 nm)
    {
        GLuint program;
        glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
        GLint location = glGetUniformLocation(program, name);
//         float packed[9] = {
//         nm.getRow(0).getX(), nm.getRow(1).getX(), nm.getRow(2).getX(),
//         nm.getRow(0).getY(), nm.getRow(1).getY(), nm.getRow(2).getY(),
//         nm.getRow(0).getZ(), nm.getRow(1).getZ(), nm.getRow(2).getZ() };
        glUniformMatrix3fv(location, 1, 0, glm::value_ptr(nm));
    }

    void SetUniform(const char* name, glm::vec3 value)
    {
        GLuint program;
        glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
        GLint location = glGetUniformLocation(program, name);
        glUniform3f(location, value.x, value.y, value.z);
    }

    void SetUniform(const char* name, glm::vec4 value)
    {
        GLuint program;
        glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
        GLint location = glGetUniformLocation(program, name);
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }
    */

    void SetUniform(const char* name, float x, float y)
    {
        GLuint program;
        glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
        GLint location = glGetUniformLocation(program, name);
        glUniform2f(location, x, y);
    }

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

    void SetUniform(const char* name, Vector2 value)
    {
        GLuint program;
        glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
        GLint location = glGetUniformLocation(program, name);
        glUniform2f(location, value.getX(), value.getY());
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

    void GLUtil::enableAnisotropicFiltering(float pctMax)
    {
      GLfloat fLargest;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &fLargest);
      GLfloat p = fLargest*pctMax;
      p = p<1.0f?1.0f:p;
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, p);
    }

  std::string GLUtil::getCubeMapFilenameForTarget(
                                          std::string basename,
                                          std::string fileExtension,
                                          GLenum target
                                          )
  {
    std::stringstream path;
    path << basename;
    switch (target) {
      case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
        path << "_xpos" << fileExtension;
        break;
      case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
        path << "_xneg" << fileExtension;
        break;
      case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
        path << "_ypos" << fileExtension;
        break;
      case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
        path << "_yneg" << fileExtension;
        break;
      case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
        path << "_zpos" << fileExtension;
        break;
      case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
        path << "_zneg" << fileExtension;
        break;
      default:
        break;
    }


    return path.str();

  }

  GLuint GLUtil::loadRaw3DData(const std::string& filename, int dx, int dy, int dz, TextureProxy& proxy)
  {
    GLuint handle;
    glGenTextures(1, &handle);
	glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, handle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    proxy.target = GL_TEXTURE_3D;
    proxy.internalFormat = GL_RED;
    proxy.width = dx;
    proxy.height = dy;
    proxy.depth = dz;
    proxy.format = GL_RED;
    proxy.pixels = nullptr;
    proxy.glID = handle;

    int position = 0;
    std::ifstream fin(filename.c_str(), std::ios::in | std::ios::binary);

    int array_size = dx*dy*dz;
    if (proxy.type == TDT_USHORT)
    {
      unsigned short *data = new unsigned short[array_size];
      size_t chunkSize = sizeof(unsigned short)* 512;
      while (fin.read((char*)&data[position], chunkSize))
        position += 512;
      fin.close();
      glTexImage3D(proxy.target, 0,
        proxy.internalFormat,
        proxy.width, proxy.height, proxy.depth, 0,
        proxy.format,
        proxy.type,
        data);
      delete[] data;
    }
    if (proxy.type == TDT_UBYTE)
    {
      unsigned char *data = new unsigned char[array_size];
      size_t chunkSize = sizeof(unsigned char)* 512;
      while (fin.read((char*)&data[position], chunkSize))
        position += 512;
      fin.close();
      glTexImage3D(proxy.target, 0,
        proxy.internalFormat,
        proxy.width, proxy.height, proxy.depth, 0,
        proxy.format,
        proxy.type,
        data);
      delete[] data;
    }
    if (proxy.type == TDT_FLOAT)
    {
      float *data = new float[array_size];
      size_t chunkSize = sizeof(float)* 512;
      while (fin.read((char*)&data[position], chunkSize))
        position += 512;
      fin.close();
	  proxy.internalFormat = GL_R32F;
      glTexImage3D(proxy.target, 0,
        proxy.internalFormat,
        proxy.width, proxy.height, proxy.depth, 0,
        proxy.format,
        proxy.type,
        data);
      delete[] data;
    }
    
    checkGLErrors();

   
    return handle;
  }

  GLuint GLUtil::createPyroclasticVolume(int n, float r, TextureProxy& proxy)
  {
    GLuint handle;
    glGenTextures(1, &handle);
	glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, handle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    unsigned char *data = new unsigned char[n*n*n];
    unsigned char *ptr = data;

    float frequency = 3.0f / n;
    float center = n / 2.0f + 0.5f;

    for (int z = 0; z < n; ++z) {
      for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
          float dx = center - x;
          float dy = center - y;
          float dz = center - z;

          float off = fabsf((float)PerlinNoise3D(
            x*frequency,
            y*frequency,
            z*frequency,
            5,
            6, 3));

          float d = sqrtf(dx*dx + dy*dy + dz*dz) / (n);
          bool isFilled = (d - off) < r;
          *ptr++ = isFilled ? 255 : 0;
        }
      }
      fprintf(stdout,"Slice %d of %d\n", z, n);
    }
    proxy.target = GL_TEXTURE_3D;
    proxy.internalFormat = GL_R8;
    proxy.width = n;
    proxy.height = n;
    proxy.depth = n;
    proxy.format = GL_RED;
    proxy.type = TextureDataType::TDT_UBYTE;
    proxy.pixels = nullptr;
    proxy.glID = handle;

    glTexImage3D(proxy.target, 0,
      proxy.internalFormat,
      proxy.width, proxy.height, proxy.depth, 0,
      proxy.format,
      proxy.type,
      data);

    checkGLErrors();

    delete[] data;
    return handle;
  }

  GLuint GLUtil::createPyroclasticDistanceField(int n, float r, float strength, TextureProxy& proxy)
  {
    GLuint handle;
    glGenTextures(1, &handle);
	glEnable(GL_TEXTURE_3D);
    glBindTexture(GL_TEXTURE_3D, handle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //float *data = new float[n*n*n];
	glm::vec4 * data = new glm::vec4[n*n*n];
    //float *ptr = data;
    glm::vec4 *ptr = data;

    float frequency = 3.0f / n;
    float center = n / 2.0f + 0.5f;

    for (int z = 0; z < n; ++z) {
      for (int y = 0; y < n; ++y) {
        for (int x = 0; x < n; ++x) {
          float dx = x - center;
          float dy = y - center;
          float dz = z - center;

          
          float off = fabsf((float)PerlinNoise3D(
            x*frequency,
            y*frequency,
            z*frequency,
            5,
            6, 3));
          

		  float d = sqrtf(dx*dx + dy*dy + dz*dz) + off*strength;
          float dd = d/(n*0.5f);
		  //Storing distance
      //*ptr++ = dd  - r;
      (*ptr).w = dd  - r;
		  //Storing normal too
		  (*ptr).x = dx / d;
		  (*ptr).y = dy / d;
      (*ptr).z = dz / d;
		  *ptr++;
        }
      }
      fprintf(stdout,"Slice %d of %d\n", z, n);
    }
	/*
    proxy.target = GL_TEXTURE_3D;
    proxy.internalFormat = GL_R32F;
    proxy.width = n;
    proxy.height = n;
    proxy.depth = n;
    proxy.format = GL_RED;
    proxy.type = TextureDataType::TDT_FLOAT;
    proxy.pixels = nullptr;
    proxy.glID = handle;
	*/
    proxy.target = GL_TEXTURE_3D;
    proxy.internalFormat = GL_RGBA32F;
    proxy.width = n;
    proxy.height = n;
    proxy.depth = n;
    proxy.format = GL_RGBA;
    proxy.type = TextureDataType::TDT_FLOAT;
    proxy.pixels = nullptr;
    proxy.glID = handle;

    glTexImage3D(proxy.target, 0,
      proxy.internalFormat,
      proxy.width, proxy.height, proxy.depth, 0,
      proxy.format,
      proxy.type,
      data);

    checkGLErrors();

    delete[] data;
    return handle;
  }
  
  GLuint GLUtil::createCubemap(std::string filenameBase, std::string fileExtension,
                       TextureProxy& proxy)
    {
      GLuint textureID;
      glGenTextures(1, &textureID);
      glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
      glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

      TextureProxy tp[6];
      tp[0].target = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
      tp[1].target = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
      tp[2].target = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
      tp[3].target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
      tp[4].target = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
      tp[5].target = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

      for(int i = 0; i < 6; i++)
      {
        std::string path = getCubeMapFilenameForTarget(filenameBase,
                                                       fileExtension,
                                                       tp[i].target);
        read_png_file(path.c_str(), tp[i], true);

        glTexImage2D(tp[i].target, 0, tp[i].internalFormat, tp[i].width,
                   tp[i].height, 0, tp[i].format, tp[i].type, tp[i].pixels.get());
        GetGLError();

      }

      proxy = tp[0];
      proxy.target = GL_TEXTURE_CUBE_MAP;
      proxy.pixels = nullptr;
      proxy.glID = textureID;


      return textureID;

    }
  void GLUtil::clear(bool color, bool depth, bool stencil)
  {
          GLbitfield mask = 0x0;
          if (color) mask |= GL_COLOR_BUFFER_BIT;
          if (depth) mask |= GL_DEPTH_BUFFER_BIT;
          if (stencil) mask |= GL_STENCIL_BUFFER_BIT;
          glClear(mask);
  }
} //namespace renderlib
