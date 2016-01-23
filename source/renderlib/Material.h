//
//  Material.h
//  Splash
//
//  Created by Kristofer Schlachter on 7/15/14.
//
//

#ifndef __Splash__Material__
#define __Splash__Material__

#include <iostream>
#include "OpenGLHelper.h"
#include "shader.h"
#include "Texture.h"
#include <map>
#include <vector>
#include <memory>
#include "vmath.hpp"

namespace renderlib{
  enum MaterialPropertyType{
    MPT_Float = 0x1, //Array of 32 bit floats
    MPT_String = 0x3, //String
    MPT_Integer = 0x4, //Array of 32 bit integers
    MPT_Buffer = 0x5 //Binary buffer
  };
  struct MaterialProperty
  {
    std::string key;
    MaterialPropertyType type;
    std::string str;
    unsigned int dataLength;
    std::vector<float> floats;
    std::vector<int> ints;
    std::vector<char> bytes;
    //float* floatData;
    //int* intData;
    //char* binaryData;

    MaterialProperty() :
      key(""),
      type(MPT_Float),
      str(""),
      dataLength(0)
      //floatData(nullptr),
      //intData(nullptr),
      //binaryData(nullptr)
    {

    }
    MaterialProperty(const MaterialProperty& mp) :
      key(mp.key),
      type(mp.type),
      str(mp.str),
      dataLength(mp.dataLength),
      floats(mp.floats),
      ints(mp.ints),
      bytes(mp.bytes)
    {

    }
    MaterialProperty& operator=(const MaterialProperty&) = delete;
  };

//Material will eventually store refs to textures and uniforms(?)
class Material
{
  public:
  Material();
  Material(std::shared_ptr<Shader> s);
  virtual ~Material();

  void bind();
  void unbind();

  void setShader(std::shared_ptr<Shader> shader) { _shader = shader; }

  Shader& getShader(){ return *_shader;}

  void setTexture(std::shared_ptr<Texture> t, std::string name);
  void addTexture(std::shared_ptr<Texture> t, std::string name);
  void removeTexture(std::shared_ptr<Texture> t, std::string name);
  void clearTextures();

  //C++ 11 way of hiding these methods
  //no copy constructor or copy assignment operato
  //Material(const Material&) = delete;
  Material & operator=(const Material&) = delete;

  public:
    std::vector<MaterialProperty> properties;
    std::string _name;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    vec3 emissive;
    float shininess;
    float shininessStrength;
    float refraction;
    std::vector<Texture> textures;
    
  private:

  protected:
    std::shared_ptr<Shader> _shader;
    std::vector< std::shared_ptr<Texture> >_textures;
    std::map<std::string, std::weak_ptr<Texture> > _textureUniformNames;

};

}//namespace renderlib


#endif /* defined(__Splash__Material__) */
