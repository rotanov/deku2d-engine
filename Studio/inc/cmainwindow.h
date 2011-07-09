#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QThread>

namespace Ui
{
	class CMainWindow;
}

class CEngineThread : public QThread
{
public:
	void run();
};

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

private slots:
	void on_actionStart_engine_triggered();
	void on_actionStop_engine_triggered();
};

#endif // CMAINWINDOW_H
