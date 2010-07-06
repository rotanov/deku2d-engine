#ifndef _GAME_H_
#define _GAME_H_

#include "2de_Engine.h"
#include <SDL/SDL_opengl.h>

class CTestUnit : public CRenderObject, public CUpdateObject
{
public:
	CTexture *shroom;
	CTestUnit()
	{
		int ScrWidth, ScrHeight;
		CEngine::Instance()->GetState(CEngine::STATE_SCREEN_WIDTH, &ScrWidth);
		CEngine::Instance()->GetState(CEngine::STATE_SCREEN_HEIGHT, &ScrHeight);

		CText *TitleText = CFactory::Instance()->New<CText>("txtDeku");
		TitleText->SetText("Deku");
		TitleText->Position = Vector2(ScrWidth * 0.5f + 15.0f, ScrHeight * 0.5f - 22.0f);
		TitleText = CFactory::Instance()->New<CText>("txtTeam");
		TitleText->SetText("team");
		TitleText->Position = Vector2(ScrWidth * 0.5f + 15.0f, ScrHeight * 0.5f - 35.0f);

		shroom = CFactory::Instance()->New<CTexture>("Shroom");
		shroom->LoadTexture(IMAGE_SHROOM_TITLE_WIDTH, IMAGE_SHROOM_TITLE_HEIGHT, reinterpret_cast<byte *>(IMAGE_SHROOM_TITLE_DATA));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	void Render()
	{
		int ScrWidth, ScrHeight;
		CEngine::Instance()->GetState(CEngine::STATE_SCREEN_WIDTH, &ScrWidth);
		CEngine::Instance()->GetState(CEngine::STATE_SCREEN_HEIGHT, &ScrHeight);

		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		shroom->Bind();
		
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(ScrWidth * 0.5f - 50.0f, ScrHeight * 0.5f - 50.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(ScrWidth * 0.5f + 50.0f, ScrHeight * 0.5f - 50.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(ScrWidth * 0.5f + 50.0f, ScrHeight * 0.5f + 50.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(ScrWidth * 0.5f - 50.0f, ScrHeight * 0.5f + 50.0f);
		glEnd();
	}
	void Update(float dt){}
};


#endif //_GAME_H_
