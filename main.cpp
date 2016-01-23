//
//  main.cpp
//
#ifdef __APPLE__
	#define GLFW_INCLUDE_GLCOREARB
#elif WIN32
	#pragma comment(lib, "GLFW/glfw3dll.lib")
	#pragma comment(lib, "opengl32.lib")
	#include <GL/glew.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <cmath>
#include "GLFW/glfw3.h" // - lib is in /usr/local/lib/libglfw3.a


#include "Platform.h"

static int winWidth = 800;
static int winHeight = 800;



void hintOpenGLCoreProfile(int major, int minor){
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
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

  if (key == '1' && action == GLFW_PRESS)
  {
    
  }
}

static void mouseButtonHandler(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
  {

  }
  else if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
  {

  }
  else if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
  {

  }
  else if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE)
  {

     
  }
}

static void mousePositionHandler(GLFWwindow* window, double x, double y)
{

}

static void mouseScrollHandler(GLFWwindow* window, double xoffset, double yoffset)
{

}

void resizeViewport(GLFWwindow* window){
  glfwGetFramebufferSize(window, &winWidth, &winHeight);
  glViewport(0, 0, winWidth, winHeight);
  glClear(GL_COLOR_BUFFER_BIT);
}

static void windowResizeHandler(GLFWwindow* window, int width, int height)
{
  resizeViewport(window);
}



#include <iostream>
#include <fstream>
static GLuint load3DScan(const std::string& filename,int dx, int dy, int dz)
{
    GLuint handle;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_3D, handle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


    int array_size = dx*dy*dz;
    unsigned short *data = new unsigned short[array_size];

    int position = 0;
    size_t chunkSize = sizeof(unsigned short)*512;

    std::ifstream fin(filename.c_str(), std::ios::in|std::ios::binary);
    if(fin.fail())
    {
      std::cout << "Could not find " << filename << std::endl;
    }
    while(fin.read((char*)&data[position],chunkSize))
    {
      position += 512;

    }


    fin.close();

    glTexImage3D(GL_TEXTURE_3D, 0,
                 GL_RED,
                 dx, dy, dz, 0,
                 GL_RED,
                 GL_UNSIGNED_SHORT,
                 data);

    delete[] data;
    return handle;
}


void initialize()
{
  printf("Initializing...\n");
}

void render()
{
  glClearColor(0.0f,0.0f,0.1f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

static void update(double seconds)
{

}

int main(void)
{
  GLFWwindow* window;
  glfwSetErrorCallback(error_callback);
  if (!glfwInit())
    exit(EXIT_FAILURE);

  hintOpenGLCoreProfile(4, 1);
  winWidth = 800;
  winHeight = 800;

  window = glfwCreateWindow(winWidth, winHeight, "3D Renderer", NULL, NULL);

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


  while (!glfwWindowShouldClose(window))
  {
    update(glfwGetTime());
    render();

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}


