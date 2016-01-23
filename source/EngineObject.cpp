#include "EngineObject.h"

/*
EngineObject::EngineObject(XMLData& storageData)
{
	std::string s = storageData.GetProperty("Tag");
	setTag(s);

}
*/

void EngineObject::init()
{
	registerObject();
}

void EngineObject::sendOutMessage(EngineObjectMessage& msg)
{
	msg.sender = _id;
	EngineObjectManager::getInstance().sendOutMessage(msg, msg.recipient);
}
