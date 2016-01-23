#pragma once


#include <iostream>
#include <memory>
#include <exception>
#include "OpenGLHelper.h"
#include "IRenderable.h"

namespace renderlib{
// An example application that renders a simple cube
class Renderer{ 
  //TODO: Make this a vector of shared ptrs to a class of
  // IRenderable and IControllable -> Make a SimObject!
  std::vector<std::shared_ptr<IRenderable> > renderObjects;

public:
  Renderer();

  void addRenderObject(std::shared_ptr<IRenderable> ro);
  void clearRenderObjects();
  
  virtual void postCreate();
  virtual void draw();
  virtual void update();
  virtual void initGl();
  virtual void shutdownGl();


  void onKey(int key, int scancode, int action, int mods);
  void onMouseButton(int button, int action, int mods);
  void onMouseMove(double x, double y);
  void onMouseScroll(double xoffset, double yoffset);
  void resizeViewport(GLFWwindow* window);
  void onWindowResize(GLFWwindow* window, int width, int height);
  void onWindowFocus(int focused);
  
};

}



