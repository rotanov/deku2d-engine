#include "Editors/CAbstractEditor.h"

#include <cassert>

#include <QFileInfo>

CAbstractEditor::CAbstractEditor()
{
	New = true;
	FileName = "Untitled";
	UpdateLabel();
}

CAbstractEditor::~CAbstractEditor()
{
}

QString CAbstractEditor::GetFileName()
{
	QFileInfo fi(FileName);
	return fi.fileName();
}

void CAbstractEditor::Open(const QString &AFileName)
{
	InternalOpen(AFileName);

	New = false;

	FileName = AFileName;
	UpdateLabel();
}

void CAbstractEditor::Save()
{
	assert(!New);

	InternalSave(FileName);
}

void CAbstractEditor::SaveAs(const QString &AFileName)
{
	InternalSave(AFileName);

	New = false;

	FileName = AFileName;
	UpdateLabel();
}

bool CAbstractEditor::isNew()
{
	return New;
}

void CAbstractEditor::UpdateLabel()
{
	emit LabelChanged(this, GetFileName(), FileName);
}
