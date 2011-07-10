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

private slots:
	void on_actionStart_engine_triggered();
	void on_actionStop_engine_triggered();
	void on_menuFile_aboutToShow();
	void on_executeButton_clicked();
	void on_clearButton_clicked();
};

#endif // CMAINWINDOW_H
