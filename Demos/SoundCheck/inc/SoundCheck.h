#ifndef _SOUND_CHECK_H
#define _SOUND_CHECK_H

#include "2de_Engine.h"
#include "2de_Gui.h"
using namespace Deku2D;

class CSoundObject : public CObject
{
public:
	CSoundObject()
	{
		EventManager->Subscribe("KeyDown", this);
		EventManager->Subscribe("ButtonClick", this);

		VolumeLabel = Factory->New<CLabel>("VolumeLabel");
		VolumeLabel->SetBox(CBox(530, 270, 100, 32));

		Volume = SoundMixer->GetMusicVolume();
		UpdateVolumeLabel();
	}


	void ProcessEvent(const CEvent &AEvent)
	{
		if (AEvent.GetName() == "KeyDown")
		{
			Uint16 key = AEvent.GetData<Uint16>("Sym");
			switch (key)
			{
			case SDLK_q:
				SoundMixer->PlaySound(SoundManager->GetSoundByName("chord"));
				break;
			case SDLK_w:
				SoundMixer->StopAllSound();
				break;

			case SDLK_a:
				SoundMixer->PlayMusic(MusicManager->GetMusicByName("bender1"));
				break;
			case SDLK_s:
				SoundMixer->StopMusic();
				break;

			case SDLK_z:
				if (Volume > 0)
				{
					Volume--;
					UpdateVolumeLabel();
				}
				SoundMixer->SetMusicVolume(Volume);
				break;
			case SDLK_x:
				if (Volume < MIX_MAX_VOLUME)
				{
					Volume++;
					UpdateVolumeLabel();
				}
				SoundMixer->SetMusicVolume(Volume);
				break;
			}
		}
		else if (AEvent.GetName() == "ButtonClick")
		{
			if (AEvent.GetSender()->GetName() == "LoadFileButton")
			{
				string test = Factory->Get<CEdit>("FileNameEdit")->GetText();
				CMusic *mus = Factory->New<CMusic>(""); // WRONG, i think
											// i mean we should specify just name of music file, that already
											// lies int the music folder
											// 	i just wanted to play some music from filesystem, why not?..
											// 	ofcourse, you can do Get<CMusic>("name_of_music_resource_already_loaded_by_engine"),
											// 	but there are too few files in our Data/Music directory to properly test sound system..
				mus->SetLoadSource(test);
				mus->Load();
				SoundMixer->PlayMusic(mus);
			}
		}
	}

private:
	void UpdateVolumeLabel()
	{
		VolumeLabel->SetText(itos(Volume));
	}
	int Volume;
	CLabel *VolumeLabel;
};

class CHelpText
{
public:
	CHelpText()
	{
		CText *HelpText = Factory->New<CText>("HelpText");
		//  We can't set position to text, since placing is a separate component
		//	So i commented it out
		//	HelpText->SetPosition(Vector2(5.0f, GLWindow->GetHeight() - 260.0f));
		HelpText->SetText("Sound: q - play, w - stop\nMusic: a - play, s - stop\nMusic volume: z - down, x - up\n"
			"You can try to enter file name and click Play\nto play it as music");
		CEngine::Instance()->RootGameObject->Attach(HelpText);
	}
};

class CLoadFileGUI : public CObject
{
public:
	CLoadFileGUI()
	{
		LoadFileButton = Factory->New<CButton>("LoadFileButton");
		LoadFileButton->SetBox(CBox(400, 270, 100, 32));
		LoadFileButton->SetText("Play");
		CEngine::Instance()->RootGameObject->Attach(LoadFileButton);

		FileNameEdit = Factory->New<CEdit>("FileNameEdit");
		FileNameEdit->SetBox(CBox(80, 270, 300, 32));
		CEngine::Instance()->RootGameObject->Attach(FileNameEdit);
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
