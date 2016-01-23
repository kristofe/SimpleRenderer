#include <stdlib.h>
#include "ShaderManager.h"
#include "shader.h"


namespace renderlib{
  Shader::Shader()
  {
  }

  Shader::~Shader()
  {
    //ShaderManager& sm = ShaderManager::getInstance();
    //sm.removeShaders(_shaders);
    //sm.removeProgram(_programInfo);
    //_shaderFileInfoList.clear();
    //_shaders.clear();
    //_attributeLocations.clear();
    //_uniformLocations.clear();
  }

  void Shader::generateName(std::string baseName)
  {
    char buffer[256];
    sprintf(buffer, "Shader_%s_%03d",baseName.c_str(),ObjectIDGenerator::getInstance().getNextID());
    _name = buffer;

  }
  bool Shader::compileShaders()
  {
    ShaderManager& sm = ShaderManager::getInstance();
    bool success = true;;

    for (ShaderSourceInfo ssi : _shaderSourceInfoList)
    {
      ShaderInfo* si;
      if(ssi.getFilename().length() > 0)
      {
        si = sm.loadShader(ssi.getFilename(), ssi.shaderType, ssi.name);
      }
      else{
        si = sm.loadShaderFromSource(ssi.getSource(), ssi.shaderType, ssi.name, ssi.immutable);
      }
      _shaders.push_back(si);
      if(si->glID == 0)
      {
        success = false;
      }
    }
    if(_name.length() == 0)
    {
      generateName("AUTO");
    }

    return success;
  }

  bool Shader::linkShaders()
  {
    ShaderManager& sm = ShaderManager::getInstance();
    _programInfo = sm.linkProgram(_name, _shaders, this);

    GLUtil::getActiveUniforms(_programInfo->glID, &_uniformLocations);

    if(_specifyingAttributeLocations == false)
    {
      GLUtil::getActiveAttributes(_programInfo->glID, &_attributeLocations);
    }


    return _programInfo->glID > 0;
  }

  GLuint Shader::getUniformLocation(const std::string& name)
  {
    std::map<std::string, ShaderUniformData>::iterator it = _uniformLocations.find(name);
    if(it != _uniformLocations.end()){
      return it->second.location;
    }

    GLint loc = glGetUniformLocation(_programInfo->glID, name.c_str());
    if(loc > 0)
      _uniformLocations.at(name).location = loc;
    return loc;
  }

  GLuint Shader::getAttributeLocation(const std::string& name)
  {
    std::map<std::string, ShaderAttributeData>::iterator it = _attributeLocations.find(name);
    if(it != _attributeLocations.end()){
      return it->second.location;
    }

    GLint loc = glGetAttribLocation(_programInfo->glID, name.c_str());
    if(loc > 0)
      _attributeLocations.at(name).location = loc;
    return loc;
  }

  GLuint Shader::getColorLocation(const std::string& name)
  {
    std::map<std::string, ShaderColorData>::iterator it = _colorLocations.find(name);
    if(it != _colorLocations.end()){
      return it->second.location;
    }

    GLint loc = glGetFragDataLocation(_programInfo->glID, name.c_str());
    if(loc > 0)
      _colorLocations.at(name).location = loc;
    return loc;
  }

  void Shader::enableVertexAttributes()
  {
    std::map<std::string, ShaderAttributeData>::iterator it = _attributeLocations.begin();
    while(it != _attributeLocations.end())
    {
      glEnableVertexAttribArray(it->second.location);
      ++it;
    }
  }

  void Shader::disableVertexAttributes()
  {
    std::map<std::string, ShaderAttributeData>::iterator it = _attributeLocations.begin();
    while(it != _attributeLocations.end())
    {
      glDisableVertexAttribArray(it->second.location);
      ++it;
    }
  }
  void Shader::reset()
  {
    _specifyingAttributeLocations = false;
    ShaderManager& sm = ShaderManager::getInstance();
    sm.removeShaders(_shaders);
    sm.removeProgram(_programInfo);
    _shaderSourceInfoList.clear();
    _shaders.clear();
    _attributeLocations.clear();
    _uniformLocations.clear();
  }

  void Shader::bind()
  {
    glUseProgram(_programInfo->glID);
    //enableVertexAttributes();
  }

  void Shader::unbind()
  {
    glUseProgram(0);
    ///disableVertexAttributes();
  }

  void Shader::setUniform(const char* name, int value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform1i(location, value);
  }

  void Shader::setUniform(const char* name, float value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform1f(location, value);
  }

  void Shader::setUniform(const char* name, float x, float y)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform2f(location, x, y);
  }

  void Shader::setUniform(const char* name, Matrix4 value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniformMatrix4fv(location, 1, 0, (float*) &value);
  }

  void Shader::setUniform(const char* name, Matrix3 nm)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    float packed[9] = {
      nm.getRow(0).getX(), nm.getRow(1).getX(), nm.getRow(2).getX(),
      nm.getRow(0).getY(), nm.getRow(1).getY(), nm.getRow(2).getY(),
      nm.getRow(0).getZ(), nm.getRow(1).getZ(), nm.getRow(2).getZ() };
    glUniformMatrix3fv(location, 1, 0, &packed[0]);
  }

  void Shader::setUniform(const char* name, Vector2 value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform2f(location, value.getX(), value.getY());
  }

  void Shader::setUniform(const char* name, Vector3 value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform3f(location, value.getX(), value.getY(), value.getZ());
  }

  void Shader::setUniform(const char* name, Vector4 value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform4f(location, value.getX(), value.getY(), value.getZ(), value.getW());
  }

  void Shader::setUniform(const char* name, Point3 value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform3f(location, value.getX(), value.getY(), value.getZ());
  }

  void Shader::setUniform(const char* name, Color value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform4f(location, value.r, value.g, value.b, value.a);
  }
  
  //////////////////////////////////////////////////////////////////////////////
  void Shader::setUniform(const char* name,const glm::mat4& value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniformMatrix4fv(location, 1, 0, glm::value_ptr(value));
  }
  
  void Shader::setUniform(const char* name,const glm::mat3& nm)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniformMatrix3fv(location, 1, 0, glm::value_ptr(nm));
  }
  
  void Shader::setUniform(const char* name, const glm::vec2& value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform2fv(location, 1, glm::value_ptr(value));
  }
  
  void Shader::setUniform(const char* name, const glm::vec3& value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform3fv(location, 1, glm::value_ptr(value));
  }
  
  void Shader::setUniform(const char* name, const glm::vec4& value)
  {
    GLint location = glGetUniformLocation(_programInfo->glID, name);
    glUniform4fv(location, 1, glm::value_ptr(value));
  }

}
