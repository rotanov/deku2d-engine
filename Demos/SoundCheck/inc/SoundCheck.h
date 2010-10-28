#ifndef _SOUND_CHECK_H
#define _SOUND_CHECK_H

#include "2de_Engine.h"
#include "2de_Gui.h"

class CSoundObject : public CObject
{
public:
	CSoundObject()
	{
		CEventManager::Instance()->Subscribe("KeyDown", this);
		CEventManager::Instance()->Subscribe("ButtonClick", this);

		VolumeLabel = CFactory::Instance()->New<CLabel>("VolumeLabel");
		VolumeLabel->SetBox(CBox(530, 270, 100, 32));

		Volume = CSoundMixer::Instance()->GetMusicVolume();
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
		else if (AEvent.GetName() == "ButtonClick")
		{
			if (AEvent.GetSender()->GetName() == "LoadFileButton")
			{
				string test = CFactory::Instance()->Get<CEdit>("FileNameEdit")->GetText();
				CMusic *mus = CFactory::Instance()->New<CMusic>(""); // WRONG, i think
											// i mean we should specify just name of music file, that already
											// lies int the music folder
											// 	i just wanted to play some music from filesystem, why not?..
											// 	ofcourse, you can do Get<CMusic>("name_of_music_resource_already_loaded_by_engine"),
											// 	but there are too few files in our Data/Music directory to properly test sound system..
				mus->SetLoadSource(test);
				mus->Load();
				CSoundMixer::Instance()->PlayMusic(mus);
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
		CText *HelpText = CFactory::Instance()->New<CText>("HelpText");
		HelpText->Position = Vector2(5.0f, CGLWindow::Instance()->GetHeight() - 260.0f);
		HelpText->SetText("Sound: q - play, w - stop\nMusic: a - play, s - stop\nMusic volume: z - down, x - up\n"
			"You can try to enter file name and click Play\nto play it as music");
	}
};

class CLoadFileGUI : public CObject
{
public:
	CLoadFileGUI()
	{
		LoadFileButton = CFactory::Instance()->New<CButton>("LoadFileButton");
		LoadFileButton->SetBox(CBox(400, 270, 100, 32));
		LoadFileButton->SetText("Play");

		FileNameEdit = CFactory::Instance()->New<CEdit>("FileNameEdit");
		FileNameEdit->SetBox(CBox(80, 270, 300, 32));
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
