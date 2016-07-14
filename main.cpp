//
//  main.cpp
//
// This is included first... It will include the opengl headers in the correct
// order.
#include "Platform.h"
#ifdef WIN32
  #include "string_util/win32_debug_buffer.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
//#include "GLFW/glfw3.h" // - lib is in /usr/local/lib/libglfw3.a
#include "Platform.h"
#include "Renderer.h"
#include "RenderManager.h"
#include "ShaderManager.h"
#include "InputManager.h"
#include "GLFWTime.h"
#include "WindowManager.h"

//Renderable objects
#include "SimpleMeshRenderer.h"
#include "TrainingDataRenderer.h"
#include "PlatformRoom.h"
#include "InstancedMeshRenderer.h"
#include "SimpleDistanceFieldRenderer.h"
#include "SimpleDFPathTracer.h"
#include "SimpleVolumeRenderer.h"
#include "MarchingCubesRenderer.h"
#include "FluidSim.h"
#include "FluidSim.h"
#include "WaveSim.h"
#include "Painter.h"

//These should be moved into a class 
uvec2 windowSize{256,256};
ivec2 windowPosition{0,0};
ivec2 mousePosition;
bool mouseDown{false};
bool mouseRightDown{false};

GLFWwindow * window{ nullptr };
unsigned int frame{ 0 };
unsigned int vsync{1};
unsigned int multiSampleCount = 1;

using namespace renderlib;

Renderer* renderer;

static void handleSceneChange(int key, int scancode, int action, int mods)
{
 	if (action == GLFW_RELEASE)
  {
    std::shared_ptr<IRenderable> p = nullptr;
    if (key == '1') {
      p = std::make_shared<SimpleMeshRenderer>();
    }
    if (key == '2') {
      p = std::make_shared<MarchingCubesRenderer>();
    }
    if (key == '3') {
      //p = std::make_shared<SimpleVolumeRenderer>();
      p = std::make_shared<InstancedMeshRenderer>();
    }
    if (key == '4') {
      p = std::make_shared<PlatformRoom>();
    }
    if (key == '5') {
      p = std::make_shared<FluidSim>();
    }
    if (key == '6') {
      p = std::make_shared<SimpleDFPathTracer>();
    }
    if (key == '7') {
      p = std::make_shared<SimpleDistanceFieldRenderer>();
    }
    if(p != nullptr)
    {
      renderer->clearRenderObjects();
      p->init();
      renderer->addRenderObject(p);
    }
  }
 
}

void hintOpenGLCoreProfile(int major, int minor){
  glfwWindowHint(GLFW_DEPTH_BITS, 16);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  #ifdef DEBUG_BUILD
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif
  if(multiSampleCount > 1){
    glfwWindowHint(GLFW_SAMPLES, multiSampleCount);
  }
}


static void error_callback(int error, const char* description)
{
  fputs(description, stderr);
}

static void keyHandler(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
  {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
  
  if ((key == 'r' || key == 'R') && action == GLFW_PRESS)
  {
    ShaderManager::getInstance().reloadShaders();
  }
  KeyInfo k(key, (renderlib::KeyInfo::KeyAction)action, mods);
  InputManager::handleKey(k);

  renderer->onKey(key, scancode, action, mods);
  
  handleSceneChange(key, scancode, action, mods);
}

static void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
  {
      mouseDown = true;
      InputManager::updatePointer(vec2(mousePosition.x,mousePosition.y),1.0f, 0.0f, 1.0f, 0);
  }
  else if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
  {
      mouseDown = false;
      InputManager::updatePointer(vec2(mousePosition.x,mousePosition.y),0.0f, 0.0f, 1.0f, 0);
  }
  else if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
  {
      mouseRightDown = true;
      InputManager::updatePointer(vec2(mousePosition.x,mousePosition.y),1.0f, 0.0f, 1.0f, 1);
      //trackball->ReturnHome();
  }
  else if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE)
  {

      mouseRightDown = false;
      InputManager::updatePointer(vec2(mousePosition.x,mousePosition.y),0.0f, 0.0f, 1.0f, 1);
  }

  renderer->onMouseButton(button, action, mods);
}

static void mousePositionHandler(GLFWwindow* window, double x, double y)
{
    if(mousePosition.x != (int)x || mousePosition.y != (int)y){
    mousePosition.x = (int)x;
    mousePosition.y = (int)y;
    if(mouseDown)
    {
      InputManager::updatePointer(
          vec2(mousePosition.x,mousePosition.y),
          1.0f, 
          1.0f, 
          1.0f, 
          0);
    }
  }
  renderer->onMouseMove(x,y);
}

static void mouseScrollHandler(GLFWwindow* window, double xoffset, double yoffset)
{
  renderer->onMouseScroll(xoffset, yoffset);
}

void resizeViewport(GLFWwindow* window){
  glfwGetFramebufferSize(window, (int*)&windowSize.x, (int*)&windowSize.y);
  //glClear(GL_COLOR_BUFFER_BIT);
  renderer->resizeViewport(window);
}

static void windowResizeHandler(GLFWwindow* window, int width, int height)
{
  resizeViewport(window);
  renderer->onWindowResize(window, width, height);
}

static void windowFocusHandler(GLFWwindow* window, int focused)
{
  renderer->onWindowFocus(focused);
}

void initialize()
{
  printf("Initializing...\n");
  std::cout << GetCurrentDir() << std::endl;
  ChangeParentDir("shaders");

  //windowSize = {960,1080};
  RenderManager::getInstance().setPrintFPS(true);
  RenderManager::getInstance().setPrintFPSInterval(3.0f);
  InputManager::init();
  renderer = new Renderer();

  GetGLError();
  GLFWTime::init();
  GetGLError();
  WindowManager::_mainWindowSize.set(windowSize.x,windowSize.y);
  GetGLError();
  
  renderer->postCreate();
  renderer->initGl();
  
  std::shared_ptr<IRenderable> p = std::make_shared<SimpleDFPathTracer>();
  p->init();
  renderer->clearRenderObjects();
  renderer->addRenderObject(p);
  
  Vector2 size = RenderManager::getInstance().getFramebufferSize();
  float aspect = size.x/size.y;
  
  float n = 0.01f;
  float f = 1000.0f;
  float fieldOfView = 0.7f;// 1.74532925f;//100 degrees
  
  vec3 eyePos = vec3(0.0f, 0.0f, 5.0f);
  mat4 projection = glm::perspective(fieldOfView, aspect, n, f);
  mat4 modelview = glm::lookAt( eyePos, vec3(0.0f,0.0f,0.0f),vec3(0.0f,1.0f,0.0f));

  vec4 eyePosCheck = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  eyePosCheck = glm::inverse(modelview) * eyePosCheck;
  Camera& c = RenderManager::getInstance().getMainCamera();
  c.setProjection(projection);
  c.setModelview(modelview);
  c.setFieldOfView(fieldOfView);

  c.setEyePosition(vec3(eyePosCheck));
  

}

void render()
{
  glClearColor(0.0f,0.0f,0.1f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderer->draw();
}

static void update(double seconds)
{
  GLFWTime::setTime((float)seconds);
  RenderManager& rm = RenderManager::getInstance();
  rm.updateFPS((float)GLFWTime::getDT());

  renderer->update();
}

int main(void)
{
#ifdef WIN32
  RedirectStdoutToMSVC();
#endif

  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    exit(EXIT_FAILURE);

  hintOpenGLCoreProfile(4, 1);

  window = glfwCreateWindow(windowSize.x, windowSize.y, "3D Renderer", NULL, NULL);

  if (!window)
  {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwMakeContextCurrent(window);
  glfwSetKeyCallback(window, keyHandler);
  glfwSetCursorPosCallback(window, mousePositionHandler);
  glfwSetMouseButtonCallback(window, mouseButtonHandler);
  glfwSetScrollCallback(window, mouseScrollHandler);
  glfwSetWindowSizeCallback(window, windowResizeHandler);
  glfwSetWindowFocusCallback(window, windowFocusHandler);

  int vsync = 0;//0 decoupled. 1 every refresh
  glfwSwapInterval(vsync);

#if!__APPLE__
  // initialise GLEW
  glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
  if(glewInit() != GLEW_OK)
  {
    fprintf(stderr,"glewInit failed");
    glGetError();//This will get rid of the invalid enum error this throws
    return -1;
  }
#endif

  initialize();

  resizeViewport(window);

  while (!glfwWindowShouldClose(window))
  {
    update(glfwGetTime());
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  renderer->shutdownGl();

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}


