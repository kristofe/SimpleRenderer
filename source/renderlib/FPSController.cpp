#include "FPSController.h"
#include "InputManager.h"
#include "ObjectIDGenerator.h"

namespace renderlib{
  FPSController::FPSController()
  {
    _objectID = ObjectIDGenerator::getInstance().getNextID();

    _velocity = vec3(0.0f);
    _rotation = glm::angleAxis(0.0f, vec3(0));
  }

  FPSController::~FPSController()
  {

  }

  /*
  void FPSController::registerWithInputManager(IControllable* c)
  {
    std::shared_ptr<IControllable> p(c);
    InputManager::addListener(p);
  }
  */

  void FPSController::init()
  {
    _rotation = glm::quat();
    _angles = vec2();
  }

  void FPSController::update(float time)
  {


  }

  void FPSController::fixedUpdate()
  {

  }

  void FPSController::handleKey(KeyInfo& key)
  {
    //printf("key:%d\n", (int)key.key);

    if (key.key >= '0' && key.key <= '9' && key.action == KeyInfo::KeyAction::RELEASE)
    {
      char idx = key.key - '0';
    }
    //264 == down arrow
    if ((key.key == 's' || key.key == 'S' || key.key == 264) && (key.action == KeyInfo::KeyAction::REPEAT
      || key.action == KeyInfo::KeyAction::PRESS))
    {
      _velocity.z = -1.0f;

    }
    if ((key.key == 's' || key.key == 'S' || key.key == 264) && (key.action == KeyInfo::KeyAction::RELEASE))
    {
      _velocity.z = 0.0f;

    }

    //264 == up arrow
    if ((key.key == 'w' || key.key == 'W' || key.key == 265) && (key.action == KeyInfo::KeyAction::REPEAT
      || key.action == KeyInfo::KeyAction::PRESS))
    {
      _velocity.z = 1.0f;
    }
    if ((key.key == 'w' || key.key == 'W' || key.key == 265) && (key.action == KeyInfo::KeyAction::RELEASE))
    {
      _velocity.z = 0.0f;
    }

    //263 == left arrow?
    if ((key.key == 'a' || key.key == 'A' || key.key == 263) && (key.action == KeyInfo::KeyAction::REPEAT
      || key.action == KeyInfo::KeyAction::PRESS))
    {
      _velocity.x = 1.0f;

    }
    if ((key.key == 'a' || key.key == 'A' || key.key == 263) && (key.action == KeyInfo::KeyAction::RELEASE))
    {
      _velocity.x = 0.0f;

    }

    //262 == right arrow?
    if ((key.key == 'd' || key.key == 'D' || key.key == 262) && (key.action == KeyInfo::KeyAction::REPEAT
      || key.action == KeyInfo::KeyAction::PRESS))
    {
      _velocity.x = -1.0f;

    }
    if ((key.key == 'd' || key.key == 'D' || key.key == 262) && (key.action == KeyInfo::KeyAction::RELEASE))
    {
      _velocity.x = 0.0f;

    }
  }

  void FPSController::handlePointer(std::vector<PointerInfo>& pointers)
  {
    PointerInfo& pi = pointers[0];
    if (pi.dragging){
      vec2 v = pi.velocity;
      _angles += v;

      vec2 a = _angles * _rotationSpeed;
      quat x_rot = glm::quat(vec3(a.y * DEG2RAD, 0.0f, 0.0f));
      quat y_rot = glm::quat(vec3(0.0f,a.x * DEG2RAD, 0.0f));
      
      
      _rotation = x_rot * y_rot;
      
    }
  }

}//namespace renderlib