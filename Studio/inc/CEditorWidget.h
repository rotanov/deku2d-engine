#ifndef CEDITORWIDGET_H
#define CEDITORWIDGET_H

#include <QWidget>

class CAbstractEditor;

class CEditorWidget : public QWidget
{
	Q_OBJECT
public:
	explicit CEditorWidget(CAbstractEditor *AEditor);

	CAbstractEditor* GetEditor();
	
private:
	CAbstractEditor *Editor;
	
};

#endif // CEDITORWIDGET_H
