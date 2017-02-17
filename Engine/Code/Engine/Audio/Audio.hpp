#pragma once

//---------------------------------------------------------------------------
#pragma comment( lib, "ThirdParty/fmod/fmodex_vc" ) // Link in the fmodex_vc.lib static library


//---------------------------------------------------------------------------
#include "ThirdParty/fmod/fmod.hpp"
#include <string>
#include <vector>
#include <map>

//---------------------------------------------------------------------------
typedef unsigned int SoundID;
typedef void* AudioChannelHandle;
const unsigned int MISSING_SOUND_ID = 0xffffffff;


/////////////////////////////////////////////////////////////////////////////
class AudioSystem
{
public:
	AudioSystem();
	virtual ~AudioSystem();

	SoundID createOrGetSound( const std::string& soundFileName );
	AudioChannelHandle playSound( SoundID soundID, float volumeLevel=1.f, bool isLooping=false );
	void stopChannel( AudioChannelHandle channel );
	void channelPausePlay( AudioChannelHandle channel );
	void update();
	void setVolume(AudioChannelHandle channel,float newVolume);

protected:
	void initializeFMOD();
	void shutDownFMOD();
	void validateResult( FMOD_RESULT result );

protected:
	FMOD::System*						m_fmodSystem;
	std::map< std::string, SoundID >	m_registeredSoundIDs;
	std::vector< FMOD::Sound* >			m_registeredSounds;
};

