#include "ObjectIDGenerator.h"

  
ObjectIDGenerator* ObjectIDGenerator::instance = nullptr;

uint32_t ObjectIDGenerator::getNextID()
{
  return nextID++;
}

ObjectIDGenerator::ObjectIDGenerator()
{
  nextID = 1;
}
