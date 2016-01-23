#include "InputManager.h"
#include "WindowManager.h"
#include "IControllable.h"
#include "GLFWTime.h"


namespace renderlib{
using namespace vmath;
  
std::vector<PointerInfo> InputManager::_pointers; 
std::map<uint32_t, std::shared_ptr<IControllable> > InputManager::_listeners;
bool InputManager::_dirty;
bool InputManager::_ctrlDown;
bool InputManager::_shiftDown;
bool InputManager::_altDown;
unsigned long InputManager::_frame;
  

void InputManager::init()
{
  for(int i = 0; i < 10; i++)
  {
    PointerInfo pi;
    pi.id = i;
    InputManager::_pointers.push_back(pi);
  }
  InputManager::_dirty = false;
  InputManager::_ctrlDown = false;
  InputManager::_shiftDown = false;
  InputManager::_altDown = false;
  InputManager::_frame = 0;
}

void InputManager::updatePointer(vec2 pos, bool down, float dragging,
                          float pressure, float id)
{
  if(id < 0 || id >= 10) return;
  PointerInfo& lastPI = InputManager::_pointers[(int)id];
  PointerInfo pi;



  pi.pos = pos;
  pi.lastPos = lastPI.pos;
    
  pi.down = down;
  pi.dragging = dragging;
  pi.pressure = pressure;
  pi.id = (int)id;
  pi.dt = GLFWTime::getDT();
  pi.lastUpdateTime = GLFWTime::getCurrentTime();
  pi.velocity = (pi.pos - pi.lastPos) * pi.dt;

  InputManager::_pointers[pi.id] = pi;
  InputManager::_dirty = true;
}

void InputManager::updatePointer(PointerInfo& pi)
{
  if(pi.id < 0 || pi.id >= 10) return;
  InputManager::_pointers[pi.id] = pi;
  InputManager::_dirty = true;

}
  
void InputManager::clearPointers()
{
	for (int i = 0; i < (int)_pointers.size(); i++)
  {
    InputManager::_pointers[i].pressure = 0.0f;
    InputManager::_pointers[i].down = false;
    InputManager::_pointers[i].peakHit = false;
  }
}
  
void InputManager::handleKey(KeyInfo& key)
{
  for(std::pair<uint32_t,std::shared_ptr<IControllable> > c : _listeners)
  {
    InputManager::_shiftDown = (key.mod & KM_MOD_SHIFT) != 0;
    InputManager::_ctrlDown = (key.mod & KM_MOD_CONTROL) != 0;
    InputManager::_altDown = (key.mod & KM_MOD_ALT) != 0;
    c.second->handleKey(key);
  }
}

  void InputManager::addListener(std::shared_ptr<IControllable> c)
{
  InputManager::_listeners[c->getID()] = c;
}

  void InputManager::removeListener(std::shared_ptr<IControllable> c)
{
  InputManager::_listeners.erase(c->getID());
}

void InputManager::clearListeners()
{
  InputManager::_listeners.erase(InputManager::_listeners.begin(),InputManager::_listeners.end());
}

void InputManager::broadcastInput()
{
  if(InputManager::_dirty == false) return;

  for(std::pair<uint32_t,std::shared_ptr<IControllable> > c : InputManager::_listeners)
  {
    c.second->handlePointer(InputManager::_pointers);
  }
  InputManager::_dirty = false;
}

}//namespace renderlib

