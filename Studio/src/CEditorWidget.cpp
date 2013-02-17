#include "CEditorWidget.h"

CEditorWidget::CEditorWidget(CAbstractEditor *AEditor) : QWidget(0)
{
	Editor = AEditor;
}

CAbstractEditor* CEditorWidget::GetEditor()
{
	return Editor;
}
