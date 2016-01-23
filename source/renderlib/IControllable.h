//
//  IControllable.h
//
//  Created by Kristofer Schlachter on 5/26/15.
//
//
#pragma once

#include "utils.h"
#include <stdint.h>//uint32_t
#include <vector>

namespace renderlib{

  class KeyInfo;
  struct PointerInfo;

  class IControllable 
  {
  public:

    IControllable(){};
    virtual ~IControllable()  {};

    virtual uint32_t  getID() = 0;
    virtual void init() = 0;
    virtual void update(float time) = 0;
    virtual void fixedUpdate() = 0;

    virtual void handleKey(KeyInfo& key) = 0;
    virtual void handlePointer(std::vector<PointerInfo>& pointers) = 0;
    //  virtual void sendMessage(Message& msg) = 0;
    //  virtual void handleMessage(Message& msg) = 0;

  };

}
