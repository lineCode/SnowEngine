-- TeapotBrains.lua : Base class for teapot brains


require("Scripts\\TeapotStates.lua");


-----------------------------------------------------------------------------------------------------------------------
-- TeapotBrain base class
-- This is the base class for teapot brains.  It defines the interface for all decision making that teapots can do.
-- the Think() function is called from the TeapotStateMachine.UpdateState() function whenever a decision needs to be 
-- made.  It returns a new state for the teapot.  Note how the base class throws an error.  This is the closest we can
-- get to a pure virtual function; it defines the interface and requires subclasses to define the implementation.  
-- It's not perfect, but it beats the generic "attempting to call nil value" error.
-----------------------------------------------------------------------------------------------------------------------
TeapotBrain = class(nil, 
{
	_teapot = nil,
});

function TeapotBrain:Init()
	return true;
end

function TeapotBrain:Think()
	error("Calling unimplemented base class version of TeapotBrain.Think()");
end

