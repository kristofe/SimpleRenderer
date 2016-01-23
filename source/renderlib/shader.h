#ifndef SHADER_H
#define SHADER_H

#include "OpenGLHelper.h"
#include "ShaderManager.h"
#include "ObjectIDGenerator.h"
#include "allmath.h"
#include <map>
#include <tuple>
#include <string>

namespace renderlib{

class Shader
{
	class ShaderSourceInfo{
	public:
	  std::string  name;
	  ShaderType shaderType;
	  bool immutable;

  private:
	  std::string  source;
	  std::string  filename;

	public:
	  ShaderSourceInfo(std::string pName, ShaderType type)
		  : name(pName), shaderType(type), immutable(false), source(""), filename("") {};

	  void setSource(std::string s, bool dontReload = true)
	  {
		  source = s;
		  immutable = dontReload;
		  filename = "";
	  }

	  void setSourceFile(std::string path, bool dontReload = false)
	  {
		  source = "";
		  immutable = dontReload;
		  filename = path;
	  }


	  std::string getSource() { return source; };
	  std::string getFilename() { return filename; };

	  void copyFrom(const ShaderSourceInfo& other)
	  {
		  this->name = other.name;
		  this->source = other.source;
		  this->filename = other.filename;
		  this->shaderType = other.shaderType;
		  this->immutable = other.immutable;
	  }

	  ShaderSourceInfo(const ShaderSourceInfo& other)
	  {
		  copyFrom(other);
	  }

	  void operator=(const ShaderSourceInfo& other)
	  {
		  copyFrom(other);
	  }
    
  private:
    ShaderSourceInfo() {};
  };
//Methods
public:
  Shader();
 
  Shader(std::string name):_name(name){};
  virtual ~Shader();

  void generateName(std::string baseName);
  
  void registerShader(std::string filename, ShaderType type)
  {
	  ShaderSourceInfo ssi(filename, type);
	  ssi.setSourceFile(filename);
    _shaderSourceInfoList.push_back(ssi);
  }

  void registerShaderSource(std::string source, ShaderType type, std::string name)
  {
	  ShaderSourceInfo ssi(name, type);
	  ssi.setSource(source);
    _shaderSourceInfoList.push_back(ssi);
  }

  void registerShaderSourceInfo(ShaderSourceInfo ssi)
  {
	  _shaderSourceInfoList.push_back(ssi);
  }
  
  void setAttributeLocations(const std::vector<ShaderAttributeData>& attribs)
  {
    _specifyingAttributeLocations = true;
    for(ShaderAttributeData sad : attribs)
    {
      _attributeLocations[sad.name] = sad;
    }
  }
  
  void setColorLocations(const std::vector<ShaderColorData>& colors)
  {
    _specifyingColorLocations = true;
    for(ShaderColorData cd : colors)
    {
      _colorLocations[cd.name] = cd;
    }
  }

  bool compileShaders();
  bool linkShaders();
  GLuint getUniformLocation(const std::string& name);
  GLuint getAttributeLocation(const std::string& name);
  GLuint getColorLocation(const std::string& name);
  void enableVertexAttributes();
  void disableVertexAttributes();
  virtual void reset();
  void bind();
  void unbind();

  inline GLuint getID()
  {
    if(_programInfo != NULL)
      return _programInfo->glID;

     return 0;
  }
  
  bool isSpecifyingAttributeLocations() { return _specifyingAttributeLocations;}
  bool isSpecifyingColorLocations() { return _specifyingColorLocations;}
  const std::map<std::string, ShaderAttributeData>& getAttributeLocations()
  {
    return _attributeLocations;
  };
  const std::map<std::string, ShaderColorData>& getColorLocations()
  {
    return _colorLocations;
  };
  
  void setUniform(const char* name, int value);
  void setUniform(const char* name, float value);
  void setUniform(const char* name, float x, float y);
  void setUniform(const char* name, Matrix4 value);
  void setUniform(const char* name, Matrix3 nm);
  void setUniform(const char* name, Vector2 value);
  void setUniform(const char* name, Vector3 value);
  void setUniform(const char* name, Vector4 value);
  void setUniform(const char* name, Point3 value);
  void setUniform(const char* name, Color value);
  void setUniform(const char* name, const glm::mat4& value);
  void setUniform(const char* name, const glm::mat3& nm);
  void setUniform(const char* name, const glm::vec2& value);
  void setUniform(const char* name, const glm::vec3& value);
  void setUniform(const char* name, const glm::vec4& value);
  

private:
  //C++ 11 way of hiding these methods
  //Default constructor, no copy constructor or copy assignment operato
  Shader(const Shader&) = delete;
  Shader & operator=(const Shader&) = delete;
  

//Data Members

protected:
  std::string _name;
  bool _specifyingAttributeLocations;
  bool _specifyingColorLocations;
  std::vector<ShaderSourceInfo>  _shaderSourceInfoList;
  std::vector<ShaderInfo*>     _shaders;
  ProgramInfo*                 _programInfo;
  std::map<std::string, ShaderAttributeData> _attributeLocations;
  std::map<std::string, ShaderUniformData>   _uniformLocations;
  std::map<std::string, ShaderColorData>   _colorLocations;


};

}
#endif // SHADER_H
