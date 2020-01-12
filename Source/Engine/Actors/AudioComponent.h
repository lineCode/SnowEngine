#pragma once
// AudioComponent.h - A component for attaching sounds to an actor

#include "ActorComponent.h"

//---------------------------------------------------------------------------------------------------------------------
// AudioComponent class.
//  This component was never directly described anywhere in the book but it's used to allow actors to trigger 
// sound effects.
//---------------------------------------------------------------------------------------------------------------------
class AudioComponent : public ActorComponent
{
	std::string m_audioResource;
	bool m_looping;
	float m_fadeInTime;
	int m_volume;

public:
	static const char *g_Name;
	virtual const char *VGetName() const { return g_Name; }

	bool Start();

    AudioComponent(void);

    virtual TiXmlElement* VGenerateXml(void);

    // ActorComponent interface
    virtual bool VInit(TiXmlElement* pData) override;
    virtual void VPostInit(void) override;
};
