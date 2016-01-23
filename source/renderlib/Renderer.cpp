#include "Renderer.h"

#ifdef __APPLE__
  #define GLFW_INCLUDE_GLCOREARB
  #include <unistd.h>
#elif WIN32
	#include <Windows.h> //This gets rid of weird errors in winnt.h
#elif __unix__
//  #define GLFW_INCLUDE_GLCOREARB
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "OpenGLHelper.h"
#include "shader.h"
#include "trackball.h"
#include "shadersource.h"

#include "renderlib/vmath.hpp"

extern "C" {
#include "renderlib/perlin.h"
}

#include "Platform.h"
#include "RenderManager.h"
#include "ObjectIDGenerator.h"
#include "EngineObjectFactory.h"
#include "InputManager.h"
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
#include "WindowManager.h"
#include "GLFWTime.h"
#include "shader.h"


using namespace renderlib;
using namespace vmath;

//#define START_SIM TrainingDataRenderer
//#define START_SIM SimpleMeshRenderer
//#define START_SIM SimpleVolumeRenderer
//#define START_SIM MarchingCubesRenderer
#define START_SIM SimpleDFPathTracer

void Renderer::createObjects(){
  std::shared_ptr<START_SIM> p = std::make_shared<START_SIM>();
  p->init();
  addRenderObject(p);

}



Renderer::Renderer()
{

  
}

void Renderer::addRenderObject(std::shared_ptr<IRenderable> ro)
{
  renderObjects.push_back(ro);
  InputManager::addListener(ro);
}

void Renderer::postCreate()
{
  
  
}

void Renderer::initGl()
{

  //TODO: The gl calls should be abstracted away and put into the rendermanager
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  GetGLError();
  std::string glinfo = GLUtil::getOpenGLInfo();
  GetGLError();
  std::cout << glinfo << std::endl;std::cout.flush();
  std::cout << GLUtil::getOpenGLInfo() << std::endl; std::cout.flush();
  //UI::log(GLUtil::getOpenGLInfo());
  
  createObjects();

}

void Renderer::shutdownGl() 
{
  for(auto sim : renderObjects){
    sim->reset();
  }
}

void Renderer::draw()
{
  //TODO: The gl calls should be abstracted away and put into the rendermanager
  glClearColor(0.1f, 0.2f, 0.4f, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  for(auto sim : renderObjects){
    
    sim->draw();
  }
}

void Renderer::update()
{
  InputManager::broadcastInput();
  double seconds = GLFWTime::getCurrentTime();
  for(auto sim : renderObjects){
    sim->update((float)seconds);
  }
}


void Renderer::onKey(int key, int scancode, int action, int mods)
{
	//if (action == GLFW_KEY_UP)
	{
		if (key == '1') {
			for (auto p : renderObjects)
			{
				p->reset();
			}

			renderObjects.clear();
			std::shared_ptr<SimpleMeshRenderer> p = std::make_shared<SimpleMeshRenderer>();
			p->init();
			addRenderObject(p);
		}
		if (key == '2') {
			for (auto p : renderObjects)
			{
				p->reset();
			}

			renderObjects.clear();
			std::shared_ptr<MarchingCubesRenderer> p = std::make_shared<MarchingCubesRenderer>();
			p->init();
			addRenderObject(p);
		}
		if (key == '3') {
			for (auto p : renderObjects)
			{
				p->reset();
			}

			renderObjects.clear();
			//std::shared_ptr<SimpleVolumeRenderer> p = std::make_shared<SimpleVolumeRenderer>();
			std::shared_ptr<InstancedMeshRenderer> p = std::make_shared<InstancedMeshRenderer>();
			p->init();
			addRenderObject(p);
		}
		if (key == '4') {
			for (auto p : renderObjects)
			{
				p->reset();
			}

			renderObjects.clear();
			std::shared_ptr<PlatformRoom> p = std::make_shared<PlatformRoom>();
			p->init();
			addRenderObject(p);
		}
		if (key == '5') {
			for (auto p : renderObjects)
			{
				p->reset();
			}

			renderObjects.clear();
			std::shared_ptr<FluidSim> p = std::make_shared<FluidSim>();
			p->init();
			addRenderObject(p);
		}
		if (key == '6') {
			for (auto p : renderObjects)
			{
				p->reset();
			}

			renderObjects.clear();
			std::shared_ptr<SimpleDFPathTracer> p = std::make_shared<SimpleDFPathTracer>();
			p->init();
			addRenderObject(p);
		}
		if (key == '7') {
			for (auto p : renderObjects)
			{
				p->reset();
			}

			renderObjects.clear();
			std::shared_ptr<SimpleDistanceFieldRenderer> p = std::make_shared<SimpleDistanceFieldRenderer>();
			p->init();
			addRenderObject(p);
		}
	}
}

void Renderer::onMouseButton(int button, int action, int mods)
{
}

void Renderer::onMouseMove(double x, double y)
{

}

void Renderer::onMouseScroll(double xoffset, double yoffset)
{
}

void Renderer::resizeViewport(GLFWwindow* window){
  ivec2 windowSize;
  int winX = 0;
  int winY = 0;
  glfwGetFramebufferSize(window, &winX, &winY);
  windowSize.x = winX;
  windowSize.y = winY;
  RenderManager::getInstance().setFramebufferSize(windowSize.x,windowSize.y);
  
  //FIXME: The gl calls should be abstracted away and put into the rendermanager
  //TODO: The gl calls should be abstracted away and put into the rendermanager
  glViewport(0, 0, windowSize.x, windowSize.y);
  glClear(GL_COLOR_BUFFER_BIT);
  WindowManager::_mainWindowSize.set(windowSize.x,windowSize.y);
  for(auto sim : renderObjects){
    sim->resize(); //FIXME: Resize should be an event and not invoked directly on the sim.
  }
  
}

void Renderer::onWindowResize(GLFWwindow* window, int width, int height)
{
  resizeViewport(window);
}

void Renderer::onWindowFocus(int focused)
{

}






