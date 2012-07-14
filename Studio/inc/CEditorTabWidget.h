#ifndef CEDITORTABWIDGET_H
#define CEDITORTABWIDGET_H

#include <QTabWidget>

#include "Editors/CAbstractEditor.h"

class CEditorWidget;

class CEditorTabWidget : public QTabWidget
{
	Q_OBJECT
public:
	explicit CEditorTabWidget(QWidget *parent = 0);

	void Add(CAbstractEditor *AEditor);

	CAbstractEditor* GetActiveEditor();

protected:
	void closeEvent(QCloseEvent *AEvent);
	
private:
	CEditorWidget* widget(int index);
	bool RequestTabClose(int AIndex);
	void CloseTab(int AIndex);
	
private slots:
	void TabLabelChanged(CAbstractEditor *AEditor, const QString &ANewLabel, const QString &ANewToolTip);
	void CurrentTabChanged(int AIndex);
	bool TabCloseRequested(int AIndex);
	
};

#endif // CEDITORTABWIDGET_H
