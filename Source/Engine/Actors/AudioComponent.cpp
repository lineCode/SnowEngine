#pragma once
// AudioComponent.cpp : A component for attaching sounds to an actor

#include "EngineStd.h"

#include "..\Utilities\String.h"
#include "..\Actors\AudioComponent.h"
#include "..\Audio\SoundProcess.h"
#include "..\MainLoop\ProcessManager.h"
#include "..\UserInterface\HumanView.h"

const char* AudioComponent::g_Name = "AudioComponent";

AudioComponent::AudioComponent()
{
	m_audioResource = "";
	m_looping = false;
	m_fadeInTime = 0.0f;
	m_volume = 100;
}

bool AudioComponent::VInit(TiXmlElement* pData)
{
    TiXmlElement* pTexture = pData->FirstChildElement("Sound");
    if (pTexture)
	{
		m_audioResource = pTexture->FirstChild()->Value();
	}

	TiXmlElement* pLooping = pData->FirstChildElement("Looping");
	if (pLooping)
	{
		std::string value = pLooping->FirstChild()->Value();
		m_looping = (value == "0") ? false : true;
	}

	TiXmlElement* pFadeIn = pData->FirstChildElement("FadeIn");
	if (pFadeIn)
	{
		std::string value = pFadeIn->FirstChild()->Value();
		m_fadeInTime = (float)atof(value.c_str());
	}

	TiXmlElement* pVolume = pData->FirstChildElement("Volume");
	if (pVolume)
	{
		std::string value = pVolume->FirstChild()->Value();
		m_volume = atoi(value.c_str());
	}

	return true;
}


TiXmlElement* AudioComponent::VGenerateXml(void)
{
    TiXmlElement* pBaseElement = GCC_NEW TiXmlElement(VGetName());

	TiXmlElement* pSoundNode = GCC_NEW TiXmlElement("Sound");
    TiXmlText* pSoundText = GCC_NEW TiXmlText(m_audioResource.c_str());
    pSoundNode->LinkEndChild(pSoundText);
    pBaseElement->LinkEndChild(pSoundNode);

    TiXmlElement* pLoopingNode = GCC_NEW TiXmlElement("Looping");
    TiXmlText* pLoopingText = GCC_NEW TiXmlText(m_looping ? "1" : "0" );
    pLoopingNode->LinkEndChild(pLoopingText);
    pBaseElement->LinkEndChild(pLoopingNode);

    TiXmlElement* pFadeInNode = GCC_NEW TiXmlElement("FadeIn");
    TiXmlText* pFadeInText = GCC_NEW TiXmlText( ToStr(m_fadeInTime).c_str());
    pFadeInNode->LinkEndChild(pFadeInText);
    pBaseElement->LinkEndChild(pFadeInNode);

    TiXmlElement* pVolumeNode = GCC_NEW TiXmlElement("Volume");
    TiXmlText* pVolumeText = GCC_NEW TiXmlText( ToStr(m_volume).c_str());
    pVolumeNode->LinkEndChild(pVolumeText);
    pBaseElement->LinkEndChild(pVolumeNode);

	return pBaseElement;
}

void AudioComponent::VPostInit()
{
	HumanView *humanView = g_pApp->GetHumanView();
	if (!humanView)
	{
		GCC_ERROR("Sounds need a human view to be heard!");
		return;
	}

	ProcessManager *processManager = humanView->GetProcessManager();
	if (!processManager)
	{
		GCC_ERROR("Sounds need a process manager to attach!");
		return;
	}

	if (!g_pApp->IsEditorRunning())
	{
		// The editor can play sounds, but it shouldn't run them when AudioComponents are initialized.

		Resource resource(m_audioResource);
		shared_ptr<ResHandle> rh = g_pApp->m_ResCache->GetHandle(&resource);
		shared_ptr<SoundProcess> sound(GCC_NEW SoundProcess(rh, 0, true));
		processManager->AttachProcess(sound);

		// fade process
		if (m_fadeInTime > 0.0f)
		{
			shared_ptr<FadeProcess> fadeProc(new FadeProcess(sound, 10000, 100)); 
			processManager->AttachProcess(fadeProc);
		}
	}
}
