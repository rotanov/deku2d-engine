#ifndef CEDITORREGISTRY_H
#define CEDITORREGISTRY_H

#include <map>
#include <QStringList>

#include "Editors/CAbstractEditor.h"

using namespace std;


class CAbstractEditorCreator
{
public:
	virtual CAbstractEditor* Create() = 0;
	virtual QString GetEditorName() = 0;
};


template<typename T>
class CEditorCreator : public CAbstractEditorCreator
{
public:
	CAbstractEditor* Create();
	QString GetEditorName();
};


template<typename T>
CAbstractEditor* CEditorCreator<T>::Create()
{
	return new T;
}

template<typename T>
QString CEditorCreator<T>::GetEditorName()
{
	return T::GetNameStatic();
}


class CEditorRegistry
{
public:
	static CEditorRegistry* Instance();

	template<typename T>
	void RegisterEditor();

	QStringList GetFilters();

	CAbstractEditor* CreateEditorByFilter(const QString &AFilter);
	CAbstractEditor* CreateEditorByName(const QString &AName);

	int CountByFilter(const QString &AFilter);
	QStringList GetEditorsForFilter(const QString &AFilter);
	QStringList GetAllEditors();

private:
	CEditorRegistry();
	~CEditorRegistry();

	typedef multimap<QString, CAbstractEditorCreator *> TCreatorsContainer;
	TCreatorsContainer Creators;
};


template<typename T>
void CEditorRegistry::RegisterEditor()
{
	QStringList filters = T::GetFiltersStatic();

	for (QStringList::iterator it = filters.begin(); it != filters.end(); ++it)
		Creators.insert(make_pair(*it, new CEditorCreator<T>()));
}

#endif // CEDITORREGISTRY_H
