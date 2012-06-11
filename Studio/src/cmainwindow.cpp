#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QTabWidget>
#include <QPlainTextEdit>
#include <QDockWidget>

#include <QTreeWidget>
#include <QTableWidget>

#include <QWindowsStyle>

#include "CCodeEditorWidget.h"
#include "CEngineLuaConsole.h"
#include "CLogViewer.h"
#include "CEngineViewport.h"
#include "CComponentTreeWidget.h"
#include "CPropertyEditWidget.h"

void CEngineThread::run()
{
	CEngine::Instance()->Run(0, NULL);
}

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::CMainWindow)
{
	CEngine::Instance()->SetProgramName("Deku Studio");
	CEngine::Instance()->SetIdleWhenInBackground(false);

	InitUI();
}

CMainWindow::~CMainWindow()
{
	delete ui;
	if (CEngine::Instance()->isRunning())
	{
		CEngine::Instance()->Lock();
		CEngine::Instance()->ShutDown();
		CEngine::Instance()->Unlock();
		EngineThread.wait();
	}
}

void CMainWindow::InitUI()
{
	ui->setupUi(this);
	connect(ui->actionExit, SIGNAL(triggered()), qApp, SLOT(quit()));

	Tabs = new QTabWidget;
	setCentralWidget(Tabs);
	Tabs->setTabsClosable(true);
	Tabs->setMovable(true);
	Tabs->setContextMenuPolicy(Qt::DefaultContextMenu);

	Tabs->addTab(new CCodeEditorWidget, QIcon(), "Code Editor");

	//CEngineViewport *port = new CEngineViewport(this);
	//Tabs->addTab(port, QIcon(), "Engine Viewport");

	EngineConsole = new CEngineLuaConsole;
	LogViewer = new CLogViewer;
	ComponentTree = new CComponentTreeWidget;
	PropertyEdit = new CPropertyEditWidget;

	QDockWidget *engineConsoleDock = new QDockWidget("Engine Console");
	engineConsoleDock->setWidget(EngineConsole);
	addDockWidget(Qt::BottomDockWidgetArea, engineConsoleDock);

	QDockWidget *logViewerDock = new QDockWidget("Log Viewer");
	logViewerDock->setWidget(LogViewer);
	addDockWidget(Qt::BottomDockWidgetArea, logViewerDock);

	QDockWidget *componentTreeDock = new QDockWidget("Component Tree");
	componentTreeDock->setWidget(ComponentTree);
	addDockWidget(Qt::LeftDockWidgetArea, componentTreeDock);

	QDockWidget *propertyEditDock = new QDockWidget("Property Edit");
	propertyEditDock->setWidget(PropertyEdit);
	addDockWidget(Qt::RightDockWidgetArea, propertyEditDock);


	connect(
		ComponentTree,
		SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		this,
		SLOT(ComponentTreeItemChanged(QTreeWidgetItem*,
			QTreeWidgetItem*))
	);

	ui->mainToolBar->setWindowTitle("Toolbar");

	QMenu *toolWindowsMenu = createPopupMenu();
	toolWindowsMenu->setTitle("Tool windows");
	ui->menuView->addMenu(toolWindowsMenu);
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
	{
		EngineThread.start();
		CEngine::Instance()->Lock();
		EventManager->Subscribe("LogOutput", LogViewer);
		EventManager->Subscribe("ConsoleOutput", EngineConsole);
		CEngine::Instance()->Unlock();
	}
}

void CMainWindow::on_actionStop_engine_triggered()
{
	CEngine::Instance()->Lock();
	CEngine::Instance()->ShutDown();
	EventManager->Unsubscribe("LogOutput", LogViewer);
	EventManager->Unsubscribe("ConsoleOutput", EngineConsole);
	CEngine::Instance()->Unlock();

	ComponentTree->clear();
}

void CMainWindow::on_menuFile_aboutToShow()
{
	bool engineRunning = EngineThread.isRunning();
	ui->actionStart_engine->setDisabled(engineRunning);
	ui->actionStop_engine->setDisabled(!engineRunning);
}

void CMainWindow::on_actionBuild_tree_triggered()
{
	if (CEngine::Instance()->isRunning())
	{
		ComponentTree->clear();
		CEngine::Instance()->Lock();
		ComponentTree->BuildTree(CEngine::Instance()->RootGameObject,
			ComponentTree->invisibleRootItem());
		CEngine::Instance()->Unlock();
	}
}

void CMainWindow::ComponentTreeItemChanged(QTreeWidgetItem *ACurrent,
	QTreeWidgetItem *APrevious)
{
	// TODO: rewrite me
	if (!ACurrent)
		return;

	CGameObject *go = NULL;
	go = (CGameObject *) ACurrent->data(0, Qt::UserRole).value<void *>();

	PropertyEdit->SetObject(go);
}
