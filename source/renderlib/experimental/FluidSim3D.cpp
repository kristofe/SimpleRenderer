//
//  FluidSim3D.cpp
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#include <math.h>
#include "FluidSim3D.h"
#include "mesh.h"
#include "Texture.h"
#include "ImageUtils.h"
#include "ObjectIDGenerator.h"
#include "RenderManager.h"

/*
PezConfig PezGetConfig()
{
  PezConfig config;
  config.Title = "Fluid3d";
  config.Width = ViewportWidth;
  config.Height = ViewportHeight;
  config.Multisampling = 0;
  config.VerticalSync = 0;
  return config;
}
 */

namespace renderlib{

using namespace vmath;

FluidSim3D::FluidSim3D()
{
  _renderSortValue = IRenderable::DefaultSortValue;
}

FluidSim3D::~FluidSim3D()
{

}

void FluidSim3D::init()
{
  PezConfig cfg = PezGetConfig();

  Trackball = CreateTrackball(cfg.Width * 1.0f, cfg.Height * 1.0f, cfg.Width * 0.5f);
  RaycastProgram = LoadProgram("Raycast.VS", "Raycast.GS", "Raycast.FS");
  Vbos.CubeCenter = CreatePointVbo(0, 0, 0);
  Vbos.FullscreenQuad = CreateQuadVbo();

  Slabs.Velocity = CreateSlab(GridWidth, GridHeight, GridDepth, 3);
  Slabs.Density = CreateSlab(GridWidth, GridHeight, GridDepth, 1);
  Slabs.Pressure = CreateSlab(GridWidth, GridHeight, GridDepth, 1);
  Slabs.Temperature = CreateSlab(GridWidth, GridHeight, GridDepth, 1);
  Surfaces.Divergence = CreateVolume(GridWidth, GridHeight, GridDepth, 3);
  InitSlabOps();
  Surfaces.Obstacles = CreateVolume(GridWidth, GridHeight, GridDepth, 3);
  CreateObstacles(Surfaces.Obstacles);
  ClearSurface(Slabs.Temperature.Ping, AmbientTemperature);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnableVertexAttribArray(SlotPosition);


}

void FluidSim3D::update(float time)
{
  float dt = microseconds * 0.000001f;
  Trackball->Update(microseconds);
  EyePosition = Point3(0, 0, 3.5f + Trackball->GetZoom());
  Vector3 up(0, 1, 0); Point3 target(0);
  Matrices.View = Matrix4::lookAt(EyePosition, target, up);
  Matrix4 modelMatrix(transpose(Trackball->GetRotation()), Vector3(0));
  modelMatrix *= Matrix4::rotationY(0.5f);
  Matrices.Modelview = Matrices.View * modelMatrix;

  Matrices.Projection = Matrix4::perspective(
                                             FieldOfView,
                                             float(ViewportWidth) / ViewportHeight, // Aspect Ratio
                                             0.0f,   // Near Plane
                                             1.0f);  // Far Plane

  Matrices.ModelviewProjection = Matrices.Projection * Matrices.Modelview;

  if (SimulateFluid) {
    glBindBuffer(GL_ARRAY_BUFFER, Vbos.FullscreenQuad);
    glVertexAttribPointer(SlotPosition, 2, GL_SHORT, GL_FALSE, 2 * sizeof(short), 0);
    glViewport(0, 0, GridWidth, GridHeight);
    Advect(Slabs.Velocity.Ping, Slabs.Velocity.Ping, Surfaces.Obstacles, Slabs.Velocity.Pong, VelocityDissipation);
    SwapSurfaces(&Slabs.Velocity);
    Advect(Slabs.Velocity.Ping, Slabs.Temperature.Ping, Surfaces.Obstacles, Slabs.Temperature.Pong, TemperatureDissipation);
    SwapSurfaces(&Slabs.Temperature);
    Advect(Slabs.Velocity.Ping, Slabs.Density.Ping, Surfaces.Obstacles, Slabs.Density.Pong, DensityDissipation);
    SwapSurfaces(&Slabs.Density);
    ApplyBuoyancy(Slabs.Velocity.Ping, Slabs.Temperature.Ping, Slabs.Density.Ping, Slabs.Velocity.Pong);
    SwapSurfaces(&Slabs.Velocity);
    ApplyImpulse(Slabs.Temperature.Ping, ImpulsePosition, ImpulseTemperature);
    ApplyImpulse(Slabs.Density.Ping, ImpulsePosition, ImpulseDensity);
    ComputeDivergence(Slabs.Velocity.Ping, Surfaces.Obstacles, Surfaces.Divergence);
    ClearSurface(Slabs.Pressure.Ping, 0);
    for (int i = 0; i < NumJacobiIterations; ++i) {
      Jacobi(Slabs.Pressure.Ping, Surfaces.Divergence, Surfaces.Obstacles, Slabs.Pressure.Pong);
      SwapSurfaces(&Slabs.Pressure);
    }
    SubtractGradient(Slabs.Velocity.Ping, Slabs.Pressure.Ping, Surfaces.Obstacles, Slabs.Velocity.Pong);
    SwapSurfaces(&Slabs.Velocity);
  }
}

void FluidSim3D::preRender()
{


}

void FluidSim3D::simulate()
{
}


void FluidSim3D::draw()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  PezConfig cfg = PezGetConfig();
  glViewport(0, 0, cfg.Width, cfg.Height);
  glClearColor(0, 0.125f, 0.25f, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBindBuffer(GL_ARRAY_BUFFER, Vbos.CubeCenter);
  glVertexAttribPointer(SlotPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
  glBindTexture(GL_TEXTURE_3D, Slabs.Density.Ping.ColorTexture);
  glUseProgram(RaycastProgram);
  SetUniform("ModelviewProjection", Matrices.ModelviewProjection);
  SetUniform("Modelview", Matrices.Modelview);
  SetUniform("ViewMatrix", Matrices.View);
  SetUniform("ProjectionMatrix", Matrices.Projection);
  SetUniform("RayStartPoints", 1);
  SetUniform("RayStopPoints", 2);
  SetUniform("EyePosition", EyePosition);
  SetUniform("RayOrigin", Vector4(transpose(Matrices.Modelview) * EyePosition).getXYZ());
  SetUniform("FocalLength", 1.0f / std::tan(FieldOfView / 2));
  SetUniform("WindowSize", float(cfg.Width), float(cfg.Height));
  glDrawArrays(GL_POINTS, 0, 1);
}



void FluidSim3D::resize()
{

}
void FluidSim3D::postRender()
{


}



void FluidSim3D::handleKey(KeyInfo& key)
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

void FluidSim3D::handlePointer(std::vector<PointerInfo>& pointers)
{
  _pointer = pointers[0];
//  std::cout << "Pointer event " << _pointer.pos.x << "," << _pointer.pos.y
//              << " " << _pointer.down << " " << _pointer.pressure << std::endl;
}


void FluidSim3D::setupShaders()
{


}

  GLuint FluidSim3D::CreateProgram(const char* vsKey, const char* gsKey, const char* fsKey)
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





  void FluidSim3D::CreateObstacles(SurfacePod dest)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glViewport(0, 0, dest.Width, dest.Height);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    GLuint program = LoadProgram("Fluid.Vertex", 0, "Fluid.Fill");
    glUseProgram(program);

    GLuint lineVbo;
    glGenBuffers(1, &lineVbo);
    GLuint circleVbo;
    glGenBuffers(1, &circleVbo);
    glEnableVertexAttribArray(SlotPosition);

    for (int slice = 0; slice < dest.Depth; ++slice) {

      glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dest.ColorTexture, 0, dest.Depth - 1 - slice);
      float z = dest.Depth / 2.0f;
      z = abs(slice - z) / z;
      float fraction = 1 - sqrt(z);
      float radius = 0.5f * fraction;

      if (slice == 0 || slice == dest.Depth - 1) {
        radius *= 100;
      }

      const bool DrawBorder = true;
      if (DrawBorder && slice != 0 && slice != dest.Depth - 1) {
#define T 0.9999f
        float positions[] = { -T, -T, T, -T, T,  T, -T,  T, -T, -T };
#undef T
        GLsizeiptr size = sizeof(positions);
        glBindBuffer(GL_ARRAY_BUFFER, lineVbo);
        glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
        GLsizeiptr stride = 2 * sizeof(positions[0]);
        glVertexAttribPointer(SlotPosition, 2, GL_FLOAT, GL_FALSE, stride, 0);
        glDrawArrays(GL_LINE_STRIP, 0, 5);
      }

      const bool DrawSphere = false;
      if (DrawSphere || slice == 0 || slice == dest.Depth - 1) {
        const int slices = 64;
        float positions[slices*2*3];
        float twopi = 8*atan(1.0f);
        float theta = 0;
        float dtheta = twopi / (float) (slices - 1);
        float* pPositions = &positions[0];
        for (int i = 0; i < slices; i++) {
          *pPositions++ = 0;
          *pPositions++ = 0;

          *pPositions++ = radius * cos(theta);
          *pPositions++ = radius * sin(theta);
          theta += dtheta;

          *pPositions++ = radius * cos(theta);
          *pPositions++ = radius * sin(theta);
        }
        GLsizeiptr size = sizeof(positions);
        glBindBuffer(GL_ARRAY_BUFFER, circleVbo);
        glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
        GLsizeiptr stride = 2 * sizeof(positions[0]);
        glVertexAttribPointer(SlotPosition, 2, GL_FLOAT, GL_FALSE, stride, 0);
        glDrawArrays(GL_TRIANGLES, 0, slices * 3);
      }
    }

    // Cleanup
    glDeleteProgram(program);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &lineVbo);
    glDeleteBuffers(1, &circleVbo);
  }

  SlabPod FluidSim3D::CreateSlab(GLsizei width, GLsizei height, GLsizei depth, int numComponents)
  {
    SlabPod slab;
    slab.Ping = CreateVolume(width, height, depth, numComponents);
    slab.Pong = CreateVolume(width, height, depth, numComponents);
    return slab;
  }

  SurfacePod FluidSim3D::CreateSurface(GLsizei width, GLsizei height, int numComponents)
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

    switch (numComponents) {
      case 1:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, width, height, 0, GL_RED, GL_HALF_FLOAT, 0);
        break;
      case 2:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, width, height, 0, GL_RG, GL_HALF_FLOAT, 0);
        break;
      case 3:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_HALF_FLOAT, 0);
        break;
      case 4:
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, 0);
        break;
    }

    PezCheckCondition(GL_NO_ERROR == glGetError(), "Unable to create normals texture");

    GLuint colorbuffer;
    glGenRenderbuffers(1, &colorbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHandle, 0);
    PezCheckCondition(GL_NO_ERROR == glGetError(), "Unable to attach color buffer");

    PezCheckCondition(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER), "Unable to create FBO.");
    SurfacePod surface = { fboHandle, textureHandle };

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    surface.Width = width;
    surface.Height = height;
    surface.Depth = 1;
    return surface;
  }

  SurfacePod FluidSim3D::CreateVolume(GLsizei width, GLsizei height, GLsizei depth, int numComponents)
  {
    GLuint fboHandle;
    glGenFramebuffers(1, &fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);

    GLuint textureHandle;
    glGenTextures(1, &textureHandle);
    glBindTexture(GL_TEXTURE_3D, textureHandle);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    switch (numComponents) {
      case 1:
        glTexImage3D(GL_TEXTURE_3D, 0, GL_R16F, width, height, depth, 0, GL_RED, GL_HALF_FLOAT, 0);
        break;
      case 2:
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RG16F, width, height, depth, 0, GL_RG, GL_HALF_FLOAT, 0);
        break;
      case 3:
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB16F, width, height, depth, 0, GL_RGB, GL_HALF_FLOAT, 0);
        break;
      case 4:
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16F, width, height, depth, 0, GL_RGBA, GL_HALF_FLOAT, 0);
        break;
    }

    PezCheckCondition(GL_NO_ERROR == glGetError(), "Unable to create volume texture");

    GLuint colorbuffer;
    glGenRenderbuffers(1, &colorbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, colorbuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureHandle, 0);
    PezCheckCondition(GL_NO_ERROR == glGetError(), "Unable to attach color buffer");

    PezCheckCondition(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER), "Unable to create FBO.");
    SurfacePod surface = { fboHandle, textureHandle };

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    surface.Width = width;
    surface.Height = height;
    surface.Depth = depth;
    return surface;
  }

  void FluidSim3D::ResetState()
  {
    glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_3D, 0);
    glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_3D, 0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_3D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_BLEND);
  }

  void FluidSim3D::InitSlabOps()
  {
    Programs.Advect = LoadProgram("Fluid.Vertex", "Fluid.PickLayer", "Fluid.Advect");
    Programs.Jacobi = LoadProgram("Fluid.Vertex", "Fluid.PickLayer", "Fluid.Jacobi");
    Programs.SubtractGradient = LoadProgram("Fluid.Vertex", "Fluid.PickLayer", "Fluid.SubtractGradient");
    Programs.ComputeDivergence = LoadProgram("Fluid.Vertex", "Fluid.PickLayer", "Fluid.ComputeDivergence");
    Programs.ApplyImpulse = LoadProgram("Fluid.Vertex", "Fluid.PickLayer", "Fluid.Splat");
    Programs.ApplyBuoyancy = LoadProgram("Fluid.Vertex", "Fluid.PickLayer", "Fluid.Buoyancy");
  }

  void FluidSim3D::SwapSurfaces(SlabPod* slab)
  {
    SurfacePod temp = slab->Ping;
    slab->Ping = slab->Pong;
    slab->Pong = temp;
  }

  void FluidSim3D::ClearSurface(SurfacePod s, float v)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, s.FboHandle);
    glClearColor(v, v, v, v);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  void FluidSim3D::Advect(SurfacePod velocity, SurfacePod source, SurfacePod obstacles, SurfacePod dest, float dissipation)
  {
    GLuint p = Programs.Advect;
    glUseProgram(p);

    SetUniform("InverseSize", recipPerElem(Vector3(float(GridWidth), float(GridHeight), float(GridDepth))));
    SetUniform("TimeStep", TimeStep);
    SetUniform("Dissipation", dissipation);
    SetUniform("SourceTexture", 1);
    SetUniform("Obstacles", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, velocity.ColorTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, source.ColorTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, obstacles.ColorTexture);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, dest.Depth);
    ResetState();
  }

  void FluidSim3D::Jacobi(SurfacePod pressure, SurfacePod divergence, SurfacePod obstacles, SurfacePod dest)
  {
    GLuint p = Programs.Jacobi;
    glUseProgram(p);

    SetUniform("Alpha", -CellSize * CellSize);
    SetUniform("InverseBeta", 0.1666f);
    SetUniform("Divergence", 1);
    SetUniform("Obstacles", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, pressure.ColorTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, divergence.ColorTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, obstacles.ColorTexture);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, dest.Depth);
    ResetState();
  }

  void FluidSim3D::SubtractGradient(SurfacePod velocity, SurfacePod pressure, SurfacePod obstacles, SurfacePod dest)
  {
    GLuint p = Programs.SubtractGradient;
    glUseProgram(p);

    SetUniform("GradientScale", GradientScale);
    SetUniform("HalfInverseCellSize", 0.5f / CellSize);
    SetUniform("Pressure", 1);
    SetUniform("Obstacles", 2);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, velocity.ColorTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, pressure.ColorTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, obstacles.ColorTexture);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, dest.Depth);
    ResetState();
  }

  void FluidSim3D::ComputeDivergence(SurfacePod velocity, SurfacePod obstacles, SurfacePod dest)
  {
    GLuint p = Programs.ComputeDivergence;
    glUseProgram(p);

    SetUniform("HalfInverseCellSize", 0.5f / CellSize);
    SetUniform("Obstacles", 1);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, velocity.ColorTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, obstacles.ColorTexture);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, dest.Depth);
    ResetState();
  }

  void FluidSim3D::ApplyImpulse(SurfacePod dest, Vector3 position, float value)
  {
    GLuint p = Programs.ApplyImpulse;
    glUseProgram(p);

    SetUniform("Point", position);
    SetUniform("Radius", SplatRadius);
    SetUniform("FillColor", Vector3(value, value, value));

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glEnable(GL_BLEND);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, dest.Depth);
    ResetState();
  }

  void FluidSim3D::ApplyBuoyancy(SurfacePod velocity, SurfacePod temperature, SurfacePod density, SurfacePod dest)
  {
    GLuint p = Programs.ApplyBuoyancy;
    glUseProgram(p);

    SetUniform("Temperature", 1);
    SetUniform("Density", 2);
    SetUniform("AmbientTemperature", AmbientTemperature);
    SetUniform("TimeStep", TimeStep);
    SetUniform("Sigma", SmokeBuoyancy);
    SetUniform("Kappa", SmokeWeight);

    glBindFramebuffer(GL_FRAMEBUFFER, dest.FboHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_3D, velocity.ColorTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, temperature.ColorTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, density.ColorTexture);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, dest.Depth);
    ResetState();
  }

  GLuint FluidSim3D::CreatePointVbo(float x, float y, float z)
  {
    float p[] = {x, y, z};
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(p), &p[0], GL_STATIC_DRAW);
    return vbo;
  }

  void FluidSim3D::SetUniform(const char* name, int value)
  {
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform1i(location, value);
  }

  void FluidSim3D::SetUniform(const char* name, float value)
  {
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform1f(location, value);
  }

  void FluidSim3D::SetUniform(const char* name, Matrix4 value)
  {
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniformMatrix4fv(location, 1, 0, (float*) &value);
  }

  void FluidSim3D::SetUniform(const char* name, Matrix3 nm)
  {
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    float packed[9] = {
      nm.getRow(0).getX(), nm.getRow(1).getX(), nm.getRow(2).getX(),
      nm.getRow(0).getY(), nm.getRow(1).getY(), nm.getRow(2).getY(),
      nm.getRow(0).getZ(), nm.getRow(1).getZ(), nm.getRow(2).getZ() };
    glUniformMatrix3fv(location, 1, 0, &packed[0]);
  }

  void FluidSim3D::SetUniform(const char* name, Vector3 value)
  {
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform3f(location, value.getX(), value.getY(), value.getZ());
  }

  void FluidSim3D::SetUniform(const char* name, float x, float y)
  {
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform2f(location, x, y);
  }

  void FluidSim3D::SetUniform(const char* name, Vector4 value)
  {
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform4f(location, value.getX(), value.getY(), value.getZ(), value.getW());
  }

  void FluidSim3D::SetUniform(const char* name, Point3 value)
  {
    GLuint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*) &program);
    GLint location = glGetUniformLocation(program, name);
    glUniform3f(location, value.getX(), value.getY(), value.getZ());
  }

  GLuint FluidSim3D::CreateQuadVbo()
  {
    short positions[] = {
      -1, -1,
      1, -1,
      -1,  1,
      1,  1,
    };
    GLuint vbo;
    GLsizeiptr size = sizeof(positions);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, size, positions, GL_STATIC_DRAW);
    return vbo;
  }


}// namespace renderlib
