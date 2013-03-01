#ifndef _2DE_SOUND_H_
#define _2DE_SOUND_H_

#include <SDL/SDL_mixer.h>

#include "2de_Core.h"
#include "2de_Resource.h"
#include "2de_CommonManager.h"
#include "2de_Object.h"
#include "2de_Singleton.h"

namespace Deku2D
{
	/**
	* CSound - sound as resource class.
	*/

	class CSound : public CResource
	{
		D2D_TYPE_INFO_INJECT(CSound)
	public:
		CSound();
		~CSound();

		bool Load();
		void Unload();

		Mix_Chunk* GetData();

	private:
		Mix_Chunk *Data;
	};

	/**
	* CMusic - music as resource class.
	*/

	class CMusic : public CResource
	{
		D2D_TYPE_INFO_INJECT(CMusic)
	public:
		CMusic();
		~CMusic();

		bool Load();
		void Unload();

		Mix_Music* GetData();

	private:
		Mix_Music *Data;
	};

	/**
	* CSoundManager - sound samples resource manager.
	*/

	class CSoundManager : public CCommonManager <list <CSound*> >
	{
	public:
		CSound* GetSoundByName(const string &SoundName);

	protected:
		CSoundManager();
		friend class CTSingleton <CSoundManager>;
	};

	static CTSingleton<CSoundManager> SoundManager;

	/**
	* CMusicManager - music resource manager.
	*/

	class CMusicManager : public CCommonManager <list <CMusic*> >
	{
	public:
		CMusic* GetMusicByName(const string &MusicName);

	protected:
		CMusicManager();
		friend class CTSingleton <CMusicManager>;
	};

	static CTSingleton<CMusicManager> MusicManager;

	/**
	* CSoundMixer - sound mixer, that is responsible for initialization/deinitialization of sound system and playing sounds and music.
	*/

	namespace SoundMixerHooks
	{

	void OnMusicFinished();
	void OnSoundFinished(int AChannel);

	};


	class CSoundMixer : public CObject
	{
	public:
		bool PlaySound(CSound *Sound, int Time = -1);
		void StopAllSound();
		bool PlayMusic(CMusic *Music, int FadeInTime = 0, int Loops = 1);
		void PauseMusic();
		void ResumeMusic();
		void StopMusic(int FadeOutTime = 0);
		void SetMusicVolume(size_t AVolume);
		size_t GetMusicVolume() const;
		void SetSoundVolume(CSound *ASound, size_t AVolume);
		size_t GetSoundVolume(CSound *ASound) const;

		// well, there are many other features in SDL_mixer (like effects, channel groups, etc.), but i'm too lazy to implement interface to them now...

	protected:
		CSoundMixer();
		~CSoundMixer();
		friend class CTSingleton<CSoundMixer>;

	private:
		bool Initialized;
		//size_t DefaultBGMVolume;	// Could be named DefaultMusicVolume // И вообще нужно ли это хранить здесь? Вот это - точно не нужно.
		//size_t DefaultSFXVolume;	// Could be named DefaultSoundVolume // А это - нужно, я считаю, так как громкость устанавливается для каждого отдельного звука.

		CResourceRefCounter<CMusic> CurrentMusic;
		map<int, CResourceRefCounter<CSound> > CurrentSounds;

		const int SOUND_MIXING_CHANNELS_COUNT;

		friend void SoundMixerHooks::OnMusicFinished();
		friend void SoundMixerHooks::OnSoundFinished(int AChannel);
		CSoundMixer& operator =(const CSoundMixer& rhs);
	};

	static CTSingleton<CSoundMixer> SoundMixer;

}	//	namespace Deku2D

#endif // _2DE_SOUND_H_
