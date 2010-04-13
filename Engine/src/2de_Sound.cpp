#include "2de_Sound.h"

#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

//////////////////////////////////////////////////////////////////////////
// CSound

CSound::CSound()
{
	filename = "";
	Data = NULL;
	SetName("CSound");
	CSoundManager::Instance()->AddObject(this);
}

CSound::CSound(const char *AFileName)
{
	filename = AFileName;
	Data = NULL;
	SetName("CSound");
	CSoundManager::Instance()->AddObject(this);
}

CSound::~CSound()
{
	if (loaded && Data != NULL)
	{
		// as written in SDL_mixer doc, it's a bad idea to free chunk being played, so maybe we should stop all sounds.. but i'm not sure...
		// Mix_HaltChannel(-1);

		Mix_FreeChunk(Data);
		Data = NULL;
	}
}

bool CSound::LoadFromFile()
{
	if (filename == "")
		return false;
	
	Data = Mix_LoadWAV(filename.c_str());
	if (Data == NULL)
	{
		Log.Log("ERROR", "Can't load sound '%s': %s", filename.c_str(), Mix_GetError());
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// CMusic

CMusic::CMusic()
{
	filename = "";
	Data = NULL;
	SetName("CMusic");
	CMusicManager::Instance()->AddObject(this);
}

CMusic::CMusic(const char *AFileName)
{
	filename = AFileName;
	Data = NULL;
	SetName("CMusic");
	CMusicManager::Instance()->AddObject(this);
}

CMusic::~CMusic()
{
	if (loaded && Data != NULL)
	{
		// Mix_HaltMusic();

		Mix_FreeMusic(Data);
		Data = NULL;
	}
}

bool CMusic::LoadFromFile()
{
	if (filename == "")
		return false;
	
	Data = Mix_LoadMUS(filename.c_str());
	if (Data == NULL)
	{
		Log.Log("ERROR", "Can't load music '%s': %s", filename.c_str(), Mix_GetError());
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// CSoundManager

CSound* CSoundManager::GetSoundByName(const string &SoundName)
{
	CSound *Result = NULL;
	Result = dynamic_cast<CSound*>(GetObject(&SoundName));
	if (Result)
		Result->CheckLoad();
	return Result;
}

CSoundManager::CSoundManager()
{
	SetName("Sound manager");
}

//////////////////////////////////////////////////////////////////////////
// CMusicManager

CMusic* CMusicManager::GetMusicByName(const string &MusicName)
{
	CMusic *Result = NULL;
	Result = dynamic_cast<CMusic*>(GetObject(&MusicName));
	if (Result)
		Result->CheckLoad();
	return Result;
}

CMusicManager::CMusicManager()
{
	SetName("Music manager");
}


//////////////////////////////////////////////////////////////////////////
// CSoundMixer

bool CSoundMixer::PlaySound(CSound *Sound, int Time /*= -1*/)
{
	if (Sound == NULL || !Initialized)
		return false;

	Mix_Chunk *SoundChunk = Sound->GetData();

	if (!Sound->CheckLoad() || SoundChunk == NULL)
		return false;

	if (Mix_PlayChannelTimed(-1, SoundChunk, 0, Time) == -1)
	{
		Log.Log("ERROR", "Can't play sound '%s': %s", Sound->GetName(), Mix_GetError());
		return false;
	}

	return true;
}

void CSoundMixer::StopAllSound()
{
	if (!Initialized)
		return;

	Mix_HaltChannel(-1);
}

bool CSoundMixer::PlayMusic(CMusic *Music, int FadeInTime /*= 0*/)
{
	if (Music == NULL || !Initialized)
		return false;
	
	Mix_Music *MusicData = Music->GetData();

	if (!Music->CheckLoad() || MusicData == NULL)
		return false;

	int result;

	if (FadeInTime == 0)
	{
		result = Mix_PlayMusic(MusicData, 1);
	}
	else
	{
		result = Mix_FadeInMusic(MusicData, 1, FadeInTime);
	}

	if (result == -1)
	{
		Log.Log("ERROR", "Can't play music '%s': %s", Music->GetName(), Mix_GetError());
		return false;
	}

	return true;
}

void CSoundMixer::StopMusic(int FadeOutTime /*= 0*/)
{
	if (!Initialized)
		return;

	if (FadeOutTime == 0)
		Mix_HaltMusic();
	else
		Mix_FadeOutMusic(FadeOutTime);
}

void CSoundMixer::SetMusicVolume(int AVolume)
{
	if (AVolume < 0 || AVolume > MIX_MAX_VOLUME) {
		Log.Log("WARNING", "Volume should be between 0 and MIX_MAX_VOLUME (%d)", MIX_MAX_VOLUME);
	}

	Mix_VolumeMusic(AVolume);
}

int CSoundMixer::GetMusicVolume() const
{
	return Mix_VolumeMusic(-1);
}

void CSoundMixer::SetSoundVolume(CSound *ASound, int AVolume)
{
	if (AVolume < 0 || AVolume > MIX_MAX_VOLUME) {
		Log.Log("WARNING", "Volume should be between 0 and MIX_MAX_VOLUME (%d)", MIX_MAX_VOLUME);
	}

	Mix_VolumeChunk(ASound->GetData(), AVolume);
}

int CSoundMixer::GetSoundVolume(CSound *ASound) const
{
	return Mix_VolumeChunk(ASound->GetData(), -1);
}

CSoundMixer::CSoundMixer()
{
	SetName("Sound mixer");
	Initialized = false;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		// well, absense of sound is not so critical, so we can continue.. but:
		// 	1. we should notice user in some way (may be log is sufficent)
		// 	2. all methods of this class should check Initialized value before attempting to use sound device
		//
		Log.Log("ERROR", "Can't initialize audio subsystem: %s", SDL_GetError());
		return;
	}

	// TODO: replace by values from config
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0)
	{
		Log.Log("ERROR", "Can't open audio device: %s", Mix_GetError());
		return;
	}
	
	Mix_AllocateChannels(SOUND_MIXING_CHANNELS_COUNT);
	Log.Log("INFO", "Audio mixing channels allocated: %d", Mix_AllocateChannels(-1));

	Initialized = true;
}

CSoundMixer::~CSoundMixer()
{
	Mix_CloseAudio();
}
