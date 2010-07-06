#ifndef _SOUND_CHECK_H
#define _SOUND_CHECK_H

#include "2de_Engine.h"
#include "2de_Gui.h"

class CSoundObject : public CObject
{
public:
	CSoundObject() : Volume(MIX_MAX_VOLUME)
	{
		CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
		VolumeLabel = CFactory::Instance()->New<CLabel>("VolumeLabel");
		VolumeLabel->aabb = CAABB(530, 270, 100, 32);
		UpdateVolumeLabel();
	}

	bool InputHandling(Uint8 state, Uint16 key, SDLMod mod, char letter)
	{
		if (state == KEY_PRESSED)
		{
			switch (key)
			{
			case SDLK_q:
				CSoundMixer::Instance()->PlaySound(CSoundManager::Instance()->GetSoundByName("chord"));
				break;
			case SDLK_w:
				CSoundMixer::Instance()->StopAllSound();
				break;

			case SDLK_a:
				CSoundMixer::Instance()->PlayMusic(CMusicManager::Instance()->GetMusicByName("bender1"));
				break;
			case SDLK_s:
				CSoundMixer::Instance()->StopMusic();
				break;

			case SDLK_z:
				if (Volume > 0)
				{
					Volume--;
					UpdateVolumeLabel();
				}
				CSoundMixer::Instance()->SetMusicVolume(Volume);
				break;
			case SDLK_x:
				if (Volume < MIX_MAX_VOLUME)
				{
					Volume++;
					UpdateVolumeLabel();
				}
				CSoundMixer::Instance()->SetMusicVolume(Volume);
				break;
			}

		}
		return true;
	}
private:
	void UpdateVolumeLabel()
	{
		VolumeLabel->Text = itos(Volume);		
	}
	int Volume;
	CLabel *VolumeLabel;
};

class CHelpText
{
public:
	CHelpText()
	{
		CFontManager::Instance()->SetCurrentFont("hge");

		// i hate this fucking "State"... completely wrong, not type-safe, inconvenient design...
		int ScreenHeight;
		CEngine::Instance()->GetState(CEngine::STATE_SCREEN_HEIGHT, &ScreenHeight);

		CText *HelpText = CFactory::Instance()->New<CText>("SoundControlsText");
		//HelpText->Position = Vector2(5.0f, ScreenHeight - 30.0f);
		HelpText->Position = Vector2(5.0f, ScreenHeight - 260.0f);
		HelpText->SetText("Sound: q - play, w - stop");

		HelpText = CFactory::Instance()->New<CText>("MusicControlsText");
		HelpText->Position = Vector2(5.0f, ScreenHeight - 270.0f);
		HelpText->SetText("Music: a - play, s - stop");

		HelpText = CFactory::Instance()->New<CText>("MusicVolumeText");
		HelpText->Position = Vector2(5.0f, ScreenHeight - 280.0f);
		HelpText->SetText("Music volume: z - down, x - up");

		HelpText = CFactory::Instance()->New<CText>("FileText1");
		HelpText->Position = Vector2(5.0f, ScreenHeight - 290.0f);
		HelpText->SetText("You can try to enter file name and click Play");

		HelpText = CFactory::Instance()->New<CText>("FileText2");
		HelpText->Position = Vector2(5.0f, ScreenHeight - 300.0f);
		HelpText->SetText("to play it as music");
	}
};

bool PlayFile(CObject *Caller)
{
	string test = CFactory::Instance()->Get<CEdit>("FileNameEdit")->Text;
	Log("SOUNDCHECK", "File name string from GUI: %s", test.c_str());
	CMusic *mus = CFactory::Instance()->New<CMusic>("mus");
	mus->Filename = test;
	mus->LoadFromFile();
	CSoundMixer::Instance()->PlayMusic(mus);
	return true;
}

class CLoadFileGUI
{
public:
	CLoadFileGUI()
	{
		LoadFileButton = CFactory::Instance()->New<CButton>("LoadFileButton");
		LoadFileButton->aabb = CAABB(400, 270, 100, 32);
		LoadFileButton->Text = "Play";
		LoadFileButton->SetCallback(&PlayFile, NULL);

		FileNameEdit = CFactory::Instance()->New<CEdit>("FileNameEdit");
		FileNameEdit->aabb = CAABB(80, 270, 300, 32);
		FileNameEdit->Text = "";


	}
private:
	CEdit *FileNameEdit;
	CButton *LoadFileButton;

};

class CSoundCheck : public CObject
{
	CSoundObject SoundObject;
	CHelpText HelpText;
	CLoadFileGUI LoadFileGUI;
};

#endif // _SOUND_CHECK_H
