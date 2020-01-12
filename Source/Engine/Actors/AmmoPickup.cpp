#pragma once
// AmmoPickup.cpp - An ammo pickup

#include "EngineStd.h"
#include "AmmoPickup.h"
#include "../Utilities/String.h"

const char* AmmoPickup::g_Name = "AmmoPickup";

bool AmmoPickup::VInit(TiXmlElement* pData)
{
    return true;
}

TiXmlElement* AmmoPickup::VGenerateXml(void)
{
    TiXmlElement* pComponentElement = GCC_NEW TiXmlElement(VGetName());
    return pComponentElement;
}

void AmmoPickup::VApply(WeakActorPtr pActor)
{
    StrongActorPtr pStrongActor = MakeStrongPtr(pActor);
    if (pStrongActor)
    {
        GCC_LOG("Actor", "Applying ammo pickup to actor id " + ToStr(pStrongActor->GetId()));
    }
}

