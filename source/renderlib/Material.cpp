//
//  Material.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 7/15/14.
//
//

#include <algorithm>
#include "Material.h"
namespace renderlib{
  
  using namespace vmath;
  
  Material::Material():_shader(nullptr)
  {
    shininess = 0.0f;
    shininessStrength = 0.0f;
    refraction = 0.0f;
  }
  
  
  Material::Material(std::shared_ptr<Shader> shader):_shader(shader)
  {
    shininess = 0.0f;
    shininessStrength = 0.0f;
    refraction = 0.0f;
  }
  
  Material::~Material()
  {
    _shader.reset();
  }
  
  void Material::bind()
  {
    _shader->bind();
    //TODO:add texture binding here as well?
    //Should texture channels be assigned dynamically here?
    //As in go down the list and assign 0 to first and add one for each?
  }
  
  void Material::unbind()
  {
    _shader->unbind();
  }
  
  void Material::setTexture(std::shared_ptr<Texture> t, std::string name)
  {
    _textures.clear();
    _textures.push_back(t);
    _textureUniformNames.clear();
    _textureUniformNames[name] = t;
  }
  
  void Material::addTexture(std::shared_ptr<Texture> t, std::string name)
  {
    if(_textureUniformNames.find(name) != _textureUniformNames.end())
    {
      _textures.push_back(t);
      _textureUniformNames[name] = t;
    }
    else{
      std::cerr << "Attempted to put duplicate texture name into material"
          << std::endl;
    }
  }
  
  void Material::removeTexture(std::shared_ptr<Texture> t, std::string name)
  {
    _textures.erase( std::remove(
                                 _textures.begin(),
                                 _textures.end(),t
                                )
                    ,_textures.end()
                    );
    _textureUniformNames.erase(_textureUniformNames.find(name));
  }
  
  void Material::clearTextures()
  {
    _textures.clear();
  }
};