//
//  ShaderManager.h
//  Splash
//
//  Created by Kristofer Schlachter on 6/12/14.
//
//

#ifndef __Splash__ShaderManager__
#define __Splash__ShaderManager__

#include <iostream>
#include <string>
#include <map>
#include <vector>

#include "OpenGLHelper.h"
#include "shader.h"

namespace renderlib{

class ShaderManager{
public:
  static ShaderManager& getInstance()
  {
    // Instantiated on first use.
    if(instance == nullptr)
      instance = new ShaderManager();
    return *instance;
  }


public:
  ShaderInfo* loadShader(std::string filename, ShaderType type, std::string name);
  ShaderInfo* loadShaderFromSource(std::string filename, ShaderType type, std::string name, bool neverReloads);
  ProgramInfo* linkProgram(std::string name,
                          const std::vector<ShaderInfo*>& shaders,
                           Shader* owner);
  void prebindAttributeLocations(GLuint program, Shader* owner);
  void prebindColorLocations(GLuint program, Shader* owner);
  bool reloadShaders();

  ShaderInfo& getShader(std::string& filename);
  ProgramInfo& getProgram(std::string& name);

  void removeShaders(std::vector<ShaderInfo*>& shaders);
  void removeProgram(ProgramInfo* program);

  //C++ 11 way of hiding these methods
  //Default constructor, no copy constructor or copy assignment operato
  //ShaderManager() = default;
  ShaderManager(){};
  ShaderManager(const ShaderManager&) = delete;
  ShaderManager & operator=(const ShaderManager&) = delete;
private:
  static ShaderManager*    instance; // Guaranteed to be destroyed.
  std::string _basePath;
  std::map<std::string, ShaderInfo*> _shaderInfoDB;
  std::map<std::string, ProgramInfo*> _programInfoDB;


};




}//namespace renderlib

#endif /* defined(__Splash__ShaderManager__) */
