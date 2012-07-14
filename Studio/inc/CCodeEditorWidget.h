#ifndef CCODEEDITORWIDGET_H
#define CCODEEDITORWIDGET_H

#include "CEditorWidget.h"

class QsciScintilla;

class CCodeEditorWidget : public CEditorWidget
{
Q_OBJECT
public:
	explicit CCodeEditorWidget(CAbstractEditor *AEditor);

	QsciScintilla *SCI;

protected:
	void focusInEvent(QFocusEvent *AEvent);

};

#endif // CCODEEDITORWIDGET_H
