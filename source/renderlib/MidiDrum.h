#ifndef __Splash__MidiDrum_
#define __Splash__MidiDrum_

#include <iostream>
#include "EngineObject.h"
#include "InputManager.h"
#include "vmath.hpp"
#include "IRenderable.h"
#include "Texture.h"
#include "IControllable.h"
#include "RenderTexture.h"
#include "Texture.h"
#include "midi/MidiController.h"

namespace renderlib {
  
  class Mesh;
  class Shader;
  class TextureProxy;
  namespace ui{
	  struct UIEnumVal;
  }
  
  class MidiDrum : public EngineObject, public IRenderable
  {
  public:
    MidiDrum();
    virtual ~MidiDrum();

    void connectToMidi();
    //IRenderable methods
    void init();
    void update(float time);
    void fixedUpdate(){};
    
    void preRender(){};
    void draw();
    void postRender(){};

    //IControllable methods
    virtual void handleKey(KeyInfo& key);
    virtual void handlePointer(std::vector<PointerInfo>& pointers);
    uint32_t getID() { return _id;};
    
    void  simulate();
	int  getRenderSortValue() { return 0; }
    uint32_t  getRenderObjectID() {return _id;};
    void  setRenderObjectID(uint32_t id){ _id = id;};
    void  setupDisplayShaders(){};
    void  setupSimulationShaders(){};
    void  debugDraw(){};
    void  resize();

    void switchDrums(bool useDrums);
    //EngineObject methods
    virtual void processMessage(EngineObjectMessage& msg) override;
    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    MidiDrum(const MidiDrum&) = delete;
    MidiDrum & operator=(const MidiDrum&) = delete;
    
  protected:
	  int _renderSortValue;

    PointerInfo _pointer;
  
    bool    _useDrums;

    PERCUSSIVE_PROGRAM _currInstrument;
    DRUM_CHANNEL_INSTRUMENTS _currDrum;
  
    MidiController _mc;
    std::vector<std::pair<int, std::string>> _ports;
    int _numPorts;
    ui::UIEnumVal* _outputsForUI;
    int _currPort;
  };

} // namespace renderlib
#endif /* defined(__Splash__MidiDrum__) */
