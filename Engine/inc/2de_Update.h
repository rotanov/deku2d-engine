#ifndef _2DE_UPDATE_H_
#define _2DE_UPDATE_H_

#include "2de_Core.h"

// Instead let there be visitor

class IVisitorBase
{
public:	
	virtual ~IVisitorBase() {};
};

template <typename T>
class IVisitor : public IVisitorBase
{
public:
	virtual void Visit(T&) = 0;
};

class IVisitableBase
{
public:
	virtual ~IVisitableBase() {}
	virtual void Accept(IVisitorBase&) = 0;

protected:
	template <class T> 
	static void ConcreteAccept(T& visited, IVisitorBase& visitor)
	{
		if (IVisitor<T>* ptr = dynamic_cast<IVisitor<T>*>(&visitor))
			ptr->Visit(visited);
		return;
	}
};


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
