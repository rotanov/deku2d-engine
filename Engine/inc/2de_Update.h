#ifndef _2DE_UPDATE_H_
#define _2DE_UPDATE_H_

#include "2de_Core.h"

class CUpdateObject : public virtual CObject
{
public:
	bool Active;
	CUpdateObject();
	~CUpdateObject();
	bool isDead() const;
	void SetDead();
	virtual void Update(float dt) = 0;
private:
	bool Dead;
};

/**
*	CUpdateManager - менеджер объектов, которые следует обновлять. Такие дела.
*	Да, тут мало кода, надо ещё какие-нибуть ф-ии нахерачить. @todo!
*	И вообще что-то мне подсказывает, что он не здесь должен быть. // пришлось вообще перенести в отдельный файл..
*/

class CUpdateManager : public CCommonManager <list<CUpdateObject*> >, public CTSingleton <CUpdateManager>
{
public:	
	bool UpdateObjects();
protected:
	CUpdateManager();
	friend class CTSingleton<CUpdateManager>;
};


#endif // _2DE_UPDATE_H_
