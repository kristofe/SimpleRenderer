//
//  SimpleMeshRenderer.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include "SimpleMeshRenderer.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "RenderManager.h"
#include "ObjectIDGenerator.h"
#include "GLFWTime.h"

//Get rid of this once we have a texture class working
#include "OpenGLHelper.h"

namespace renderlib{

SimpleMeshRenderer::SimpleMeshRenderer()
{
  _renderSortValue = IRenderable::DefaultSortValue;
  _renderObjectID = ObjectIDGenerator::getInstance().getNextID();
}

SimpleMeshRenderer::~SimpleMeshRenderer()
{

}

void SimpleMeshRenderer::init()
{
  //For now just hard code some stuff in here to see if everything else works!
  _shader = new Shader();
  _shader->registerShader("shaders/vertShader.glsl", ShaderType::VERTEX);
  _shader->registerShader("shaders/fragShader.glsl", ShaderType::FRAGMENT);
  _shader->compileShaders();
  _shader->linkShaders();

  //_mesh = new Mesh();
  //_mesh->createScreenQuad(Vector2(-1,-1), Vector2(1,1));
  //_mesh->createCube();
  //_mesh->bindAttributesToVAO(*_shader);

  _mvp.identity();

  _texture.loadFile("assets/companion_cube.png",TextureFilterMode::LINEAR, TextureClampMode::CLAMP_TO_EDGE);
  //_renderTexture.setupFBO(1024,1024,true, GL_RGBA8, GL_RGBA,TextureDataType::TDT_UBYTE);
  //_renderTexture.setupDebugData(Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
  //_renderTexture.setClearColor(Color(0.0f, 0.0f, 0.2f, 1.0f));


  //_model.loadModelFromFile("assets/models/sponza/sponza.obj");
  //_model.loadModelFromFile("assets/models/sphere.dae");
  //_model.loadModelFromFile("assets/models/nanosuit/nanosuit2.obj");
  _model.loadModelFromFile("assets/models/KidsRoomModel/kidsroom.fbx", true);
  //_model.bindAttributesToVAO(*_shader);
  vec3 min, max;
  _model.calculateBoundingBox(min, max);
  printf("\n");
  printf("Min %3.4f,%3.4f,%3.4f\n", min.x, min.y, min.z);
  printf("Max %3.4f,%3.4f,%3.4f\n", max.x, max.y, max.z);
  mat4 mm = glm::translate(vec3(0, -19, 0));
  mm = mm * glm::scale(vec3(25.0, 25.0, 25.0));
  _model.setTransform(mm);

  _fpsController = std::make_shared<FPSController>();
  InputManager::addListener(_fpsController);
  _fpsController->init();
}

void SimpleMeshRenderer::update(float time)
{

  _fpsController->update(time);
  // Conversion from Euler angles (in radians) to Quaternion
  //vec3 EulerAngles(time*0.3, time*0.1, 0);
  //quat q = quat(EulerAngles);
  //_m = glm::mat4_cast(q);
  


  //FIXME: Make sure these happen in the correct coordinate frame.
  //They are in the world coordinate frame right now
  
  vec3 translationSpeed(10.0);
  _position += _fpsController->getVelocity() * GLFWTime::getDT()  * translationSpeed;;
  _orientation = _fpsController->getRotation();
  _m = glm::translate(_position) * glm::toMat4(_orientation);
  

  

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

void SimpleMeshRenderer::preRender()
{


}

void SimpleMeshRenderer::draw()
{
  glEnable(GL_DEPTH_TEST);
  //glDisable(GL_CULL_FACE);
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
 // _mesh->drawBuffers();

  _model.draw(mv,proj, cam.getEyePosition());

/*
  _renderTexture.unbindFBO();

  _renderTexture.bindAllTargetsStartingAt(0);
  _shader->setUniform("ModelViewProjection", _mvpRot);
  _shader->setUniform("texture0", 0);
  _mesh->drawBuffers();
//  _renderTexture.unbind();
 */
  _shader->unbind();
/*
  if(_debugDraw)
  {
    _renderTexture.debugDraw();
  }
  _renderTexture.clear();
 */
}

void SimpleMeshRenderer::postRender()
{


}

}// namespace renderlib
