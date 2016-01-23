//
//  FPSController
//
//  Created by Kristofer Schlachter on 5/26/15.
//
//
#pragma once

#include <iostream>
#include <stdint.h>//uint32_t
#include "allmath.h"
#include "InputManager.h"
#include "IControllable.h"

namespace renderlib {


  class FPSController : public IControllable
  {
  public:
    FPSController();
    virtual ~FPSController();

    virtual void init();
    uint32_t  getID() {return _objectID;};

    virtual void update(float time);
    virtual void fixedUpdate();

    virtual void handleKey(KeyInfo& key);
    virtual void handlePointer(std::vector<PointerInfo>& pointers);

    virtual vec3 getVelocity(){ return _velocity; }
    virtual vec2 getAngles(){ return _angles; }
    virtual quat getRotation(){ return _rotation; }
    
    virtual void setRotationSpeed(vec2 r) { _rotationSpeed = r; }

    //C++ 11 way of hiding these methods
    //no copy constructor or copy assignment operato
    FPSController(const FPSController&) = delete;
    FPSController & operator=(const FPSController&) = delete;

  protected:
    vec3 _velocity;
    vec2 _angles;
    quat _rotation;
    uint32_t _objectID;
    float _lastTime{ 0.0f };
    vec2 _rotationSpeed{10.5};

  };

} // namespace renderlib
