#pragma once
// Audio.cpp : Defines a simple sound system.

class SoundResourceExtraData;

//////////////////////////////////////////////////////////////////////
// SoundType Description
//
// This is an enum that represents the different kinds of sound data
// streams the sound system can handle.
//
//////////////////////////////////////////////////////////////////////

enum SoundType
{
	SOUND_TYPE_FIRST,
	SOUND_TYPE_MP3 = SOUND_TYPE_FIRST,
	SOUND_TYPE_WAVE,
	SOUND_TYPE_MIDI,
	SOUND_TYPE_OGG,

	// This needs to be the last sound type
	SOUND_TYPE_COUNT,
	SOUND_TYPE_UNKNOWN,
};

extern char *gSoundExtentions[];

//////////////////////////////////////////////////////////////////////
// class IAudioBuffer							- 
//
// The interface class that defines the public API for audio buffers.
// An audio buffer maps to one instance of a sound being played, 
// which ISNT the sound data. Two different sounds can be played from
// the same source data - such as two explosions in two different places.
//////////////////////////////////////////////////////////////////////

class IAudioBuffer
{
public:
	virtual ~IAudioBuffer() { }

	virtual void *VGet()=0;
	virtual shared_ptr<ResHandle> VGetResource()=0;
	virtual bool VOnRestore()=0;

	virtual bool VPlay(int volume, bool looping)=0;
	virtual bool VPause()=0;
	virtual bool VStop()=0;
	virtual bool VResume()=0;

	virtual bool VTogglePause()=0;
	virtual bool VIsPlaying()=0;
	virtual bool VIsLooping() const=0;
	virtual void VSetVolume(int volume)=0;
    virtual void VSetPosition(unsigned long newPosition)=0;
	virtual int VGetVolume() const=0;
	virtual float VGetProgress()=0;
};

//////////////////////////////////////////////////////////////////////
// class AudioBuffer							-  
//
// Implements IAudiobuffer interface using a smart pointer to SoundResource.
//
//////////////////////////////////////////////////////////////////////

class AudioBuffer : public IAudioBuffer
{
public: 
	virtual shared_ptr<ResHandle> VGetResource() { return m_Resource; }
	virtual bool VIsLooping() const { return m_isLooping; }
	virtual int VGetVolume() const { return m_Volume; }
protected:
	AudioBuffer(shared_ptr<ResHandle >resource) 
		{ m_Resource = resource; 
		  m_isPaused = false;
		  m_isLooping = false;
		  m_Volume = 0;
		}	// disable public construction

	shared_ptr<ResHandle> m_Resource;

	// Is the sound paused
	bool m_isPaused;

	// Is the sound looping
	bool m_isLooping;

	//the volume
	int m_Volume;
};


//////////////////////////////////////////////////////////////////////
// class IAudio									- 
//
// This interface class describes the public interface for 
// a game's audio system. 
//////////////////////////////////////////////////////////////////////

class IAudio
{
public:
	virtual bool VActive()=0;

	virtual IAudioBuffer *VInitAudioBuffer(shared_ptr<ResHandle> handle)=0;
	virtual void VReleaseAudioBuffer(IAudioBuffer* audioBuffer)=0;

	virtual void VStopAllSounds()=0;
	virtual void VPauseAllSounds()=0;
	virtual void VResumeAllSounds()=0;

	virtual bool VInitialize(HWND hWnd)=0;
	virtual void VShutdown()=0;
};

//////////////////////////////////////////////////////////////////////
// class Audio									- 
//
// Implements IAudio interface - but not all the way - this is 
// still a base class. See class DirectSoundAudio.
// 
//////////////////////////////////////////////////////////////////////

class Audio : public IAudio
{
public:
	Audio();
	~Audio() { VShutdown(); }
	virtual void VStopAllSounds();
	virtual void VPauseAllSounds();
	virtual void VResumeAllSounds();

	virtual void VShutdown();
	static bool HasSoundCard(void);
	bool IsPaused() { return m_AllPaused; }

protected:

	typedef std::list<IAudioBuffer *> AudioBufferList;

	AudioBufferList m_AllSamples;	// List of all currently allocated audio buffers
	bool m_AllPaused;				// Has the sound system been paused?
	bool m_Initialized;				// Has the sound system been initialized?
};

extern Audio *g_pAudio;


