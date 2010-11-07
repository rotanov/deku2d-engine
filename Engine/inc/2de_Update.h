#ifndef _2DE_UPDATE_H_
#define _2DE_UPDATE_H_

#include "2de_Core.h"

class CAbstractScene;	// Separate header, may be?

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

class CUpdateManager : public CTSingleton <CUpdateManager>
{
public:	
	bool UpdateObjects();
	CGameObject *RootGameObject;

protected:
	CUpdateManager();
	friend class CTSingleton<CUpdateManager>;
};

class CTimerComponent : public CGameObject
{
public:
	CTimerComponent();
	CTimerComponent(float AInterval);

	void ProcessEvent(const CEvent &AEvent);

	bool isEnabled() const;
	void SetEnabled(bool AEnabled);

	float GetInterval() const;
	void SetInterval(float AInterval);

private:
	bool Enabled;
	float Interval;
	float Accumulated;

};

#endif // _2DE_UPDATE_H_
