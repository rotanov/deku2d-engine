#include "CEditorRegistry.h"

CEditorRegistry::CEditorRegistry()
{
}

CEditorRegistry::~CEditorRegistry()
{
	for (TCreatorsContainer::iterator it = Creators.begin(); it != Creators.end(); ++it)
		delete (*it).second;
	Creators.clear();
}

CEditorRegistry* CEditorRegistry::Instance()
{
	static CEditorRegistry instance;
	return &instance;
}

QStringList CEditorRegistry::GetFilters()
{
	QStringList result;
	for (TCreatorsContainer::iterator it = Creators.begin(); it != Creators.end(); ++it)
		result.append((*it).first);
	result.removeDuplicates();
	return result;
}

CAbstractEditor* CEditorRegistry::CreateEditorByFilter(const QString &AFilter)
{
	TCreatorsContainer::const_iterator it = Creators.find(AFilter);
	if (it == Creators.end())
		return NULL;

	return (*it).second->Create();
}

CAbstractEditor* CEditorRegistry::CreateEditorByName(const QString &AName)
{
	for (TCreatorsContainer::const_iterator it = Creators.begin(); it != Creators.end(); ++it)
	{
		if ((*it).second->GetEditorName() == AName)
			return (*it).second->Create();
	}

	return NULL;
}

int CEditorRegistry::CountByFilter(const QString &AFilter)
{
	return Creators.count(AFilter);
}

QStringList CEditorRegistry::GetEditorsForFilter(const QString &AFilter)
{
	QStringList result;

	for (TCreatorsContainer::const_iterator it = Creators.lower_bound(AFilter); it != Creators.upper_bound(AFilter); ++it)
		result.append((*it).second->GetEditorName());

	return result;
}

QStringList CEditorRegistry::GetAllEditors()
{
	QStringList result;

	for (TCreatorsContainer::const_iterator it = Creators.begin(); it != Creators.end(); ++it)
		result.append((*it).second->GetEditorName());

	return result;
}
