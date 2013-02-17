#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include <2de_Engine.h>

#include "CEditorRegistry.h"

using namespace Deku2D;

namespace Ui
{
	class CMainWindow;
}

class CEngineThread : public QThread
{
public:
	void run();
};

class CLogViewer;
class CEngineLuaConsole;
class CEditorTabWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QTableWidget;
class QTableWidgetItem;
class CComponentTreeWidget;
class CPropertyEditWidget;

class CMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	CMainWindow(QWidget *parent = 0);
	~CMainWindow();

protected:
	void changeEvent(QEvent *e);
	void closeEvent(QCloseEvent *AEvent);

private:
	Ui::CMainWindow *ui;
	CEngineThread EngineThread;

	CEditorTabWidget *Tabs;
	CLogViewer *LogViewer;
	CEngineLuaConsole *EngineConsole;
	CComponentTreeWidget *ComponentTree;
	CPropertyEditWidget *PropertyEdit;

	void InitUI();

private slots:
	void ComponentTreeItemChanged(QTreeWidgetItem *ACurrent, QTreeWidgetItem *APrevious);

	void on_menuFile_aboutToShow();
	void on_menuFile_aboutToHide();
	void on_actionNew_triggered();
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionSave_As_triggered();

	void on_menuEngine_aboutToShow();
	void on_actionStart_engine_triggered();
	void on_actionStop_engine_triggered();
	void on_actionBuild_tree_triggered();
};

#endif // CMAINWINDOW_H
