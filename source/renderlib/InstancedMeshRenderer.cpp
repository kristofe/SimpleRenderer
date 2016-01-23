//
//  InstancedMeshRenderer.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include "InstancedMeshRenderer.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "RenderManager.h"

//Get rid of this once we have a texture class working
#include "OpenGLHelper.h"

namespace renderlib{

InstancedMeshRenderer::InstancedMeshRenderer()
{
  _renderSortValue = IRenderable::DefaultSortValue;
}

InstancedMeshRenderer::~InstancedMeshRenderer()
{

  delete _shader;
  delete _cubeMesh;
  delete _roomMesh;
}

void InstancedMeshRenderer::init()
{
  //For now just hard code some stuff in here to see if everything else works!
  _shader = new Shader();
  _shader->registerShader("shaders/vertShader.glsl", ShaderType::VERTEX);
  _shader->registerShader("shaders/fragShader.glsl", ShaderType::FRAGMENT);
  _shader->compileShaders();
  _shader->linkShaders();

  _cubeMesh = new Mesh();
  //_mesh->createScreenQuad(Vector2(-1,-1), Vector2(1,1));
  _cubeMesh->createCube();
  _cubeMesh->bindAttributesToVAO(*_shader);

  _roomMesh = new Mesh();
  //_mesh->createScreenQuad(Vector2(-1,-1), Vector2(1,1));
  _roomMesh->createCube();
  _roomMesh->bindAttributesToVAO(*_shader);
  _mvp.identity();

  _cubeTexture.loadPNG("assets/companion_cube.png",TextureFilterMode::LINEAR, TextureClampMode::CLAMP_TO_EDGE);
  _wallTexture.loadPNG("assets/tiles.png",TextureFilterMode::LINEAR, TextureClampMode::CLAMP_TO_EDGE);
  //_renderTexture.setupFBO(1024,1024,true, GL_RGBA8, GL_RGBA,TextureDataType::TDT_UBYTE);
  //_renderTexture.setupDebugData(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
  //_renderTexture.setClearColor(Color(0.0f, 0.0f, 0.2f, 1.0f));

}

void InstancedMeshRenderer::update(float time)
{

  
  // Conversion from Euler angles (in radians) to Quaternion
  //vec3 EulerAngles(time*0.3, time*0.1, 0);
  vec3 EulerAngles(time, time, 0);
  quat q = quat(EulerAngles);
  _m = glm::mat4_cast(q);
  


  //_m = Matrix4::rotationZYX(Vector3(time*0.2f, time*0.3,0));
  /*
  Point3 EyePosition(0, 0, 5);

  Vector3 up(0, 1, 0); Point3 target(0);
  Matrix4 ViewMatrix = Matrix4::lookAt(EyePosition, target, up);

  _m = Matrix4::rotationZYX(Vector3(time*0.2f, time*0.3,0));
  Matrix4 ModelviewMatrix = ViewMatrix * _m;

  float n = 1.0f;
  float f = 100.0f;
  float fieldOfView = 0.7f;

  Matrix4 ProjectionMatrix = Matrix4::perspective(fieldOfView, 1, n, f);
  _mvp = ProjectionMatrix * ViewMatrix * _m;
  //_m = Matrix4::rotation(-time * 0.25f, Vector3::yAxis());
  _mvpRot = ProjectionMatrix * ViewMatrix * _m;
 */
  
  //Overriding everything. using camera

  

}

void InstancedMeshRenderer::preRender()
{


}

void InstancedMeshRenderer::draw()
{
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  Camera& cam = RenderManager::getInstance().getMainCamera();
  mat4 view = cam.getModelview();
  mat4 proj = cam.getProjection();
  _shader->bind();

  mat4 m = glm::scale(vec3(40,40,40));
  
  _wallTexture.bindToChannel(0);
  _shader->setUniform("ModelViewProjection", proj * view * m);
  _shader->setUniform("ModelView", view * m);
  _shader->setUniform("NormalTransform", m);
  _shader->setUniform("texture0", 0);
  _roomMesh->drawBuffers();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  _cubeTexture.bindToChannel(0);
  _shader->setUniform("ModelViewProjection", proj * view * _m);
  _shader->setUniform("ModelView", view * _m);
  _shader->setUniform("NormalTransform", _m);
  _shader->setUniform("texture0", 0);
  _cubeMesh->drawBuffers();

  _shader->unbind();
}

void InstancedMeshRenderer::postRender()
{


}

}// namespace renderlib
