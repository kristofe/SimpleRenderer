#include "SimpleCamera.h"
#include "RenderManager.h"
#include "allmath.h"

#include "Model.h"


namespace renderlib{
  
using namespace vmath;
  
Model::Model()
{
  init();
}

void Model::init()
{
  _transform = mat4();

  _shader.registerShader("shaders/model/modelVertShader.glsl", ShaderType::VERTEX);
  _shader.registerShader("shaders/model/modelFragShader.glsl", ShaderType::FRAGMENT);
  _shader.compileShaders();
  _shader.linkShaders();
}
  
Model::~Model()
{
  clearMeshes();
  clearMaterials();
}

void Model::draw(mat4 view, mat4 proj, vec3 eyePosition)
{
  //Camera& cam = RenderManager::getInstance().getMainCamera();
  //mat4 view = cam.getModelview();
  //mat4 proj = cam.getProjection();
  mat4 mv = view *_transform;
  _shader.bind();
  mat4 mvp = proj * mv;

  //TODO: Properly handle all of the textures
  //_texture.bindToChannel(0);
  _shader.setUniform("projection", proj);
  _shader.setUniform("view", view);

  //Lighting uniforms
  _shader.setUniform("viewPos", eyePosition);

  glm::vec3 pointLightPositions[] = {
    glm::vec3(2.3f, -1.6f, -3.0f),
    glm::vec3(-1.7f, 0.9f, 1.0f)
  };

  //TODO: CREATE BETTER LIGHTS! AT LEAST ADD A DIRECTIONAL
  // Point light 1
  _shader.setUniform("pointLights[0].position", pointLightPositions[0]);
  _shader.setUniform("pointLights[0].ambient", vec3(0.05f, 0.05f, 0.05f));
  _shader.setUniform("pointLights[0].diffuse", vec3(1.0f, 1.0f, 1.0f));
  _shader.setUniform("pointLights[0].specular", vec3(1.0f, 1.0f, 1.0f));
  _shader.setUniform("pointLights[0].constant", 1.0f);
  _shader.setUniform("pointLights[0].linear", 0.0009f);
  _shader.setUniform("pointLights[0].quadratic", 0.00032f);
  // Point light 2
  _shader.setUniform("pointLights[1].position", pointLightPositions[1]);
  _shader.setUniform("pointLights[1].ambient", vec3(0.05f, 0.05f, 0.05f));
  _shader.setUniform("pointLights[1].diffuse", vec3(1.0f, 1.0f, 1.0f));
  _shader.setUniform("pointLights[1].specular", vec3(1.0f, 1.0f, 1.0f));
  _shader.setUniform("pointLights[1].constant", 1.0f);
  _shader.setUniform("pointLights[1].linear", 0.0009f);
  _shader.setUniform("pointLights[1].quadratic", 0.00032f);

  //Material
  for (unsigned int i = 0; i < _meshes.size(); i++)
  {
    auto m = _meshes[i];
    auto mat = _materials[m->getMaterialIndex()];

    _shader.setUniform("material.diffuse", mat->diffuse);
    _shader.setUniform("material.specular", mat->specular);
    _shader.setUniform("material.ambient", mat->ambient);
    _shader.setUniform("material.emissive", mat->emissive);
    _shader.setUniform("material.shininess", mat->shininess);
    //_shader.setUniform("material.shininess", 32.0f);
    _shader.setUniform("material.shininessStrength", mat->shininessStrength);
    _shader.setUniform("material.refraction", mat->refraction);
    glm::mat4 mtx = this->_transforms[i];
    _shader.setUniform("model", _transform * mtx);

    int j = 0;
    for (auto tex : mat->textures)
    {
      tex.bindToChannel(j);
      std::string uniformName = "material." + tex.getType() + "1";
      _shader.setUniform(uniformName.c_str(), j);
      ++j;
    }
    m->drawBuffers();
  }
}

void Model::calculateBoundingBox(vec3& min, vec3& max)
{
  _min.x = _min.y = _min.z = 99999999.0f;
  _max.x = _max.y = _max.z = -99999999.0f;
 
  for (auto m : _meshes)
  {
    vec3 min, max;
    m->calculateBoundingBox(min, max);

    if (min.x < _min.x) _min.x = min.x;
    if (min.y < _min.y) _min.y = min.y;
    if (min.z < _min.z) _min.z = min.z;

    if (max.x > _max.x) _max.x = max.x;
    if (max.y > _max.y) _max.y = max.y;
    if (max.z > _max.z) _max.z = max.z;
  }

  min = _min;
  max = _max;


}

void Model::bindAttributesToVAO(Shader& shader)
{
  for (auto m : _meshes)
  {
    m->bindAttributesToVAO(shader);
  }
}

void Model::collapseMeshes(Mesh& mesh, std::vector<Material>& materials)
{
  //FIXME: What if the mesh isn't indexed?
  int xformIdx = 0;
  for(std::shared_ptr<Mesh>& subMesh : _meshes)
  {
    glm::mat4 xform = mat4(_transforms[xformIdx++]);
    glm::mat4 xposeXform = glm::inverseTranspose(xform);

    uint32_t materialIdx = subMesh->getMaterialIndex();
    
    std::vector<Vector3>& positions = subMesh->getPositionVector();
    std::vector<Vector3>& normals = subMesh->getNormalVector();
    std::vector<uint32_t>& indices = subMesh->getIndexVector();
    for(unsigned int i = 0; i < indices.size()/3; ++i)
    {
	  int i0 = indices[i*3];
	  int i1 = indices[i*3 + 1];
	  int i2 = indices[i*3 + 2];
     //Now go through each triangle
      vec3 p0(positions[i0].x, positions[i0].y, positions[i0].z);
      vec3 p1(positions[i1].x, positions[i1].y, positions[i1].z);
      vec3 p2(positions[i2].x, positions[i2].y, positions[i2].z);
      vec3 n0(normals[i0].x, normals[i0].y, normals[i0].z);
      vec3 n1(normals[i1].x, normals[i1].y, normals[i1].z);
      vec3 n2(normals[i2].x, normals[i2].y, normals[i2].z);
      
      //Transform the data according to the models local transform
      p0 = vec3(vec4(p0,1.0) * xform);
      p1 = vec3(vec4(p1,1.0) * xform);
      p2 = vec3(vec4(p2,1.0) * xform);
      
      n0 = vec3(vec4(n0,1.0) * xposeXform);
      n1 = vec3(vec4(n1,1.0) * xposeXform);
      n2 = vec3(vec4(n2,1.0) * xposeXform);
      
      mesh.addPosition(p0);
      mesh.addPosition(p1);
      mesh.addPosition(p2);
      
      mesh.addNormal(n0);
      mesh.addNormal(n1);
      mesh.addNormal(n2);
      
      //TODO: How do I handle material id's???
	  //FIXME: Can't put them into indices because it breaks triangle setup etc.
      
    }
  }
  
  for(std::shared_ptr<Material> mat : _materials)
  {
    materials.push_back(*mat);
  }

  mesh.constructBuffer();
  
  
}
// Loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
void Model::loadModelFromFile(std::string path, bool flipUVs, bool forDistField)
{
  _filename = path.substr(path.find_last_of('/'), path.length());
  // Read file via ASSIMP
  Assimp::Importer importer;
  
  unsigned int flags = aiProcessPreset_TargetRealtime_Fast;
  if (forDistField)
  {
	  flags = aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_PreTransformVertices | aiProcess_ValidateDataStructure |
		  aiProcess_RemoveRedundantMaterials | aiProcess_FixInfacingNormals | aiProcess_FindDegenerates | aiProcess_SortByPType | aiProcess_GenSmoothNormals;
	   //This with aiProcess_FindDegenerates and aiProcess_SortByPType will get rid of all degenerate triangles and not create lines or points.
	  importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType::aiPrimitiveType_POINT | aiPrimitiveType::aiPrimitiveType_LINE);
	  //Configures the #aiProcess_PretransformVertices step to normalize all vertex components into the[-1, 1] range.
	  importer.SetPropertyInteger(AI_CONFIG_PP_PTV_NORMALIZE, 1);

  }
  if (flipUVs)
  {
    flags |= aiProcess_FlipUVs;
  }
  const aiScene* scene = importer.ReadFile(path, flags);

  // Check for errors
  if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
  {
    std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
    return;
  }
  // Retrieve the directory path of the filepath
  this->_directory = path.substr(0, path.find_last_of('/'));

  //Process the materials
  this->processMaterials(scene);

  // Process ASSIMP's root node recursively
  glm::mat4 identity;
  this->processNode(scene->mRootNode, scene, identity);

  for (auto m : _meshes)
  {
    m->bindAttributesToVAO();
  }
}

//Process all of the materials
void Model::processMaterials(const aiScene* scene)
{
  //Process the materials
  for (int i = 0; i < scene->mNumMaterials; i++)
  {
    std::shared_ptr<Material> newMat = std::make_shared<Material>();
    _materials.push_back(newMat);

    aiMaterial* mat = scene->mMaterials[i];
    aiString name;
    mat->Get(AI_MATKEY_NAME, name);
    
    newMat->_name = name.C_Str();
    aiColor3D c;

    mat->Get(AI_MATKEY_COLOR_DIFFUSE, c);
    newMat->diffuse.r = c.r;
    newMat->diffuse.g = c.g;
    newMat->diffuse.b = c.b;

    mat->Get(AI_MATKEY_COLOR_SPECULAR, c);
    newMat->specular.r = c.r;
    newMat->specular.g = c.g;
    newMat->specular.b = c.b;

    mat->Get(AI_MATKEY_COLOR_AMBIENT, c);
    newMat->ambient.r = c.r;
    newMat->ambient.g = c.g;
    newMat->ambient.b = c.b;

    mat->Get(AI_MATKEY_COLOR_EMISSIVE, c);
    newMat->emissive.r = c.r;
    newMat->emissive.g = c.g;
    newMat->emissive.b = c.b;

    float v=0.0f;
    mat->Get(AI_MATKEY_SHININESS, v);
    newMat->shininess = v;

    v=0.0f;
    mat->Get(AI_MATKEY_SHININESS_STRENGTH, v);
    newMat->shininessStrength = v;

    v=0.0f;
    mat->Get(AI_MATKEY_REFRACTI, v);
    newMat->refraction = v;
    /*
    newMat->properties.resize(mat->mNumProperties);//Construct the properties now and initialize them below.

    for (int propId = 0; propId < mat->mNumProperties; propId++)
    {
      aiMaterialProperty* prop = mat->mProperties[propId];
      MaterialProperty& mp = newMat->properties[propId];

      mp.key = prop->mKey.C_Str();

      char* buff = nullptr;

      switch (prop->mType)
      {
      case aiPTI_Integer:
        mp.type = MPT_Integer;
        for (int i = 0; i < prop->mDataLength/sizeof(int); i++){
          mp.ints.push_back(((int*)prop->mData)[i]);
        }
        break;

      case aiPTI_String:
        mp.type = MPT_String;
        buff = (char*)malloc(prop->mDataLength);
        memcpy(buff, prop->mData, prop->mDataLength);
        mp.str.append(buff);
        break;

      case aiPTI_Float:
        mp.type = MPT_Float;
        for (int i = 0; i < prop->mDataLength/sizeof(float); i++){
          mp.floats.push_back(((float*)prop->mData)[i]);
        }
        break;

      case aiPTI_Buffer:
        mp.type = MPT_Buffer;
        for (int i = 0; i < prop->mDataLength/sizeof(char); i++){
          mp.bytes.push_back(((char*)prop->mData)[i]);
        }
        break;
      }
      if (buff != nullptr) free(buff);
    }
    */
  }
}
// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform)
{
  glm::mat4 nodeTransform;
  for (int i = 0; i < 4; i++)
  for (int j = 0; j < 4; j++)
    nodeTransform[i][j] = (node->mTransformation[j])[i];

  nodeTransform = parentTransform * nodeTransform;
  
  // Process each mesh located at the current node
  for (GLuint i = 0; i < node->mNumMeshes; i++)
  {
    // The node object only contains indices to index the actual objects in the scene. 
    // The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    this->_meshes.push_back(this->processMesh(mesh, scene));


    this->_transforms.push_back(nodeTransform);
  }
  
  // After we've processed all of the meshes (if any) we then recursively process each of the children nodes
  for (GLuint i = 0; i < node->mNumChildren; i++)
  {
    this->processNode(node->mChildren[i], scene, nodeTransform);
  }

}


std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
  using namespace std;
  std::shared_ptr<Mesh> m = std::make_shared<Mesh>();

  // Data to fill
  vector<Vector3>& positions = m->getPositionVector();;
  vector<Vector3>& normals = m->getNormalVector();
  vector<Vector4>& tangents = m->getTangentVector();
  vector<Vector2>& uvs = m->getUVVector();
  vector<Color>& colors = m->getColorVector();
  vector<uint32_t>& indices = m->getIndexVector();
  vector<Texture> textures;

  // Walk through each of the mesh's vertices
  for (GLuint i = 0; i < mesh->mNumVertices; i++)
  {
    Vector3 vector; // We declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
    // Positions
    vector.x = mesh->mVertices[i].x;
    vector.y = mesh->mVertices[i].y;
    vector.z = mesh->mVertices[i].z;
    positions.push_back(vector);
    // Normals
    vector.x = mesh->mNormals[i].x;
    vector.y = mesh->mNormals[i].y;
    vector.z = mesh->mNormals[i].z;
    normals.push_back(vector);
    // Texture Coordinates
    if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
    {
      Vector2 vec;
      // A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
      // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
      vec.x = mesh->mTextureCoords[0][i].x;
      vec.y = mesh->mTextureCoords[0][i].y;
      uvs.push_back(vec);
    }
    // Tangents
    if(mesh->HasTangentsAndBitangents())
    {
      Vector4 vec;
      vec.x = mesh->mTangents[i].x;
      vec.y = mesh->mTangents[i].y;
      vec.z = mesh->mTangents[i].z;
      tangents.push_back(vec);

      //Stuff the bitangent into the color channel
      Color c;
      c.r = mesh->mBitangents[i].x;
      c.g = mesh->mBitangents[i].y;
      c.b = mesh->mBitangents[i].z;
      colors.push_back(c);
    }
  }
  // Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
  for (GLuint i = 0; i < mesh->mNumFaces; i++)
  {
    aiFace face = mesh->mFaces[i];
    // Retrieve all indices of the face and store them in the indices vector
    for (GLuint j = 0; j < face.mNumIndices; j++)
      indices.push_back(face.mIndices[j]);
  }
  // Process materials
  if (mesh->mMaterialIndex >= 0)
  {
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    // We assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // Diffuse: texture_diffuseN
    // Specular: texture_specularN
    // Normal: texture_normalN

    // 1. Diffuse maps
    vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. Specular maps
    vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. Ambient maps
    vector<Texture> ambientMaps = this->loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ambient");
    textures.insert(textures.end(), ambientMaps.begin(), ambientMaps.end());
    // 4. Ambient maps
    vector<Texture> emissiveMaps = this->loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive");
    textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
    // 4. Height maps
    vector<Texture> heightMaps = this->loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_height");
    textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
    // 5. Normal maps
    vector<Texture> normalMaps = this->loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
    textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    // 6. Shininess maps
    vector<Texture> shininessMaps = this->loadMaterialTextures(material, aiTextureType_SHININESS, "texture_shininess");
    textures.insert(textures.end(), shininessMaps.begin(), shininessMaps.end());
    // 7. Opacity maps
    vector<Texture> opacityMaps = this->loadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity");
    textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());
    // 8. Displacement maps
    vector<Texture> displacementMaps = this->loadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_displacement");
    textures.insert(textures.end(), displacementMaps.begin(), displacementMaps.end());
    // 9. Lightmap maps
    vector<Texture> lightmapMaps = this->loadMaterialTextures(material, aiTextureType_LIGHTMAP, "texture_lightmap");
    textures.insert(textures.end(), lightmapMaps.begin(), lightmapMaps.end());
  }

  m->constructBuffer();
  m->setMaterialIndex(mesh->mMaterialIndex);
  auto material = _materials[mesh->mMaterialIndex];
  material->textures = textures;

  // Return a mesh object created from the extracted mesh data
  return m;
}

// Checks all material textures of a given type and loads the textures if they're not loaded yet.
// The required info is returned as a Texture struct.
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
  std::vector<Texture> textures;
  for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    // Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
    GLboolean skip = false;
    for (GLuint j = 0; j < _textures_loaded.size(); j++)
    {
      std::string path = this->_directory + "/" + str.C_Str();
      if (_textures_loaded[j].getPath()== path)
      {
        textures.push_back(_textures_loaded[j]);
        skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
        break;
      }
    }
    if (!skip)
    {   // If texture hasn't been loaded already, load it
      Texture texture;
      printf("Model %s is trying to load texture type: %s\tfile:%s\n",_filename.c_str(),typeName.c_str(), str.C_Str());
      texture.loadFile(str.C_Str(), this->_directory);
      texture.setType(typeName);
      textures.push_back(texture);
      this->_textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
    }
  }
  return textures;
}
};