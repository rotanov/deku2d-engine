#ifndef _2DE_UPDATE_H_
#define _2DE_UPDATE_H_

#include "2de_Core.h"


//////////////////////////////////////////////////////////////////////////

/**
* Ruins of the mighty CUpdateManager
*/

class CGameObject;

class CUpdateManager : public CTSingleton <CUpdateManager>
{
public:	
	bool UpdateObjects();
	CGameObject *RootGameObject;	// TODO: move somewhere.. possibly to CEngine or some other place..
					// 	also, easily obtainable from factory using CFactory::Instance()->Get("RootGameObject");

protected:
	CUpdateManager();
	friend class CTSingleton<CUpdateManager>;
};

#endif // _2DE_UPDATE_H_
