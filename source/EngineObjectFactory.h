#pragma once

#include "EngineObject.h"

//A class usefull for instantiating objects from XML
class EngineObjectFactory
{
public:
	template<typename T>
	static std::shared_ptr<T> create();

};

template<typename T>
std::shared_ptr<T> EngineObjectFactory::create()
{
	std::shared_ptr<T> p = std::make_shared<T>();
	p->init();

	return p;
}