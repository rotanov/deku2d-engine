#ifndef _2DE_SOUND_H
#define _2DE_SOUND_H

#include <SDL/SDL_mixer.h>

#include "2de_Core.h"

#define SOUND_MIXING_CHANNELS_COUNT 32

/**
 * CSound - класс звукового семлпа как ресурса.
 */

class CSound : public CResource
{
public:
	CSound();
	CSound(const char *AFileName);
	~CSound();
	static CObject* NewSound()
	{
		return new CSound;
	}
	bool LoadFromFile();
	__INLINE Mix_Chunk* GetData() { return Data; }

private:
	Mix_Chunk *Data;
};

/**
 * CMusic - класс музыки как ресурса.
 */

class CMusic : public CResource
{
public:
	CMusic();
	CMusic(const char *AFileName);
	~CMusic();
	static CObject* NewMusic()
	{
		return new CMusic;
	}
	bool LoadFromFile();
	__INLINE Mix_Music* GetData() { return Data; }

private:
	Mix_Music *Data;
};

/**
 * CSoundManager - ресурсовый менеджер звуковых семплов.
 */

class CSoundManager : public CList, public CTSingleton<CSoundManager>
{
public:
	CSound* GetSoundByName(const string &SoundName);

protected:
	CSoundManager();
	friend class CTSingleton<CSoundManager>;
};

/**
 * CMusicManager - ресурсовый менеджер музыки.
 */

class CMusicManager : public CList, public CTSingleton<CMusicManager>
{
public:
	CMusic* GetMusicByName(const string &MusicName);

protected:
	CMusicManager();
	friend class CTSingleton<CMusicManager>;
};

/**
 * CSoundMixer - звуковой микшер, отвечающий за инициализацию и деинициализацию звуковой системы, а также воспроизведение звука и музыки.
 */

class CSoundMixer : public CTSingleton<CSoundMixer>
{
public:
	bool PlaySound(CSound *Sound, int Time = -1);
	void StopAllSound();
	bool PlayMusic(CMusic *Music, int FadeInTime = 0);
	void StopMusic(int FadeOutTime = 0);
	void SetMusicVolume(int AVolume);
	int GetMusicVolume() const;
	void SetSoundVolume(CSound *ASound, int AVolume);
	int GetSoundVolume(CSound *ASound) const;

	// well, there are many other features in SDL_mixer (like effects, channel groups, etc.), but i'm too lazy to implement interface to them now...

protected:
	CSoundMixer();
	~CSoundMixer();
	friend class CTSingleton<CSoundMixer>;

private:
	bool Initialized;

};

#endif // _2DE_SOUND_H
