#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <stdint.h>

//prototype of object
class EngineObject;

class EngineObjectMessage
{
public:
	uint32_t  sender;
	uint32_t recipient;
	bool broadcast;
	std::string messageType;
	void* data;

	EngineObjectMessage()
	{
		sender = 0;
		recipient = 0;
		broadcast = false;
		messageType = "";
	}

	EngineObjectMessage(std::string tag,std::string type, void* d = nullptr)
	{
		sender = 0;
		setRecipientFromTag(tag);
		broadcast = false;
		messageType = type;
		data = d;
	}

	EngineObjectMessage(int recip, std::string type, void* d = nullptr)
	{
		sender = 0;
		recipient = recip;
		broadcast = false;
		messageType = type;
		data = d;
	}

	void set(std::string tag, std::string type, void* d = nullptr)
	{
		sender = 0;
		setRecipientFromTag(tag);
		broadcast = false;
		messageType = type;
		data = d;
	}

	void set(int recip, std::string type, void* d = nullptr)
	{
		sender = 0;
		recipient = recip;
		broadcast = false;
		messageType = type;
		data = d;
	}
	~EngineObjectMessage()
	{
		//data.reset();
	}

	EngineObjectMessage& operator=(EngineObjectMessage& other){ 
		clone(other);
    return *this;
	}

	EngineObjectMessage(EngineObjectMessage& other){
		clone(other);
	};

	void clone(EngineObjectMessage& other)
	{
		sender = other.sender;
		recipient = other.recipient;
		broadcast = other.broadcast;
		messageType = other.messageType;
		data = other.data;
	}
   

	/*
	void setDataHeap(void* val)
	{
		data = std::shared_ptr<void>(val);
	}

	void setDataStack(void* val)
	{
		data = std::shared_ptr<void>(val, [](void* x){});
	}
  */

	void setRecipientFromTag(std::string tag);


	void* getData()
	{
		return data;
		/*
		auto ptr = data.lock();
		if (ptr)
		{
			return ptr.get();
		}
		return nullptr;
    */
	}


};

class EngineObjectManager
{
public:
	static EngineObjectManager& getInstance()
	{
		if (instance == nullptr)
			instance = new EngineObjectManager();
		return *instance;
	}
  
	EngineObjectManager(){};

	void registerObject(std::shared_ptr<EngineObject> eo);
	void deRegisterObject(EngineObject& obj);

	void sendOutMessage(EngineObjectMessage& msg, std::string recipient);
	void sendOutMessage(EngineObjectMessage& msg, uint32_t recipient);
	void broadcastMessage(EngineObjectMessage& msg);
	void processMessage(EngineObjectMessage& msg);
   
	uint32_t getIDFromTag(std::string tag);
	std::weak_ptr<EngineObject> getObjectFromTag(std::string tag);
	std::weak_ptr<EngineObject> getObjectFromID(uint32_t id);

	EngineObjectManager& operator=(EngineObjectManager& other) = delete;
	EngineObjectManager(EngineObjectManager& other) = delete;
protected:
	static EngineObjectManager* instance;
	std::unordered_multimap<std::string, uint32_t> messageListenersDB;
	std::unordered_map<uint32_t, std::shared_ptr<EngineObject>> engineObjectDB;
	std::unordered_map<std::string, uint32_t> tagDB;


   
};