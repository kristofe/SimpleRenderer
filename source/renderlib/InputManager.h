#ifndef __Splash__InputManager__
#define __Splash__InputManager__
#include <stdint.h> //uint32_t
#include <iostream>
#include <string>
#include <memory>
#include <map>
#include <vector>
#include "allmath.h"

class Sensor;

namespace renderlib{

  class IControllable;

using namespace vmath;
struct PointerInfo
{
  vec2 pos{0.0f, 0.0f};
  vec2 lastPos{0.0f, 0.0f};
  bool   down;
  bool   peakHit;
  bool   hasPeaked;
  float   dragging;
  float   pressure;
  float   maxPressure;
  vec2   majorAxis;
  vec2   minorAxis;
  vec2   velocity{ 0.0f, 0.0f };
  float     width;
  int     id;
  float startDownTime;
  float lastUpdateTime;
  float dt;
  
  PointerInfo()
  {
    pos = vec2(0,0);
    lastPos = vec2(0,0);
    velocity = vec2(0,0);
    down = false;
    peakHit = false;
    hasPeaked = false;
    dragging = 0.0f;
    pressure = 1.0f;
    maxPressure = 1.0f;
    id = 0;
    width = 0;
    startDownTime = 0;
    lastUpdateTime = 0;
    dt = 0;
  }

  void copyFrom(const PointerInfo& other)
  {
    this->pos = other.pos;
    this->lastPos = other.lastPos;
    this->down = other.down;
    this->peakHit = other.peakHit;
    this->hasPeaked = other.hasPeaked;
    this->dragging = other.dragging;
    this->pressure = other.pressure;
    this->maxPressure = other.maxPressure;
    this->majorAxis = other.majorAxis;
    this->minorAxis = other.minorAxis;
    this->velocity = other.velocity;
    this->width = other.width;
    this->id = other.id;
    this->startDownTime = other.startDownTime;
    this->lastUpdateTime = other.lastUpdateTime;
    this->dt = other.dt;
  }
  
  PointerInfo(const PointerInfo& other)
  {
    copyFrom(other);
  }
  
  void operator=(const PointerInfo& other)
  {
    copyFrom(other);
  };
  

};

#define KM_MOD_SHIFT  0x0001
#define KM_MOD_CONTROL 0x0002
#define KM_MOD_ALT 0x0008
class KeyInfo
{
public:
  enum KeyAction
  {
    RELEASE = 0,
    PRESS = 1,
    REPEAT = 2
  };
  
  int         key;
  KeyAction   action;
  int         mod;
  
  KeyInfo(int k, KeyAction a, int m):key(k),action(a),mod(m){};
  
  void copyFrom(const KeyInfo& other)
  {
    this->key = other.key;
    this->action = other.action;
    this->mod = other.mod;
  }
  
  KeyInfo(const KeyInfo& other)
  {
    copyFrom(other);
  }
  
  void operator=(const KeyInfo& other)
  {
    copyFrom(other);
  };
  

}; 

  
  
class InputManager{
public:
  static void init();
  static void updatePointer(vec2 pos, bool down, float dragging,
                            float pressure, float id);
  static void updatePointer(PointerInfo& pi);
  static void clearPointers();
  static void addListener(std::shared_ptr<IControllable> c);
  static void handleKey(KeyInfo& key);
  static void removeListener(std::shared_ptr<IControllable> c);
  static void clearListeners();
  static void broadcastInput();

  static bool getShiftDown() { return _shiftDown;}
  static bool getCTRLDown() { return _ctrlDown;}
  static bool getALTDown() { return _altDown;}

  //C++ 11 way of hiding these methods
  //Default constructor, no copy constructor or copy assignment operato
  InputManager() = default;
  InputManager(const InputManager&) = delete;
  InputManager & operator=(const InputManager&) = delete;
  
private:
  static std::vector<PointerInfo> _pointers; 
  static std::map<uint32_t, std::shared_ptr<IControllable> > _listeners;
  static bool _dirty;
  static bool _shiftDown;
  static bool _ctrlDown;
  static bool _altDown;
  static unsigned long _frame;
  
};




}//namespace renderlib

#endif /* defined(__Splash__InputManager__) */
