//
//  FluidSim.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include <math.h>
#include "FluidSim.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "ObjectIDGenerator.h"
#include "RenderManager.h"
#include "GLFWTime.h"

namespace renderlib{

using namespace vmath;

FluidSim::FluidSim()
{
  _renderSortValue = IRenderable::DefaultSortValue;
}

FluidSim::~FluidSim()
{

}

void FluidSim::init()
{
  resize();
  AmbientTemperature = 0.0f;
  ImpulseTemperature = 10.0f;
  ImpulseDensity = 1.0f;
  NumJacobiIterations = 40;
  TimeStep = 0.125f;
  SmokeBuoyancy = 1.0f;
  SmokeWeight = 0.05f;
  GradientScale = 1.125f / CellSize;
  TemperatureDissipation = 0.99f;
  VelocityDissipation = 0.99f;
  DensityDissipation = 0.9999f;
  ImpulsePosition.X = GridWidth / 2;
  ImpulsePosition.Y = -(int)SplatRadius / 2;

  //The lines below should probably be moved into resize... just need to check on how to tear
  //down the previous data properly.
  int w = GridWidth;
  int h = GridHeight;
  Velocity = CreateSlab(w, h, 2);
  Density = CreateSlab(w, h, 1);
  Pressure = CreateSlab(w, h, 1);
  Temperature = CreateSlab(w, h, 1);
  Divergence = CreateSurface(w, h, 3);
  InitSlabOps();
  VisualizeProgram = CreateProgram("shaders/fluids/Vertex.vert", 0, "shaders/fluids/Visualize.frag");

  Obstacles = CreateSurface(w, h, 3);
  CreateObstacles(Obstacles, w, h);

  w = ViewportWidth * 2;
  h = ViewportHeight * 2;
  HiresObstacles = CreateSurface(w, h, 1);
  CreateObstacles(HiresObstacles, w, h);

  QuadVao = CreateQuad();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ClearSurface(Temperature.Ping, AmbientTemperature);


}

void FluidSim::update(float time)
{
  //TimeStep = GLFWTime::getDT();
  //void PezUpdate(unsigned int elapsedMicroseconds)
  //{
    glViewport(0, 0, GridWidth, GridHeight);

    Advect(Velocity.Ping, Velocity.Ping, Obstacles, Velocity.Pong, VelocityDissipation);
    SwapSurfaces(&Velocity);

    Advect(Velocity.Ping, Temperature.Ping, Obstacles, Temperature.Pong, TemperatureDissipation);
    SwapSurfaces(&Temperature);

    Advect(Velocity.Ping, Density.Ping, Obstacles, Density.Pong, DensityDissipation);
    SwapSurfaces(&Density);

    ApplyBuoyancy(Velocity.Ping, Temperature.Ping, Density.Ping, Velocity.Pong);
    SwapSurfaces(&Velocity);

    ApplyImpulse(Temperature.Ping, ImpulsePosition, ImpulseTemperature);
    ApplyImpulse(Density.Ping, ImpulsePosition, ImpulseDensity);

    ComputeDivergence(Velocity.Ping, Obstacles, Divergence);
    ClearSurface(Pressure.Ping, 0);

    for (int i = 0; i < NumJacobiIterations; ++i) {
      Jacobi(Pressure.Ping, Divergence, Obstacles, Pressure.Pong);
      SwapSurfaces(&Pressure);
    }

    SubtractGradient(Velocity.Ping, Pressure.Ping, Obstacles, Velocity.Pong);
    SwapSurfaces(&Velocity);
  //}
}

void FluidSim::preRender()
{


}

void FluidSim::simulate()
{
}


void FluidSim::draw()
{
  GLuint windowFbo = 0;
  //void PezRender(GLuint windowFbo)
  //{
  // Bind visualization shader and set up blend state:
  glUseProgram(VisualizeProgram);
  GLint fillColor = glGetUniformLocation(VisualizeProgram, "FillColor");
  GLint scale = glGetUniformLocation(VisualizeProgram, "Scale");
  glEnable(GL_BLEND);

  // Set render target to the backbuffer:
  glViewport(0, 0, ViewportWidth, ViewportHeight);
  glBindFramebuffer(GL_FRAMEBUFFER, windowFbo);
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);

  // Draw ink:
  glBindTexture(GL_TEXTURE_2D, Density.Ping.TextureHandle);
  glUniform3f(fillColor, 1, 1, 1);
  glUniform2f(scale, 1.0f / ViewportWidth, 1.0f / ViewportHeight);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Draw obstacles:
  glBindTexture(GL_TEXTURE_2D, HiresObstacles.TextureHandle);
  glUniform3f(fillColor, 0.125f, 0.4f, 0.75f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Disable blending:
  glDisable(GL_BLEND);
  //}
}



void FluidSim::resize()
{
  CellSize = 1.25f;
  vmath::Vector2 size = RenderManager::getInstance().getFramebufferSize();
  ViewportWidth = (int)size.x;
  ViewportHeight = (int)size.y;
  GridWidth = ViewportWidth / 2;
  GridHeight = ViewportHeight / 2;
  SplatRadius = ((float)GridWidth * 0.02f);

  //ViewportWidth =512;
  //ViewportHeight = 512;
  //GridWidth = ViewportWidth / 2;
  //GridHeight = ViewportHeight / 2;
  //SplatRadius = ((float)GridWidth / 8.0f);
}
void FluidSim::postRender()
{


}



void FluidSim::handleKey(KeyInfo& key)
{

  if(key.key == ' ' && key.action == KeyInfo::KeyAction::PRESS)
  {

  }
  if(key.key == ' ' && key.action == KeyInfo::KeyAction::RELEASE)
  {

  }
  //264 == down arrow
  if(key.key == 264 && (key.action == KeyInfo::KeyAction::REPEAT
                        || key.action == KeyInfo::KeyAction::PRESS))
  {


  }
  //264 == up arrow
  if(key.key == 265 && (key.action == KeyInfo::KeyAction::REPEAT
                        || key.action == KeyInfo::KeyAction::PRESS))
  {

  }

  if((key.key == 'd' || key.key == 'D' )&& key.action == KeyInfo::KeyAction::PRESS)
  {


  }


  if(key.action == KeyInfo::KeyAction::PRESS)
  {
    switch (key.key)
    {
      case ',':
      case '<':

        break;
      case '.':
      case '>':

        break;
      case '/':
      case '?':

        break;
    }
  }
}

void FluidSim::handlePointer(std::vector<PointerInfo>& pointers)
{
  _pointer = pointers[0];
  std::cout << "Pointer event " << _pointer.pos.x << "," << _pointer.pos.y
              << " " << _pointer.down << " " << _pointer.pressure << std::endl;
}


void FluidSim::setupShaders()
{
  /*
  std::vector<Shader*> shaders = {
    _advectShader,
    _bouyancyShader,
    _computeDivergenceShader,
    _fillShader,
    _jacobiShader,
    _splatShader,
    _subtractGradientShader,
    _visualizeShader
  };
  std::vector<std::string> fragShaders = {
    "shaders/fluids/Advect.frag",
    "shaders/fluids/Bouyancy.frag",
    "shaders/fluids/ComputeDivergence.frag",
    "shaders/fluids/FillRed.frag",
    "shaders/fluids/Jacobi.frag",
    "shaders/fluids/Splat.frag",
    "shaders/fluids/SubtractGradient.frag",
    "shaders/fluids/Visualize.frag"
  };
  std::string vertShader = "shaders/fluids/PassThrough.vert";


  int idx = 0;
  for(Shader* shader : shaders)
  {
    shader = new Shader();
    shader->registerShader(vertShader, ShaderType::VERTEX);
    shader->registerShader(fragShaders[idx++], ShaderType::FRAGMENT);
    shader->setAttributeLocations(Mesh::getShaderAttributeLocations());
    shader->compileShaders();
    shader->linkShaders();
  }
   */

}

  GLuint FluidSim::CreateProgram(const char* vsKey, const char* gsKey, const char* fsKey)
  {
    Shader* shader = new Shader();
    shader->registerShader(vsKey, ShaderType::VERTEX);
    shader->registerShader(fsKey, ShaderType::FRAGMENT);
    shader->setAttributeLocations(Mesh::getShaderAttributeLocations());
    shader->compileShaders();
    shader->linkShaders();

    _shaders.push_back(shader);
    return shader->getID();
  }



  GLuint FluidSim::CreateQuad()
  {
    short positions[] = {
      -1, -1,
      1, -1,
      -1,  1,
      1,  1,
    };

    // Create the VAO:
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create the VBO:
    GLuint vbo;
    GLsizeiptr size = sizeof(positions);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);

    // Set up the vertex layout:
    GLsizeiptr stride = 2 * sizeof(positions[0]);
    glEnableVertexAttribArray(PositionSlot);
    glVertexAttribPointer(PositionSlot, 2, GL_SHORT, GL_FALSE, stride, 0);

    return vao;
  }

  FluidSim::Slab FluidSim::CreateSlab(GLsizei width, GLsizei height, int numComponents)
  {
    Slab slab;
    slab.Ping = CreateSurface(width, height, numComponents);
    slab.Pong = CreateSurface(width, height, numComponents);
    return slab;
  }

  FluidSim::Surface FluidSim::CreateSurface(GLsizei width, GLsizei height, int numComponents)
  {
    GLuint fboHandle;
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    const int UseHalfFloats = 1;
    if (UseHalfFloats) {
      switch (numComponents) {
        case 1: glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_HALF_FLOAT, 0); break;
        case 2: glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_HALF_FLOAT, 0); break;
        case 3: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_HALF_FLOAT, 0); break;
        case 4: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, 0); break;
        default: throw std::runtime_error("Illegal slab format.");
      }
    } else {
      switch (numComponents) {
        case 1: glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, 0); break;
        case 2: glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, width, height, 0, GL_RG, GL_FLOAT, 0); break;
        case 3: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, 0); break;
        case 4: glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0); break;
        default: throw std::runtime_error("Illegal slab format.");
      }
    }

    PezCheckCondition(GL_NO_ERROR == glGetError(), "Unable to create normals texture");

    GLuint colorbuffer;
    glGenRenderbuffers(1, &colorbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHandle, 0);
    PezCheckCondition(GL_NO_ERROR == glGetError(), "Unable to attach color buffer");

    PezCheckCondition(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER), "Unable to create FBO.");
    Surface surface = { fboHandle, textureHandle, numComponents };

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return surface;
  }

  void FluidSim::CreateObstacles(Surface dest, int width, int height)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glViewport(0, 0, width, height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint program = CreateProgram("shaders/fluids/Vertex.vert", 0, "shaders/fluids/FillRed.frag");
    glUseProgram(program);

    const int DrawBorder = 1;
    if (DrawBorder) {
#define T 0.9999f
      float positions[] = { -T, -T, T, -T, T,  T, -T,  T, -T, -T };
#undef T
      GLuint vbo;
      GLsizeiptr size = sizeof(positions);
      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
      GLsizeiptr stride = 2 * sizeof(positions[0]);
      glEnableVertexAttribArray(PositionSlot);
      glVertexAttribPointer(PositionSlot, 2, GL_FLOAT, GL_FALSE, stride, 0);
      glDrawArrays(GL_LINE_STRIP, 0, 5);
      glDeleteBuffers(1, &vbo);
    }

    const int DrawCircle = 1;
    if (DrawCircle) {
      const int slices = 64;
      float positions[slices*2*3];
      float twopi = 8*atan(1.0f);
      float theta = 0;
      float dtheta = twopi / (float) (slices - 1);
      float* pPositions = &positions[0];
      for (int i = 0; i < slices; i++) {
        *pPositions++ = 0;
        *pPositions++ = 0;

        *pPositions++ = 0.25f * cos(theta) * height / width;
        *pPositions++ = 0.25f * sin(theta);
        theta += dtheta;

        *pPositions++ = 0.25f * cos(theta) * height / width;
        *pPositions++ = 0.25f * sin(theta);
      }
      GLuint vbo;
      GLsizeiptr size = sizeof(positions);
      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
      GLsizeiptr stride = 2 * sizeof(positions[0]);
      glEnableVertexAttribArray(PositionSlot);
      glVertexAttribPointer(PositionSlot, 2, GL_FLOAT, GL_FALSE, stride, 0);
      glDrawArrays(GL_TRIANGLES, 0, slices * 3);
      glDeleteBuffers(1, &vbo);
    }

    // Cleanup
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);

  }

  void FluidSim::ResetState()
  {
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
  }

  void FluidSim::InitSlabOps()
  {
    Programs.Advect = CreateProgram("shaders/fluids/Vertex.vert", 0, "shaders/fluids/Advect.frag");
    Programs.Jacobi = CreateProgram("shaders/fluids/Vertex.vert", 0, "shaders/fluids/Jacobi.frag");
    Programs.SubtractGradient = CreateProgram("shaders/fluids/Vertex.vert", 0, "shaders/fluids/SubtractGradient.frag");
    Programs.ComputeDivergence = CreateProgram("shaders/fluids/Vertex.vert", 0, "shaders/fluids/ComputeDivergence.frag");
    Programs.ApplyImpulse = CreateProgram("shaders/fluids/Vertex.vert", 0, "shaders/fluids/Splat.frag");
    Programs.ApplyBuoyancy = CreateProgram("shaders/fluids/Vertex.vert", 0, "shaders/fluids/Buoyancy.frag");
  }

  void FluidSim::SwapSurfaces(Slab* slab)
  {
    Surface temp = slab->Ping;
    slab->Ping = slab->Pong;
    slab->Pong = temp;
  }

  void FluidSim::ClearSurface(Surface s, float v)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, s.FboHandle);
    glClearColor(v, v, v, v);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void FluidSim::Advect(Surface velocity, Surface source, Surface obstacles, Surface dest, float dissipation)
  {
    GLuint p = Programs.Advect;
    glUseProgram(p);

    GLint inverseSize = glGetUniformLocation(p, "InverseSize");
    GLint timeStep = glGetUniformLocation(p, "TimeStep");
    GLint dissLoc = glGetUniformLocation(p, "Dissipation");
    GLint sourceTexture = glGetUniformLocation(p, "SourceTexture");
    GLint obstaclesTexture = glGetUniformLocation(p, "Obstacles");

    glUniform2f(inverseSize, 1.0f / GridWidth, 1.0f / GridHeight);
    glUniform1f(timeStep, TimeStep);
    glUniform1f(dissLoc, dissipation);
    glUniform1i(sourceTexture, 1);
    glUniform1i(obstaclesTexture, 2);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, source.TextureHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, obstacles.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
  }

  void FluidSim::Jacobi(Surface pressure, Surface divergence, Surface obstacles, Surface dest)
  {
    GLuint p = Programs.Jacobi;
    glUseProgram(p);

    GLint alpha = glGetUniformLocation(p, "Alpha");
    GLint inverseBeta = glGetUniformLocation(p, "InverseBeta");
    GLint dSampler = glGetUniformLocation(p, "Divergence");
    GLint oSampler = glGetUniformLocation(p, "Obstacles");

    glUniform1f(alpha, -CellSize * CellSize);
    glUniform1f(inverseBeta, 0.25f);
    glUniform1i(dSampler, 1);
    glUniform1i(oSampler, 2);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, pressure.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, divergence.TextureHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, obstacles.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
  }

  void FluidSim::SubtractGradient(Surface velocity, Surface pressure, Surface obstacles, Surface dest)
  {
    GLuint p = Programs.SubtractGradient;
    glUseProgram(p);

    GLint gradientScale = glGetUniformLocation(p, "GradientScale");
    glUniform1f(gradientScale, GradientScale);
    GLint halfCell = glGetUniformLocation(p, "HalfInverseCellSize");
    glUniform1f(halfCell, 0.5f / CellSize);
    GLint sampler = glGetUniformLocation(p, "Pressure");
    glUniform1i(sampler, 1);
    sampler = glGetUniformLocation(p, "Obstacles");
    glUniform1i(sampler, 2);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pressure.TextureHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, obstacles.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
  }

  void FluidSim::ComputeDivergence(Surface velocity, Surface obstacles, Surface dest)
  {
    GLuint p = Programs.ComputeDivergence;
    glUseProgram(p);

    GLint halfCell = glGetUniformLocation(p, "HalfInverseCellSize");
    glUniform1f(halfCell, 0.5f / CellSize);
    GLint sampler = glGetUniformLocation(p, "Obstacles");
    glUniform1i(sampler, 1);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, obstacles.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
  }

  void FluidSim::ApplyImpulse(Surface dest, Vector2i position, float value)
  {
    GLuint p = Programs.ApplyImpulse;
    glUseProgram(p);

    GLint pointLoc = glGetUniformLocation(p, "Point");
    GLint radiusLoc = glGetUniformLocation(p, "Radius");
    GLint fillColorLoc = glGetUniformLocation(p, "FillColor");

    float height = RenderManager::getInstance().getFramebufferSize().y;
    glUniform2f(pointLoc, (float) _pointer.pos.x * 0.5f, (height - _pointer.pos.y) * 0.5f);
    //glUniform2f(pointLoc, (float) position.X, (float) position.Y);
    glUniform1f(radiusLoc, SplatRadius);
    glUniform3f(fillColorLoc, value, value, value);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glEnable(GL_BLEND);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
  }

  void FluidSim::ApplyBuoyancy(Surface velocity, Surface temperature, Surface density, Surface dest)
  {
    GLuint p = Programs.ApplyBuoyancy;
    glUseProgram(p);

    GLint tempSampler = glGetUniformLocation(p, "Temperature");
    GLint inkSampler = glGetUniformLocation(p, "Density");
    GLint ambTemp = glGetUniformLocation(p, "AmbientTemperature");
    GLint timeStep = glGetUniformLocation(p, "TimeStep");
    GLint sigma = glGetUniformLocation(p, "Sigma");
    GLint kappa = glGetUniformLocation(p, "Kappa");

    glUniform1i(tempSampler, 1);
    glUniform1i(inkSampler, 2);
    glUniform1f(ambTemp, AmbientTemperature);
    glUniform1f(timeStep, TimeStep);
    glUniform1f(sigma, SmokeBuoyancy);
    glUniform1f(kappa, SmokeWeight);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, velocity.TextureHandle);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, temperature.TextureHandle);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, density.TextureHandle);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    ResetState();
  }


}// namespace renderlib
