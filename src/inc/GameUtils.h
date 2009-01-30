#ifndef  _GAME_UTILS_H_
#define _GAME_UTILS_H_

#include "CoreUtils.h"
#include "OpenglUtils.h"

class CLevelMap : public CRenderResource
{
public:



	CLevelMap()
	{
		// todo : in cpp ->
		name = "CLevelMap";
		type = T_RENDERABLE | T_UPDATABLE;
	}
	~CLevelMap(){}
	

};

//class CTileset : public C

#endif