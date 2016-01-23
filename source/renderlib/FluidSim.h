#ifndef __Splash__FluidSim__
#define __Splash__FluidSim__

#include <iostream>
#include "InputManager.h"
#include "vmath.hpp"
#include "IRenderable.h"
#include "Texture.h"
#include "RenderTexture.h"
#include "Texture.h"



//#define CellSize (1.25f)
//#define ViewportWidth (512)
//#define ViewportHeight (512)
//#define GridWidth (ViewportWidth / 2)
//#define GridHeight (ViewportHeight / 2)
//#define SplatRadius ((float) GridWidth / 8.0f)

#define PezCheckCondition(A, B) if (!(A)) { throw std::runtime_error(B); }


namespace renderlib {
  
  class Mesh;
  class Shader;
  class TextureProxy;
  
  class FluidSim : public IRenderable
  {
  public:
    
    typedef struct Surface_ {
      GLuint FboHandle;
      GLuint TextureHandle;
      int NumComponents;
    } Surface;
    
    typedef struct Slab_ {
      Surface Ping;
      Surface Pong;
    } Slab;
    
    typedef struct Vector2i_ {
      int X;
      int Y;

    Vector2i_():X(0),Y(0){};

	  Vector2i_(int x, int y) :X(x), Y(y){};
    } Vector2i;
    
    struct ProgramsRec {
      GLuint Advect;
      GLuint Jacobi;
      GLuint SubtractGradient;
      GLuint ComputeDivergence;
      GLuint ApplyImpulse;
      GLuint ApplyBuoyancy;
    } Programs;
    
    
  public:
    FluidSim();
    virtual ~FluidSim();
    
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
    void reset(){};

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    FluidSim(const FluidSim&) = delete;
    FluidSim & operator=(const FluidSim&) = delete;
    
    
    
    float AmbientTemperature;
    float ImpulseTemperature;
    float ImpulseDensity;
    int NumJacobiIterations;
    float TimeStep;
    float SmokeBuoyancy;
    float SmokeWeight;
    float GradientScale;
    float TemperatureDissipation;
    float VelocityDissipation;
    float DensityDissipation;
    Vector2i ImpulsePosition;
    
    const int PositionSlot = 0;
    
    GLuint CreateQuad();
    void ResetState();
    GLuint CreateProgram(const char* vsKey, const char* gsKey, const char* fsKey);
    Surface CreateSurface(GLsizei width, GLsizei height, int numComponents);
    Slab CreateSlab(GLsizei width, GLsizei height, int numComponents);
    void CreateObstacles(Surface dest, int width, int height);
    void InitSlabOps();
    void SwapSurfaces(Slab* slab);
    void ClearSurface(Surface s, float value);
    void Advect(Surface velocity, Surface source, Surface obstacles, Surface dest, float dissipation);
    void Jacobi(Surface pressure, Surface divergence, Surface obstacles, Surface dest);
    void SubtractGradient(Surface velocity, Surface pressure, Surface obstacles, Surface dest);
    void ComputeDivergence(Surface velocity, Surface obstacles, Surface dest);
    void ApplyImpulse(Surface dest, Vector2i position, float value);
    void ApplyBuoyancy(Surface velocity, Surface temperature, Surface density, Surface dest);
    
  protected:
    int _renderSortValue;
    uint32_t _id;
    PointerInfo _pointer;
    
    std::vector<Shader*> _shaders;
    Shader* _advectShader;
    Shader* _bouyancyShader;
    Shader* _computeDivergenceShader;
    Shader* _fillShader;
    Shader* _jacobiShader;
    Shader* _splatShader;
    Shader* _subtractGradientShader;
    Shader* _visualizeShader;
    
    GLuint QuadVao;
    GLuint VisualizeProgram;
    Slab Velocity, Density, Pressure, Temperature;
    Surface Divergence, Obstacles, HiresObstacles;
   

    float CellSize;// (1.25f)
    int ViewportWidth;// (512)
    int ViewportHeight;// (512)
    int GridWidth;// (ViewportWidth / 2)
    int GridHeight;// (ViewportHeight / 2)
    float SplatRadius;// ((float)GridWidth / 8.0f)
  };

} // namespace renderlib
#endif /* defined(__Splash__FluidSim__) */
