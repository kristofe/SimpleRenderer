#include "EngineObjectManager.h"
#include "EngineObject.h"


EngineObjectManager* EngineObjectManager::instance = nullptr;

void EngineObjectMessage::setRecipientFromTag(std::string tag)
{
  recipient = EngineObjectManager::getInstance().getIDFromTag(tag);
}

void EngineObjectManager::registerObject(std::shared_ptr<EngineObject> eo)
{
	auto it = engineObjectDB.find(eo->getID());
	if (it == engineObjectDB.end())
	{
    //Insert the object
		//std::shared_ptr<EngineObject>p(eo);
		engineObjectDB.insert({ eo->getID(), eo });
	}

	if (eo->getTag().compare(DEFAULT_TAG) != 0)
	{
    auto itTag = tagDB.find(eo->getTag());
    if (itTag == tagDB.end())
    {
      //Insert the object
      tagDB.insert({ eo->getTag(), eo->getID() });
    }
	}
}

void EngineObjectManager::deRegisterObject(EngineObject& obj)
{
  
	auto it = engineObjectDB.find(obj.getID());
	if (it != engineObjectDB.end())
	{
    //remove the object
		engineObjectDB.erase(it);
	}

	auto itTag = tagDB.find(obj.getTag());
	if (itTag != tagDB.end())
	{
		//remove the object
		tagDB.erase(itTag);
	}

}

void EngineObjectManager::sendOutMessage(EngineObjectMessage& msg, std::string recipient)
{
	uint32_t id = getIDFromTag(recipient);
	sendOutMessage(msg, id);
}

void EngineObjectManager::sendOutMessage(EngineObjectMessage& msg, uint32_t recipient)
{
	auto go = getObjectFromID(recipient);
    
	if (auto ptr = go.lock())
	{
		ptr->processMessage(msg);
	}
}

void EngineObjectManager::broadcastMessage(EngineObjectMessage& msg)
{
	for (auto pairObj : engineObjectDB)
	{
      pairObj.second->processMessage(msg);
	}
}
std::weak_ptr<EngineObject> EngineObjectManager::getObjectFromTag(std::string tag)
{
	uint32_t id = getIDFromTag(tag);
	return getObjectFromID(id);
}

std::weak_ptr<EngineObject> EngineObjectManager::getObjectFromID(uint32_t id)
{
	std::weak_ptr<EngineObject> ret;
	auto it = engineObjectDB.find(id);
	if (it != engineObjectDB.end())
	{
		ret =  it->second;
	}
	return ret;

}

uint32_t EngineObjectManager::getIDFromTag(std::string tag)
{
	auto it = tagDB.find(tag);
	if (it != tagDB.end())
	{
		return it->second;
	}
	return 0;

}

