#pragma once
#ifndef MATERIALMESSAGE_H
#define MATERIALMESSAGE_H
#include "Message.h"
#include <Graphics\Material.h>
class MaterialMessage : public Message
{
public:
	MaterialMessage(Material newMat) : Message(MsgType::MATERIAL)
	{
		material = newMat;
	}
	Material material;
};

#endif // !MATERIALMESSAGE_H
