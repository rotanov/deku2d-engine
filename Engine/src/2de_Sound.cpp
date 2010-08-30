#include "2de_Sound.h"

//////////////////////////////////////////////////////////////////////////
// CSound

CSound::CSound()
{
	Filename = "";
	Data = NULL;
	SetName("CSound");
	CSoundManager::Instance()->Add(this);
}

CSound::CSound(const char *AFileName)
{
	Filename = AFileName;
	Data = NULL;
	SetName("CSound");
	CSoundManager::Instance()->Add(this);
}

CSound::~CSound()
{
	if (Loaded && Data != NULL)
	{
		// as written in SDL_mixer doc, it's a bad idea to free chunk being played, so maybe we should stop all sounds.. but i'm not sure...
		// Mix_HaltChannel(-1);

		Mix_FreeChunk(Data);
		Data = NULL;
	}

	CSoundManager::Instance()->Remove(this);
}

bool CSound::LoadFromFile()
{
	if (Filename == "")
		return false;
	
	Data = Mix_LoadWAV(Filename.c_str());
	if (Data == NULL)
	{
		Log("ERROR", "Can't load sound '%s': %s", Filename.c_str(), Mix_GetError());
	}

	return true;
}

Mix_Chunk* CSound::GetData()
{
	return Data;
}

//////////////////////////////////////////////////////////////////////////
// CMusic

CMusic::CMusic()
{
	Filename = "";
	Data = NULL;
	SetName("CMusic");
	CMusicManager::Instance()->Add(this);
}

CMusic::CMusic(const char *AFileName)
{
	Filename = AFileName;
	Data = NULL;
	SetName("CMusic");
	CMusicManager::Instance()->Add(this);
}

CMusic::~CMusic()
{
	if (Loaded && Data != NULL)
	{
		// Mix_HaltMusic();

		Mix_FreeMusic(Data);
		Data = NULL;
	}

	CMusicManager::Instance()->Remove(this);
}

bool CMusic::LoadFromFile()
{
	if (Filename == "")
		return false;
	
	Data = Mix_LoadMUS(Filename.c_str());
	if (Data == NULL)
	{
		Log("ERROR", "Can't load music '%s': %s", Filename.c_str(), Mix_GetError());
	}

	return true;
}

Mix_Music* CMusic::GetData()
{
	return Data;
}

//////////////////////////////////////////////////////////////////////////
// CSoundManager

CSound* CSoundManager::GetSoundByName(const string &SoundName)
{
	CSound *Result = NULL;
	Result = Get(SoundName);
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
	Result = Get(MusicName);
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
		Log("ERROR", "Can't play sound '%s': %s", Sound->GetName().c_str(), Mix_GetError());
		return false;
	}
	// May be we have to set sound volume here?
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
		Log("ERROR", "Can't play music '%s': %s", Music->GetName().c_str(), Mix_GetError());
		return false;
	}

	return true;
}

void CSoundMixer::PauseMusic()
{
	if (!Mix_PlayingMusic())
		return;

	Mix_PauseMusic();
}

void CSoundMixer::ResumeMusic()
{
	if (!Mix_PausedMusic())
		return;

	Mix_ResumeMusic();
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

void CSoundMixer::SetMusicVolume(size_t AVolume)
{
	if (AVolume > MIX_MAX_VOLUME)
	{
		Log("WARNING", "Volume should be between 0 and MIX_MAX_VOLUME (%d)", MIX_MAX_VOLUME);
		return;
	}

	Mix_VolumeMusic(AVolume);
}

size_t CSoundMixer::GetMusicVolume() const
{
	return Mix_VolumeMusic(-1);
}

void CSoundMixer::SetSoundVolume(CSound *ASound, size_t AVolume)
{
	if (!Initialized || ASound == NULL)	// What if ASound == NULL
	{
		Log("ERROR", "SetSoundVolume Mixer should be initialized, or ASound is NULL");
		return;
	}
	if (AVolume > MIX_MAX_VOLUME)
	{
		Log("WARNING", "Volume should be between 0 and MIX_MAX_VOLUME (%d)", MIX_MAX_VOLUME);
		return;
	}

	Mix_VolumeChunk(ASound->GetData(), AVolume);
}

size_t CSoundMixer::GetSoundVolume(CSound *ASound) const
{
	if (!Initialized || ASound == NULL)	// What if ASound == NULL
	{
		Log("ERROR", "SetSoundVolume Mixer should be initialized, or ASound is NULL");
		return 0;
	}

	return Mix_VolumeChunk(ASound->GetData(), -1);
}

CSoundMixer::CSoundMixer() : SOUND_MIXING_CHANNELS_COUNT(32) // : DefaultBGMVolume(MIX_MAX_VOLUME), DefaultSFXVolume(MIX_MAX_VOLUME)
{
	SetName("Sound mixer");
	Initialized = false;

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
	{
		// well, absense of sound is not so critical, so we can continue.. but:
		// 	1. we should notice user in some way (may be log is sufficent)
		// 	2. all methods of this class should check Initialized value before attempting to use sound device
		//
		Log("ERROR", "Can't initialize audio subsystem: %s", SDL_GetError());
		return;
	}

	// @todo: replace by values from config
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) < 0)
	{
		Log("ERROR", "Can't open audio device: %s", Mix_GetError());
		return;
	}
	
	Mix_AllocateChannels(SOUND_MIXING_CHANNELS_COUNT);
	Log("INFO", "Audio mixing channels allocated: %d", Mix_AllocateChannels(-1));

	Initialized = true;
}

CSoundMixer::~CSoundMixer()
{
	Mix_CloseAudio();
}
