#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include "cluasyntaxhighlighter.h"

void CEngineThread::run()
{
	CEngine::Instance()->Run(0, NULL);
}

CMainWindow::CMainWindow(QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::CMainWindow)
{
	CEngine::Instance()->SetProgramName("Deku Studio");
	CEngine::Instance()->SetIdleWhenInBackground(false);
	ui->setupUi(this);
	connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));

	LuaSyntaxHL = new CLuaSyntaxHighlighter(ui->scriptText->document());
}

CMainWindow::~CMainWindow()
{
	delete ui;
	delete LuaSyntaxHL;
	CEngine::Instance()->ShutDown();
	EngineThread.wait();
}

void CMainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type())
	{
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

void CMainWindow::on_menuFile_aboutToShow()
{
	bool engineRunning = EngineThread.isRunning();
	ui->actionStart_engine->setDisabled(engineRunning);
	ui->actionStop_engine->setDisabled(!engineRunning);
}

void CMainWindow::on_executeButton_clicked()
{
	if (EngineThread.isRunning())
	{
		SDL_mutex *mutex = CEngine::Instance()->BigEngineLock;
		SDL_mutexP(mutex);
		CLuaVirtualMachine::Instance()->RunString(ui->scriptText->toPlainText().toStdString());
		SDL_mutexV(mutex);
	}
}

void CMainWindow::on_clearButton_clicked()
{
	ui->scriptText->clear();
}
