#ifndef _SOUND_CHECK_H
#define _SOUND_CHECK_H

#include "2de_Engine.h"
#include "2de_Gui.h"

class CSoundObject : public CObject
{
public:
	CSoundObject()
	{
		CEngine::Instance()->AddKeyInputFunction(&CObject::InputHandling, this);
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
			}

		}
		return true;
	}
};

class CHelpText : public CRenderObject
{
public:
	CHelpText()
	{
		CFontManager::Instance()->SetCurrentFont("hge");
	}
	bool Render()
	{
		CFontManager *FontManager = CFontManager::Instance();

		// i hate this fucking "State"... completely wrong, not type-safe, inconvenient design...
		int ScreenHeight;
		CEngine::Instance()->GetState(CEngine::STATE_SCREEN_HEIGHT, &ScreenHeight);

		FontManager->PrintEx(5, ScreenHeight - 30, 0.0f, "Sound: q - play, w - stop"); 
		FontManager->PrintEx(5, ScreenHeight - 60, 0.0f, "Music: a - play, s - stop"); 
		FontManager->PrintEx(5, ScreenHeight - 90, 0.0f, "You can try to enter file name and click Play");
		FontManager->PrintEx(5, ScreenHeight - 120, 0.0f, "to play it as music");

		return true;
	}
};

bool PlayFile(CObject *Caller)
{
	string test = CFactory::Instance()->Get<CEdit>("FileNameEdit")->Text;
	//string test = dynamic_cast<CEdit*>(CGUIManager::Instance()->GetLast()->GetData())->Text;
	Log.Log("SOUNDCHECK", "File name string from GUI: %s", test.c_str());
	CMusic *mus = CFactory::Instance()->New<CMusic>("mus");
	mus->filename = test;
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
		LoadFileButton->aabb = CAABB(400, 300, 100, 32);
		LoadFileButton->Text = "Play";
		LoadFileButton->SetCallback(&PlayFile, NULL);

		FileNameEdit = CFactory::Instance()->New<CEdit>("FileNameEdit");
		FileNameEdit->aabb = CAABB(80, 300, 300, 32);
		FileNameEdit->Text = "";


	}
private:
	CEdit *FileNameEdit;
	CButton *LoadFileButton;

};

class CSoundCheck
{
	CSoundObject SoundObject;
	CHelpText HelpText;
	CLoadFileGUI LoadFileGUI;
};

#endif // _SOUND_CHECK_H
