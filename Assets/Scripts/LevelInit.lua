--========================================================================
-- LevelInit.lua : A script that's executed when the level is initializing

require("scripts\\ActorManager.lua");

-----------------------------------------------------------------------------------------------------------------------
-- Global actor manager										- Chapter 21, page 738
-----------------------------------------------------------------------------------------------------------------------
g_actorMgr = ActorManager:Create();


-----------------------------------------------------------------------------------------------------------------------
-- constructor / destructor functions for actors
-----------------------------------------------------------------------------------------------------------------------
function AddPlayer(scriptObject)
	g_actorMgr:AddPlayer(scriptObject);
end

function RemovePlayer(scriptObject)
	g_actorMgr:RemovePlayer(scriptObject);
end

function AddEnemy(scriptObject)
	g_actorMgr:AddEnemy(scriptObject);
end

function RemoveEnemy(scriptObject)
	g_actorMgr:RemoveEnemy(scriptObject);
end

function AddSphere(scriptObject)
	g_actorMgr:AddSphere(scriptObject);
end

function RemoveSphere(scriptObject)
	g_actorMgr:RemoveSphere(scriptObject);
end


-----------------------------------------------------------------------------------------------------------------------
-- Debug stuff
-----------------------------------------------------------------------------------------------------------------------
function ShowPlayerOrientation()
	print("Player Orientation: " .. g_actorMgr:GetPlayer():GetYOrientationRadians());
end

function SetPlayerOrientation(radians)
	g_actorMgr:GetPlayer():RotateY(radians);
end

