#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include <2de_Engine.h>

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
class QTabWidget;
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

private:
	Ui::CMainWindow *ui;
	CEngineThread EngineThread;

	QTabWidget *Tabs;
	CLogViewer *LogViewer;
	CEngineLuaConsole *EngineConsole;
	CComponentTreeWidget *ComponentTree;
	CPropertyEditWidget *PropertyEdit;

	void InitUI();

private slots:
	void on_actionStart_engine_triggered();
	void on_actionStop_engine_triggered();
	void on_menuFile_aboutToShow();
	void on_actionBuild_tree_triggered();
	void ComponentTreeItemChanged(QTreeWidgetItem *ACurrent, QTreeWidgetItem *APrevious);
};

#endif // CMAINWINDOW_H
