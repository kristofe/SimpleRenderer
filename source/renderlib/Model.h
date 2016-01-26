//
//  Model.h
//  Splash
//
//  Created by Kristofer Schlachter on 7/15/14.
//
//

#ifndef __Splash__Model__
#define __Splash__Model__

#include "OpenGLHelper.h"
#include "shader.h"
#include "mesh.h"
#include "Material.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <memory>

#include "allmath.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/config.h>
namespace renderlib{

    //FIXME: Vao construction and vertex attrib binding should happen at the
    //same time.
    //Order of VAO initialization
    //1 create and bind vao
    //2 bind or create and bind array buffer
    //3 specify attributes.  This is the step that actually associates the buffers with the vao.


class Model
{
public:
  Model();
  Model(std::string path)
  {
    init();
    this->loadModelFromFile(path);
  }

  void init();

  virtual ~Model();
  void collapseMeshes(Mesh& mesh, std::vector<Material>& materials);
  void loadModelFromFile(std::string path, bool flipUVs = false, bool forDistField = false);
  std::shared_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
  void processMaterials(const aiScene* scene);
  void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform);
  std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

  void draw(mat4 view, mat4 projection, vec3 eyePosition);

  void addMesh(std::shared_ptr<Mesh> mesh) { _meshes.push_back(mesh); }
  void setMesh(std::shared_ptr<Mesh> mesh) { clearMeshes();  _meshes.push_back(mesh); }
  void clearMeshes(){
    for (auto m : _meshes)
    {
      m.reset();
    }
    _meshes.clear();
  }
  void clearMaterials(){
    for (auto m : _materials)
    {
      m.reset();
    }
    _materials.clear();
  }
 // void setMaterial(std::shared_ptr<Material> material) { _material = _material; }
  void setTransform(mat4& m){ _transform = m; }

  //Mesh& getMesh(){ return *_mesh;}
  //Material& getMaterial() { return *_material;}
  mat4& getTransform() { return _transform;}

  void calculateBoundingBox(vec3& min, vec3& max);
  void bindAttributesToVAO(Shader& shader);

  //C++ 11 way of hiding these methods
  //no copy constructor or copy assignment operato
  Model(const Model&) = delete;
  Model & operator=(const Model&) = delete;
private:

protected:
  Shader _shader;
  std::vector<std::shared_ptr<Mesh>> _meshes;
  std::vector<glm::mat4> _transforms;
  std::vector<std::shared_ptr<Material>> _materials;
  mat4 _transform;
  std::string _filename;
  std::string _directory;
  std::vector<Texture> _textures_loaded;
  vec3 _min;
  vec3 _max;
};

}//namespace renderlib

#endif /* defined(__Splash__Model__) */
