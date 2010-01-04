#ifndef _SOUND_CHECK_H
#define _SOUND_CHECK_H

#include "2de_Engine.h"

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
		CEngine::Instance()->GetState(STATE_SCREEN_HEIGHT, &ScreenHeight);

		FontManager->PrintEx(5, ScreenHeight - 30, 0.0f, "Sound: q - play, w - stop"); 
		FontManager->PrintEx(5, ScreenHeight - 60, 0.0f, "Music: a - play, s - stop"); 
		FontManager->PrintEx(5, ScreenHeight - 90, 0.0f, "You can try to enter file name and click Play");
		FontManager->PrintEx(5, ScreenHeight - 120, 0.0f, "to play it as music");

		CPrimitiveRender pr;
		/*pr.lClr = RGBAf(0.5f, 0.5f, 0.5f, 0.5f);
		pr.sClr = RGBAf(0.5f, 0.5f, 0.5f, 0.5f);
		pr.pClr = RGBAf(0.5f, 0.5f, 0.5f, 0.5f);*/
		glLoadIdentity();
		pr.grArrowL(Vector2(320.0f, (scalar) (ScreenHeight - 240)), Vector2(320.0f, (scalar) (ScreenHeight - 200)));

		FontManager->PrintEx(5, ScreenHeight - 270, 0.0f, "GUI is unstable and may crash - you've been warned...");

		return true;
	}
};

bool PlayFile()
{
	string test = dynamic_cast<CEdit*>(GuiManager.GetLast()->GetData())->Text;
	Log.Log("SOUNDCHECK", "File name string from GUI: %s", test.c_str());
	CMusic *mus = new CMusic(test.c_str());
	mus->LoadFromFile();
	CSoundMixer::Instance()->PlayMusic(mus);
	return true;
}

class CLoadFileGUI
{
public:
	CLoadFileGUI()
	{
		LoadFileButton = new CButton(CAABB(400, 300, 100, 32), "Play", RGBAf(0.4f, 0.4f, 0.4f, 1.0f), PlayFile);
		LoadFileButton->SetParent(&GuiManager);

		FileNameEdit = new CEdit;
		FileNameEdit->aabb = CAABB(80, 300, 300, 32);
		FileNameEdit->Text = "";
		FileNameEdit->color = RGBAf(0.5f, 0.5f, 0.6f, 0.9f);
		FileNameEdit->SetParent(&GuiManager);


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
