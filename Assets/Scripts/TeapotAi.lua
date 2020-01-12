-- TeapotAi.lua : Defines the state machine for AI teapots

require("Scripts\\TeapotStates.lua");
require("Scripts\\TeapotBrains.lua");

-----------------------------------------------------------------------------------------------------------------------
-- State machine								- Chapter 18, page 616
-----------------------------------------------------------------------------------------------------------------------
TeapotStateMachine = class(nil,
{
	_teapot = nil,
	_currentState = nil,
	_brain = nil,
});

function TeapotStateMachine:Destroy()
	self._currentState = nil;
	self._brain = nil;
end

function TeapotStateMachine:SetState(newState)
	if (self._currentState == nil or not self._currentState:IsInstance(newState)) then
		self:_InternalSetState(newState);
	end
end

function TeapotStateMachine:ChooseBestState()
	if (self._brain) then
		local newState = self._brain:Think();
		
		if (newState ~= nil) then
			print("Teapot chose " .. newState.name);
		else
			print("Teapot chose nil");
		end
		
		self:SetState(newState);
	end
end

function TeapotStateMachine:Update(deltaMs)
	if (self._currentState) then
		self._currentState:Update(deltaMs);
	end
end

function TeapotStateMachine:_InternalSetState(newState)
	self._currentState = newState:Create({_teapot = self._teapot});
	self._currentState:Init();
end

