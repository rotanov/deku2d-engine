#ifndef CCODEEDITOR_H
#define CCODEEDITOR_H

#include <QString>

#include "Editors/CAbstractEditor.h"
#include "CCodeEditorWidget.h"


class CCodeEditor : public CAbstractEditor
{
	Q_OBJECT
public:
	explicit CCodeEditor();
	~CCodeEditor();

	QString GetName();
	QStringList GetFilters();

	static QString GetNameStatic();
	static QStringList GetFiltersStatic();

	QWidget* GetWidget();

	bool isModified();

public slots:
	void ModificationChanged(bool AValue);

protected:
	void InternalOpen(const QString &AFileName);
	void InternalSave(const QString &AFileName);

private:
	CCodeEditorWidget *Widget;
};

#endif // CCODEEDITOR_H
