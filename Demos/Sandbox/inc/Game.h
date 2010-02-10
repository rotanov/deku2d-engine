#ifndef _GAME_H_
#define _GAME_H_

#include "2de_Engine.h"
#include <SDL\SDL_opengl.h>

extern const unsigned int IMAGE_SHROOM_WIDTH;
extern const unsigned int IMAGE_SHROOM_HEIGHT;
extern char IMAGE_SHROOM_DATA[];


class CTestUnit : public CRenderObject, public CUpdateObject
{
public:
	CTexture shroom;
	CTestUnit()
	{
		//SetName((string)"CTestUnit" + GetID());
		void *temp = IMAGE_SHROOM_DATA;
		shroom.width = IMAGE_SHROOM_WIDTH;
		shroom.height = IMAGE_SHROOM_HEIGHT;
		shroom.data = (byte*)temp;
		shroom.bpp = 4;
		shroom.MakeTexture();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	bool Render()
	{
		int ScrWidth, ScrHeight;
		CEngine::Instance()->GetState(STATE_SCREEN_WIDTH, &ScrWidth);
		CEngine::Instance()->GetState(STATE_SCREEN_HEIGHT, &ScrHeight);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnable(GL_TEXTURE_2D);
		shroom.Bind();
		
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(ScrWidth * 0.5f - 50.0f, ScrHeight * 0.5f - 50.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(ScrWidth * 0.5f + 50.0f, ScrHeight * 0.5f - 50.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(ScrWidth * 0.5f + 50.0f, ScrHeight * 0.5f + 50.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(ScrWidth * 0.5f - 50.0f, ScrHeight * 0.5f + 50.0f);
		glEnd();

		CFontManager::Instance()->Print(ScrWidth * 0.5f + 15.0f, ScrHeight * 0.5f - 22.0f, 0.0f, "Deku");
		CFontManager::Instance()->Print(ScrWidth * 0.5f + 15.0f, ScrHeight * 0.5f - 35.0f, 0.0f, "team");
		return true;
	}
	bool Update(float dt)
	{
		return true;
	}
};


#endif //_GAME_H_