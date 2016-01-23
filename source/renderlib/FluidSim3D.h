#ifndef __Splash__FluidSim3D__
#define __Splash__FluidSim3D__

#include <iostream>
#include "InputManager.h"
#include "vmath.hpp"
#include "IRenderable.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "Texture.h"


#define PezCheckCondition(A, B) if (!(A)) { throw std::runtime_error(B); }

using namespace vmath;

namespace renderlib {
  
  class Mesh;
  class Shader;
  class TextureProxy;
  
  class FluidSim3D : public IRenderable
  {
  public:
    static struct {
      GLuint Advect;
      GLuint Jacobi;
      GLuint SubtractGradient;
      GLuint ComputeDivergence;
      GLuint ApplyImpulse;
      GLuint ApplyBuoyancy;
    } Programs;
    
    enum AttributeSlot {
      SlotPosition,
      SlotTexCoord,
    };
    
    struct ITrackball {
      virtual void MouseDown(int x, int y) = 0;
      virtual void MouseUp(int x, int y) = 0;
      virtual void MouseMove(int x, int y) = 0;
      virtual void ReturnHome() = 0;
      virtual vmath::Matrix3 GetRotation() const = 0;
      virtual float GetZoom() const = 0;
      virtual void Update(unsigned int microseconds) = 0;
    };
    
    struct TexturePod {
      GLuint Handle;
      GLsizei Width;
      GLsizei Height;
    };
    
    struct SurfacePod {
      GLuint FboHandle;
      GLuint ColorTexture;
      GLsizei Width;
      GLsizei Height;
      GLsizei Depth;
    };
    
    struct SlabPod {
      SurfacePod Ping;
      SurfacePod Pong;
    };
    
    static struct {
      SlabPod Velocity;
      SlabPod Density;
      SlabPod Pressure;
      SlabPod Temperature;
    } Slabs;
    
    static struct {
      SurfacePod Divergence;
      SurfacePod Obstacles;
      SurfacePod HiresObstacles;
    } Surfaces;
    
    static struct {
      Matrix4 Projection;
      Matrix4 Modelview;
      Matrix4 View;
      Matrix4 ModelviewProjection;
    } Matrices;
    
    static struct {
      GLuint CubeCenter;
      GLuint FullscreenQuad;
    } Vbos;

    
    
  public:
    FluidSim3D();
    virtual ~FluidSim3D();
    
    //IRenderable methods
    void init();
    void update(float time);
    void fixedUpdate(){};
    
    void preRender();
    void draw();
    void postRender();

    //IControllable methods
    virtual void handleKey(KeyInfo& key);
    virtual void handlePointer(std::vector<PointerInfo>& pointers);
    uint32_t getID() { return _id;};
    
    void  simulate();
    int  getRenderSortValue() {return _renderSortValue;};
    uint32_t  getRenderObjectID() {return _id;};
    void  setRenderObjectID(uint32_t id){ _id = id;};
    void  setupShaders();
    void  resize();

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    FluidSim3D(const FluidSim3D&) = delete;
    FluidSim3D & operator=(const FluidSim3D&) = delete;
    

    
    const int PositionSlot = 0;
    
    ITrackball* CreateTrackball(float width, float height, float radius);
    GLuint LoadProgram(const char* vsKey, const char* gsKey, const char* fsKey);
    void SetUniform(const char* name, int value);
    void SetUniform(const char* name, float value);
    void SetUniform(const char* name, float x, float y);
    void SetUniform(const char* name, vmath::Matrix4 value);
    void SetUniform(const char* name, vmath::Matrix3 value);
    void SetUniform(const char* name, vmath::Vector3 value);
    void SetUniform(const char* name, vmath::Point3 value);
    void SetUniform(const char* name, vmath::Vector4 value);
    TexturePod LoadTexture(const char* path);
    SurfacePod CreateSurface(int width, int height, int numComponents = 4);
    SurfacePod CreateVolume(int width, int height, int depth, int numComponents = 4);
    GLuint CreatePointVbo(float x, float y, float z);
    GLuint CreateQuadVbo();
    void CreateObstacles(SurfacePod dest);
    SlabPod CreateSlab(GLsizei width, GLsizei height, GLsizei depth, int numComponents);
    void InitSlabOps();
    void SwapSurfaces(SlabPod* slab);
    void ClearSurface(SurfacePod s, float v);
    void Advect(SurfacePod velocity, SurfacePod source, SurfacePod obstacles, SurfacePod dest, float dissipation);
    void Jacobi(SurfacePod pressure, SurfacePod divergence, SurfacePod obstacles, SurfacePod dest);
    void SubtractGradient(SurfacePod velocity, SurfacePod pressure, SurfacePod obstacles, SurfacePod dest);
    void ComputeDivergence(SurfacePod velocity, SurfacePod obstacles, SurfacePod dest);
    void ApplyImpulse(SurfacePod dest, vmath::Vector3 position, float value);
    void ApplyBuoyancy(SurfacePod velocity, SurfacePod temperature, SurfacePod density, SurfacePod dest);
    
    const float CellSize = 1.25f;
    const int ViewportWidth = 320;
    const int GridWidth = 96;
    const int ViewportHeight = ViewportWidth;
    const int GridHeight = GridWidth;
    const int GridDepth = GridWidth;
    const float SplatRadius = GridWidth / 8.0f;
    const float AmbientTemperature = 0.0f;
    const float ImpulseTemperature = 10.0f;
    const float ImpulseDensity = 1.0f;
    const int NumJacobiIterations = 40;
    const float TimeStep = 0.25f;
    const float SmokeBuoyancy = 1.0f;
    const float SmokeWeight = 0.0125f;
    const float GradientScale = 1.125f / CellSize;
    const float TemperatureDissipation = 0.99f;
    const float VelocityDissipation = 0.99f;
    const float DensityDissipation = 0.9995f;
    const Vector3 ImpulsePosition( GridWidth / 2.0f, GridHeight - (int) SplatRadius / 2.0f, GridDepth / 2.0f);
    
  protected:
    int _renderSortValue;
    uint32_t _id;
    PointerInfo _pointer;
    
    std::vector<Shader*> _shaders;
    
    
    ITrackball* Trackball;
    Point3 EyePosition;
    GLuint RaycastProgram;
    float FieldOfView = 0.7f;
    bool SimulateFluid = true;
    
  };

} // namespace renderlib
#endif /* defined(__Splash__FluidSim3D__) */
