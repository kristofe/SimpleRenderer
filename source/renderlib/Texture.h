//
//  Texture.h
//  VolumeRenderer
//
//  Created by Kristofer Schlachter on 6/10/14.
//
//

#ifndef __VolumeRenderer__Texture__
#define __VolumeRenderer__Texture__

#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <memory>
#include "vmath.hpp"
#include "allmath.h"
#include "OpenGLHelper.h"

namespace renderlib{

using namespace vmath;
  
class UniformHGrid;
class TriangleMesh;
class Mesh;

class Texture
{
public:


public:
  Texture();
  virtual ~Texture();

  void setTextureProxy(std::shared_ptr<TextureProxy> tp);
  void loadBlank();
  void loadDistanceFieldFromDisk( std::string const& filename);
  void createDistanceFieldFromMesh(int dim, const TriangleMesh& mesh, bool writeToFile = false, std::string const& filename = "");
  void writeDistanceFieldToDisk(int dim, glm::vec4 * data, std::string const& filename);
  void readDistanceFieldFromDisk(int& dim, glm::vec4** data, std::string const& filename);
  //void writeDistanceFieldToDisk(int dim, float * data, std::string const& filename);
  //void readDistanceFieldFromDisk(int& dim, float** data, std::string const& filename);
  void loadFile(std::string path, std::string directory, TextureFilterMode tfm = TextureFilterMode::LINEAR, TextureClampMode tcm = TextureClampMode::REPEAT);
  void loadFile(std::string path, TextureFilterMode tfm, TextureClampMode tcm);
  void loadPNG(std::string path, TextureFilterMode tfm, TextureClampMode tcm);
  void loadTGA(std::string path, TextureFilterMode tfm, TextureClampMode tcm);
  void loadCubemapPNG(std::string basepath);
  void createPyroclasticVolume(int n, float r);
  void createPyroclasticDistanceField(int n, float r, float strength);
  void loadRaw3DData(const std::string& filename, int dx, int dy, int dz, TextureDataType textureDataType);
  virtual void bindToChannel(int textureChannel);
  //virtual void unbind(int textureChannel);

  void setAnisoPercent(float pct){ _textureProxy->anisoValue = pct;}
  void uploadData(){_textureProxy->uploadData();}

  virtual void debugDraw();
  virtual void setupDebugData(Vector2 min, Vector2 max);

  unsigned int getID(){ return static_cast<unsigned int>(_textureProxy->glID); }
  unsigned int getWidth(){ return _textureProxy->width; }
  unsigned int getHeight(){ return _textureProxy->height; }

  std::string getPath(){ return _path; }
  std::string getType(){ return _type; }
  void setType(std::string type){ _type = type; }

  //C++ 11 way of hiding these methods
  //Default constructor, no copy constructor or copy assignment operato
  //Texture() = default;
  //Texture(const Texture&) = delete;
  //Texture & operator=(const Texture&) = delete;
  Texture(const Texture& other)
  {
    this->_path = other._path;
    this->_type = other._type;
    this->_debugShader = other._debugShader;
    this->_debugMesh = other._debugMesh;
    this->_textureProxy = other._textureProxy;

  }
  Texture & operator=(const Texture& other)
  {
    this->_path = other._path;
    this->_type = other._type;
    this->_debugShader = other._debugShader;
    this->_debugMesh = other._debugMesh;
    this->_textureProxy = other._textureProxy;
    return *this;
  }

protected:
  std::shared_ptr<TextureProxy> _textureProxy;
  Mesh* _debugMesh;
  Shader* _debugShader;
  std::string _path;
  std::string _type;
  bool _is3D;
};

} //namespace renderlib


#endif /* defined(__VolumeRenderer__Texture__) */
