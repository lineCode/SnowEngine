#pragma once
// File: TeapotController.h

#include "../Engine/Engine/Interfaces.h"

class SceneNode;

class TeapotController : public IPointerHandler, public IKeyboardHandler
{
protected:
	BYTE					m_bKey[256];			// Which keys are up and down
	shared_ptr<SceneNode>	m_object;

public:
	TeapotController(shared_ptr<SceneNode> object);
//	void SetObject(shared_ptr<SceneNode> newObject);

	void OnUpdate(DWORD const elapsedMs);

public:
	virtual bool VOnPointerMove(const Point &mousePos, const int radius) { return true; }
	virtual bool VOnPointerButtonDown(const Point &mousePos, const int radius, const std::string &buttonName);
	virtual bool VOnPointerButtonUp(const Point &mousePos, const int radius, const std::string &buttonName) { return (buttonName == "PointerLeft"); }

    bool VOnKeyDown(const BYTE c);
    bool VOnKeyUp(const BYTE c);
};

