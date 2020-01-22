#pragma once

//RHI Render Hardware Interface, Inspired By UE4.

class RHI {
public:
	RHI();

	virtual bool InitRHI()=0;
	virtual bool ReleaseRHI()=0;
	virtual bool InitRHIResource()=0;
	virtual bool ReleaseRHIResource()=0;


};