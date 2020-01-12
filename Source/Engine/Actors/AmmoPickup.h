#pragma once
// AmmoPickup.h - An ammo pickup

#include "PickupInterface.h"

//---------------------------------------------------------------------------------------------------------------------
// AmmoPickup implementation class.
// Chapter 6, page 170
//  This component isn't actually used in Teapot wars.  It was used as an example for describing components in 
// the book.  I decided to include it here to give you a starting point to add ammo drops in your game.  :)
//---------------------------------------------------------------------------------------------------------------------
class AmmoPickup : public PickupInterface
{
public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual TiXmlElement* VGenerateXml(void) override;
    virtual void VApply(WeakActorPtr pActor) override;
};
