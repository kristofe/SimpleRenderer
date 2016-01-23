//
//  ShaderManager.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 6/12/14.
//
//

#include "glutil.h"
#include "ShaderManager.h"
#include "md5.h"

namespace renderlib {
  
ShaderManager* ShaderManager::instance = nullptr;
  
ShaderInfo* ShaderManager::loadShader(std::string filename, ShaderType type, std::string name)
{
  std::map<std::string, ShaderInfo *>::iterator it = _shaderInfoDB.find(name);
  if (it != _shaderInfoDB.end()) {
	  std::cout << "ShaderManager::loadShader found a previous copy of " << name << ". returning that instance" << std::endl;
	  return it->second;
  }
  ShaderInfo* info = new ShaderInfo();
  
  info->filename = filename;
  info->shaderType = type;
  info->immutable = false;
  info->name = name;
  info->source = GLUtil::getShaderSource(filename);
  info->hash = md5(info->source);
  info->glID = GLUtil::compileShader(info->name, info->source,
                                    (GLenum)info->shaderType);
  info->changed = false;
  _shaderInfoDB[info->name] = info;
  return info;
}

  ShaderInfo* ShaderManager::loadShaderFromSource(std::string source, ShaderType type, std::string name, bool neverReloads)
{
  std::map<std::string, ShaderInfo *>::iterator it = _shaderInfoDB.find(name);
  if (it != _shaderInfoDB.end()) {
	  std::cout << "ShaderManager::loadShaderFromSource found a previous copy of " << name << ". returning that instance" << std::endl;
	  return it->second;
  }
  ShaderInfo* info = new ShaderInfo();
  
  info->shaderType = type;
  info->source = source;
  info->immutable = neverReloads;
  info->hash = md5(info->source);
  info->name = name;
  info->filename = info->hash;
  info->glID = GLUtil::compileShader(info->name, info->source,
                                     (GLenum)info->shaderType);
  info->changed = false;
  _shaderInfoDB[info->name] = info;
  return info;
}
  
ProgramInfo* ShaderManager::linkProgram(std::string name,
                        const std::vector<ShaderInfo*>& shaders,
                                        Shader* owner)
{
  ProgramInfo* info = new ProgramInfo();
  info->name = name;
  
  info->glID = glCreateProgram();
  for(ShaderInfo* shader: shaders)
  {
    info->shaders.push_back(shader);
    glAttachShader(info->glID, shader->glID);
  }
  if(owner->isSpecifyingAttributeLocations())
  {
    prebindAttributeLocations(info->glID,owner);
  }
  if(owner->isSpecifyingColorLocations())
  {
    prebindColorLocations(info->glID,owner);
  }
  GLUtil::linkAndVerifyProgram(info->glID,info);
  _programInfoDB[name] = info;
  
  info->owner = owner;
  
  return info;
  
}
  
void ShaderManager::prebindAttributeLocations(GLuint program,  Shader* owner)
{
  const std::map<std::string, ShaderAttributeData>& attrLocs
                                            = owner->getAttributeLocations();
  for(std::pair<std::string, ShaderAttributeData> attLoc : attrLocs)
  {
    GLuint index = attLoc.second.location;
    const char* name = attLoc.second.name.c_str();
    GLUtil::bindAttributeLocation(program, index, name);
  }
  
}
void ShaderManager::prebindColorLocations(GLuint program,  Shader* owner)
{
  const std::map<std::string, ShaderColorData>& colorLocs
  = owner->getColorLocations();
  for(std::pair<std::string, ShaderColorData> colorLoc : colorLocs)
  {
    GLuint index = colorLoc.second.location;
    const char* name = colorLoc.second.name.c_str();
    GLUtil::bindColorLocation(program, index, name);
  }
  
}
void ShaderManager::removeShaders(std::vector<ShaderInfo*>& shaders)
{
  for(ShaderInfo* shader : shaders)
  {
    std::string key = shader->name;
    std::map<std::string, ShaderInfo *>::iterator it = _shaderInfoDB.find(key);
    if (it != _shaderInfoDB.end()) {
      delete it->second;
      _shaderInfoDB.erase(it);
    }
  }
}
  
void ShaderManager::removeProgram(ProgramInfo* program)
{
  if(program == nullptr)
  {
    std::cerr << "Tried to remove null program from ShaderManager" << std::endl;
    return;
  }
  std::map<std::string, ProgramInfo *>::iterator it =
                              _programInfoDB.find(program->name);
  if (it != _programInfoDB.end()) {
    delete it->second;
    _programInfoDB.erase(it);
  }
}
bool ShaderManager::reloadShaders()
{
  bool success = true;
 
  for(std::pair<std::string, ShaderInfo*> shader : _shaderInfoDB)
  {
    ShaderInfo& info = *shader.second;
    if(info.immutable)
    {
      continue;
    }
    std::string source = GLUtil::getShaderSource(info.filename);
    
    std::string hash = md5(source);
    if(hash.compare(info.hash))
    {
      std::cout << "Recompiling (" << info.glID << ") " << info.filename << std::endl;
      info.source = source;
      bool compiled = GLUtil::recompileShader(info);
      if(!compiled)
      {
        std::cerr << "Failed to compile " << info.filename << std::endl;
      }
      
      info.hash = hash;
      info.changed = true;
    }
  }
  
  for(std::pair<std::string, ProgramInfo*> program : _programInfoDB)
  {
    ProgramInfo& info = *program.second;
    bool changed = false;
    for(ShaderInfo* shader : info.shaders)
    {
      if(shader->changed)
      {
        changed = true;
        break;
      }
    }
    
    if(changed)
    {
      std::cout << "Re-linking (" << info.glID << ") " << info.name << std::endl;
      
      bool success = false;
      if (info.owner->isSpecifyingAttributeLocations() || info.owner->isSpecifyingColorLocations())
      {
        success = GLUtil::recreateProgramAndAttachShaders(info);
        
      if (info.owner->isSpecifyingAttributeLocations())
      {
        prebindAttributeLocations(info.glID, info.owner);
      }
      if (info.owner->isSpecifyingColorLocations())
      {
        prebindColorLocations(info.glID, info.owner);
      }
        success = GLUtil::linkAndVerifyProgram(info.glID,&info) != 0;
      }
      else
      {
        success = GLUtil::relinkAndVerifyProgram(info);
      }
      if(!success)
      {
        std::cerr << "Failed to recompile program " << std::endl;
      }
    }
    
  }
  
  return success;
}
  
ShaderInfo& ShaderManager::getShader(std::string& name)
{
  return *_shaderInfoDB[name];
}

ProgramInfo& ShaderManager::getProgram(std::string& name)
{
  return *_programInfoDB[name];
}
  
}//namespace renderlib
