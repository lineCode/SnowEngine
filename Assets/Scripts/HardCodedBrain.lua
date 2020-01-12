--========================================================================
-- HardCodedBrain.lua : Implements a brain using hard-coded logic

require("Scripts\\TeapotBrains.lua");
require("Scripts\\TeapotStates.lua");

-----------------------------------------------------------------------------------------------------------------------
-- HardCodedBrain 											- Chapter 18, page 620
-- This is the hard-coded brain.  It just takes what it needs from the gamestate and does some simple hard-coded logic
-- to find the appropriate state.
-----------------------------------------------------------------------------------------------------------------------
HardCodedBrain = class(TeapotBrain,
{
	--
});

function HardCodedBrain:Think()
	local playerPos = Vec3:Create(g_actorMgr:GetPlayer():GetPos());
	local pos = Vec3:Create(self._teapot:GetPos());
	local diff = playerPos - pos;
	
	-- player close
	if (diff:Length() < 20) then
		-- hit points low, run
		if (self._teapot.hitPoints <= 1) then
			return RunAwayState;
		-- hit points not low, attack
		else
			return AttackState;
		end
		
	-- player not close, resume patrol
	else
		return PatrolState;
	end
	
end

