#ifndef _GAME_H_
#define _GAME_H_

#include "2de_Engine.h"

class CTestUnit : public CRenderObject, public CUpdateObject
{
public:
	CTestUnit()
	{
		//SetName((string)"CTestUnit" + GetID());
	}
	bool Render();
	bool Update(float dt);
};


#endif //_GAME_H_