#pragma once

#include <string>
#include "EngineObjectManager.h"
#include "ObjectIDGenerator.h"

#define DEFAULT_TAG "NONE"
class EngineObject;

class EngineObject : public std::enable_shared_from_this<EngineObject>
{
  
public:
	EngineObject(std::string tag) : _tag(tag)
	{
		_id = ObjectIDGenerator::getInstance().getNextID();
	}

	EngineObject() 
	{
		_id = ObjectIDGenerator::getInstance().getNextID();
		_tag = DEFAULT_TAG;
	}
	//EngineObject(XMLData& storageData);

	virtual ~EngineObject()
	{
		EngineObjectManager::getInstance().deRegisterObject(*this);
	}

	void registerObject()
	{
		EngineObjectManager::getInstance().registerObject(shared_from_this());
	}

	inline const std::string& getTag() { return _tag; }
	inline void setTag(const std::string tag){ _tag = tag; }
	inline uint32_t getID() { return _id; }
	void init();

  //Need to be overidden to be useful
	virtual void sendOutMessage(EngineObjectMessage& msg);
	virtual void processMessage(EngineObjectMessage& msg){};

	EngineObject(const EngineObject& other) { _id = other._id; _tag = other._tag; }
	EngineObject & operator=(const EngineObject& other) { _id = other._id; _tag = other._tag; return *this;}

protected:
	uint32_t _id;
	std::string _tag;
  

};
