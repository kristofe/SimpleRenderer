#ifndef __Splash__ObjectIDGenerator__
#define __Splash__ObjectIDGenerator__

#include <stdint.h>


class ObjectIDGenerator{
public:
	ObjectIDGenerator();

  uint32_t getNextID();

  static ObjectIDGenerator& getInstance()
  {
	  if (instance == nullptr)
		  instance = new ObjectIDGenerator();
	  return *instance;
  }

  //C++ 11 way of hiding these methods
  //Default constructor, no copy constructor or copy assignment operato
  ObjectIDGenerator(const ObjectIDGenerator&) = delete;
  ObjectIDGenerator & operator=(const ObjectIDGenerator&) = delete;

private:
  uint32_t nextID;
  static ObjectIDGenerator* instance;
  
};



#endif /* defined(__Splash__ObjectIDGenerator__) */
