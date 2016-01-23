//
//  TrainingDataRenderer.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include "TrainingDataRenderer.h"
#include "mesh.h"
#include "Globals.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "RenderManager.h"
#include "ObjectIDGenerator.h"
#include "GLFWTime.h"
#include <iostream>
#include <fstream>

//Get rid of this once we have a texture class working
#include "OpenGLHelper.h"

namespace renderlib{

TrainingDataRenderer::TrainingDataRenderer()
{
  _renderSortValue = IRenderable::DefaultSortValue;
  _renderObjectID = ObjectIDGenerator::getInstance().getNextID();
}

TrainingDataRenderer::~TrainingDataRenderer()
{
  delete _mesh;//I own the mesh
  delete _labelsMesh;
}
  
void TrainingDataRenderer::loadTrainingData(const std::string& filename, bool labels)
{
  std::string delimiter = "\t";
  std::ifstream  fin(filename.c_str());
  std::string    line;
  
  //  std::cout << " good()=" << fin.good();
  //  std::cout << " eof()=" << fin.eof();
  //  std::cout << " fail()=" << fin.fail();
  //  std::cout << " bad()=" << fin.bad() << std::endl;
  
  if(fin.fail())
  {
    std::cout << "Failed to open " << filename << std::endl;
  }
  
  std::vector< std::vector<float> > data;
  while(std::getline(fin, line))
  {
    line = rtrim(line);
    std::vector<std::string> elems;
    SplitString(line, delimiter.c_str()[0], elems);
    std::vector<float> positions_row;
    for(std::string s : elems)
    {
      float f = std::stof(s);
      positions_row.push_back(f);
    }
    data.push_back(positions_row);
  }
  
  if(labels == false)
  {
    _positions.clear();
    for(std::vector<float>& row : data)
    {
      if(row.size() % 3 == 0)
      {
        std::vector<vec3> p_row;
        for(unsigned int i = 0; i < row.size(); i+=3)
        {
          vec3 p(row[i], row[i+1], row[i+2]);
          p_row.push_back(p);
        }
        _positions.push_back(p_row);
      }
    }
  }
  else
  {
    
    _labels.clear();
    for(std::vector<float>& row : data)
    {
      if(row.size() % 3 == 0)
      {
        for(unsigned int i = 0; i < row.size(); i+=3)
        {
          vec3 p(row[i], row[i+1], row[i+2]);
          _labels.push_back(p);
        }
      }
    }
  }
  
}

void TrainingDataRenderer::init()
{
  loadTrainingData("assets/mocap_data/positions_only_mocap_data.txt", false);
  loadTrainingData("assets/mocap_data/positions_only_mocap_labels.txt", true);
  //For now just hard code some stuff in here to see if everything else works!
  _shader = new Shader();
  _shader->registerShader("shaders/vertColoredVerts.glsl", ShaderType::VERTEX);
  _shader->registerShader("shaders/fragColoredVerts.glsl", ShaderType::FRAGMENT);
  _shader->compileShaders();
  _shader->linkShaders();

  std::vector<Color> colors = {
    Color::white(),
    Color::black(),
    Color::red(),
    Color::blue(),
    Color::green()
  };
  
  Vector3 normal(1.0, 0.0,0.0);
  Vector2 uv(0.0,0.0);
  Vector4 tangent(1.0, 0.0, 0.0, 0.0);
  
  _mesh = new Mesh();
  for(std::vector<vec3> row : _positions)
  {
    int color_idx= 0;
    for(vec3 v : row)
    {
      Color c = colors[color_idx];
      Vector3 c3(c.r,c.g,c.b);
      Vector4 c4(c.r,c.g,c.b, c.a);
      Vector3 p(v.x ,v.y,v.z);
      _mesh->addPosition(p);
      
      //FIXME:  THE COLOR CHANNEL IS OFF/INCORRECT.  HAD TO STUFF COLORS INTO THE NORMAL
      //CHANNEL.  THE TANGENT OR UV IS WRONG.
      _mesh->addColor(colors[color_idx]);
      _mesh->addNormal(c3);
      _mesh->addUV(uv);
      _mesh->addTangent(c4);
      color_idx++;
    }
  }
  _mesh->constructBuffer();
  _mesh->setPrimitiveTypeToPoints();
  _mesh->bindAttributesToVAO(*_shader);
  //_mesh->bindAttributesToVAO();
  
  
  _labelsMesh = new Mesh();
  for(vec3 v : _labels)
  {
    Color c(1.0, 1.0,0.0,1.0);
    Vector3 n(c.r ,c.g,c.b);
    Vector4 t(c.r ,c.g,c.b, c.a);
    Vector3 p(v.x ,v.y,v.z);
    _labelsMesh->addPosition(p);
      
      //FIXME:  THE COLOR CHANNEL IS OFF/INCORRECT.  HAD TO STUFF COLORS INTO THE NORMAL
      //CHANNEL.  THE TANGENT OR UV IS WRONG.
    _labelsMesh->addColor(c);
    _labelsMesh->addNormal(n);
    _labelsMesh->addTangent(t);
  }
  _labelsMesh->constructBuffer();
  _labelsMesh->setPrimitiveTypeToPoints();
  _labelsMesh->bindAttributesToVAO(*_shader);

  _mvp.identity();

  vec3 min, max;
  _mesh->calculateBoundingBox(min, max);
  printf("\n");
  printf("Min %3.4f,%3.4f,%3.4f\n", min.x, min.y, min.z);
  printf("Max %3.4f,%3.4f,%3.4f\n", max.x, max.y, max.z);
  
  _fpsController = std::make_shared<FPSController>();
  InputManager::addListener(_fpsController);
  _fpsController->init();
}

void TrainingDataRenderer::update(float time)
{

  float fps = 60;
  _currVertStart = 5*((int)(fps*time)) % _mesh->getVertexCount();
  // Conversion from Euler angles (in radians) to Quaternion
  vec3 EulerAngles(time*0.3, time*0.1, 0);
  quat q = quat(EulerAngles);
  _m = glm::mat4_cast(q);
  


  vec3 translationSpeed(10.0);
  _position += _fpsController->getVelocity() * GLFWTime::getDT()  * translationSpeed;;
  _orientation = _fpsController->getRotation();
  _m = glm::translate(_position) * glm::toMat4(_orientation);
  

  _fpsController->update(time);
  /*
  //_m = Matrix4::rotationZYX(Vector3(time*0.2f, time*0.3,0));
  
  Point3 EyePosition(0, 0, 5);

  Vector3 up(0, 1, 0); Point3 target(0);
  Matrix4 ViewMatrix = Matrix4::lookAt(EyePosition, target, up);

  _m = Matrix4::rotationZYX(Vector3(time*0.2f, time*0.3,0));
  Matrix4 ModelviewMatrix = ViewMatrix * _m;
  
  float n = 0.01f;
  float f = 1000.0f;
  float fieldOfView = 0.7f;

  Matrix4 ProjectionMatrix = Matrix4::perspective(fieldOfView, 1, n, f);
  _mvp = ProjectionMatrix * ViewMatrix * _m;
  //_m = Matrix4::rotation(-time * 0.25f, Vector3::yAxis());
  _mvpRot = ProjectionMatrix * ViewMatrix * _m;
 */
  
  //Overriding everything. using camera

  

}

void TrainingDataRenderer::preRender()
{


}

void TrainingDataRenderer::draw()
{
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  Camera& cam = RenderManager::getInstance().getMainCamera();
  mat4 view = cam.getModelview();
  mat4 proj = cam.getProjection();
  mat4 mv = view *_m;
  _shader->bind();


  /*
  _renderTexture.clear();
  _renderTexture.bindFBO();

   */
  mat4 mvp = proj * mv;
  
  _texture.bindToChannel(0);
  _shader->setUniform("ModelViewProjection", mvp);
  _shader->setUniform("ModelView", mv);
  _shader->setUniform("NormalTransform", mv);
  _shader->setUniform("texture0", 0);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  _mesh->drawBuffersRange(_currVertStart,5);
  _labelsMesh->drawBuffersRange(_currVertStart/5,1);
  //_mesh->drawBuffers();
  glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

 
  _shader->unbind();

}

void TrainingDataRenderer::postRender()
{


}

}// namespace renderlib
