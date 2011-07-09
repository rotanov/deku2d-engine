#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <2de_Engine.h>

void CEngineThread::run()
{
	CEngine::Instance()->Run(0, NULL);
}

CMainWindow::CMainWindow(QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::CMainWindow)
{
	CEngine::Instance()->SetProgramName("Deku Studio");
	ui->setupUi(this);
}

CMainWindow::~CMainWindow()
{
	delete ui;
	CEngine::Instance()->ShutDown();
	EngineThread.wait();
}

void CMainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void CMainWindow::on_actionStart_engine_triggered()
{
	if (!EngineThread.isRunning())
		EngineThread.start();
}

void CMainWindow::on_actionStop_engine_triggered()
{
	CEngine::Instance()->ShutDown();
}
