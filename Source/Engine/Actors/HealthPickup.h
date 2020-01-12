#pragma once
// HealthPickup.h - A health pickup

#include "PickupInterface.h"


//---------------------------------------------------------------------------------------------------------------------
// HealthPickup implementation class.
// Chapter 6, page 170
//  This component isn't actually used in Teapot wars.  It was used as an example for describing components in 
// the book.  I decided to include it here to give you a starting point to add health drops in your game.  :)
//---------------------------------------------------------------------------------------------------------------------
class HealthPickup : public PickupInterface
{
public:
	static const char* g_Name;
	virtual const char* VGetName() const { return g_Name; }

    virtual bool VInit(TiXmlElement* pData) override;
    virtual TiXmlElement* VGenerateXml(void) override;
    virtual void VApply(WeakActorPtr pActor) override;
};
