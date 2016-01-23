//
//  IRenderable.h
//  Splash
//
//  Created by Kristofer Schlachter on 6/19/14.
//
//

#ifndef Splash_IRenderable_h
#define Splash_IRenderable_h

#include "IControllable.h"
#include "utils.h"
#include <stdint.h>//uint32_t
#include <vector>

namespace renderlib{
  
class KeyInfo;
struct PointerInfo;
  
class IRenderable : public IControllable
{
public:
  const int DefaultSortValue = 100;
public:
  
  IRenderable(){};
  virtual ~IRenderable()  {};
  virtual void preRender() = 0;
  virtual void draw() = 0;
  virtual void postRender() = 0;
  virtual int  getRenderSortValue() = 0;
  virtual uint32_t  getID() = 0;
  virtual void reset() = 0;
  virtual void resize() = 0;
  
//In IControllable but replicated here for clarity
  virtual void init() = 0;
  virtual void update(float time) = 0;
  virtual void fixedUpdate() = 0;
  
//In IControllable
//  virtual void handleKey(KeyInfo& key) = 0;
//  virtual void handlePointer(std::vector<PointerInfo>& pointers) = 0;
  
};
  
}
#endif
