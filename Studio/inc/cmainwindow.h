#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QThread>

#include <2de_Engine.h>

namespace Ui
{
	class CMainWindow;
}

class CEngineThread : public QThread
{
public:
	void run();
};

class CLuaSyntaxHighlighter;
class CLogViewer;
class CEngineLuaConsole;
class QTabWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QTableWidget;
class QTableWidgetItem;

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
	CLuaSyntaxHighlighter *LuaSyntaxHL;

	QTabWidget *Tabs;
	CLogViewer *LogViewer;
	CEngineLuaConsole *EngineConsole;
	QTreeWidget *ComponentTree;
	QTableWidget *PropertyEdit;

private slots:
	void on_actionStart_engine_triggered();
	void on_actionStop_engine_triggered();
	void on_menuFile_aboutToShow();
	void on_actionBuild_tree_triggered();
	void ComponentTreeItemChanged(QTreeWidgetItem *ACurrent, QTreeWidgetItem *APrevious);
	void PropertyEditItemChanged(QTableWidgetItem *AItem);
};

#endif // CMAINWINDOW_H
