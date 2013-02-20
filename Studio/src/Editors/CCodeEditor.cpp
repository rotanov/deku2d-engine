#include "Editors/CCodeEditor.h"

#include <Qsci/qsciscintilla.h>
#include <2de_Core.h>
#include <2de_Storage.h>

using namespace Deku2D;


CCodeEditor::CCodeEditor()
{
	Widget = new CCodeEditorWidget(this);
	connect(
		Widget->SCI, SIGNAL(modificationChanged(bool)),
		this, SLOT(ModificationChanged(bool))
	);
}

CCodeEditor::~CCodeEditor()
{
	delete Widget;
}

QString CCodeEditor::GetName()
{
	return GetNameStatic();
}

QStringList CCodeEditor::GetFilters()
{
	return GetFiltersStatic();
}

QString CCodeEditor::GetNameStatic()
{
	return "Code Editor";
}

QStringList CCodeEditor::GetFiltersStatic()
{
	QStringList list;
	list.append("Lua files (*.lua)");
	return list;
}

QWidget* CCodeEditor::GetWidget()
{
	return Widget;
}

bool CCodeEditor::isModified()
{
	return Widget->SCI->isModified();
}

void CCodeEditor::ModificationChanged(bool AValue)
{
	UpdateLabel();
}

void CCodeEditor::InternalOpen(const QString &AFileName)
{
	CFile f(AFileName.toStdString(), CFile::OPEN_MODE_READ);
	Widget->SCI->setText(QString::fromStdString(f.GetContent()));
	f.Close();
	Widget->SCI->setModified(false);
}

void CCodeEditor::InternalSave(const QString &AFileName)
{
	CFile f(AFileName.toStdString(), CFile::OPEN_MODE_WRITE);
	f.SetContent(Widget->SCI->text().toStdString());
	f.Close();
	Widget->SCI->setModified(false);
}
