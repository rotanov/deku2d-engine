#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QTabWidget>
#include <QPlainTextEdit>
#include <QDockWidget>

#include <QTreeWidget>
#include <QTableWidget>

#include <QWindowsStyle>

#include <QFileDialog>
#include <QMessageBox>

#include "CEditorTabWidget.h"
#include "CCodeEditorWidget.h"
#include "CEngineLuaConsole.h"
#include "CLogViewer.h"
#include "CEngineViewport.h"
#include "CComponentTreeWidget.h"
#include "CPropertyEditWidget.h"

#include "Editors/CCodeEditor.h"

#include "Dialogs/CFileSaveDialog.h"
#include "Dialogs/CSelectEditorDialog.h"

void CEngineThread::run()
{
	CEngine::Instance()->Run(0, NULL);
}

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent), ui(new Ui::CMainWindow)
{
	CEngine::Instance()->SetProgramName("Deku Studio");
	CEngine::Instance()->SetIdleWhenInBackground(false);

	CEditorRegistry::Instance()->RegisterEditor<CCodeEditor>();

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

	Tabs = new CEditorTabWidget;
	setCentralWidget(Tabs);

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

void CMainWindow::closeEvent(QCloseEvent *AEvent)
{
	if (Tabs->close())
		AEvent->accept();
	else
		AEvent->ignore();
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

// kind of hack to enable/disable menu items and still have access to shortcuts
// TODO: rewrite using idle timer, manual key processing or event-based enable/disable

void CMainWindow::on_menuFile_aboutToShow()
{
	CAbstractEditor *ed = Tabs->GetActiveEditor();
	bool maySave = ed && (ed->isModified() || ed->isNew());
	ui->actionSave->setEnabled(maySave);
	ui->actionSave_As->setEnabled(maySave);
}

void CMainWindow::on_menuFile_aboutToHide()
{
	ui->actionSave->setEnabled(true);
	ui->actionSave_As->setEnabled(true);
}

void CMainWindow::on_actionNew_triggered()
{
	QString edName = CSelectEditorDialog::Execute(CEditorRegistry::Instance()->GetAllEditors());
	if (edName.isEmpty())
		return;

	CAbstractEditor *ed = CEditorRegistry::Instance()->CreateEditorByName(edName);
	if (ed)
		Tabs->Add(ed);
}

void CMainWindow::on_actionOpen_triggered()
{
	QFileDialog dlg(this, "Open...");
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.setNameFilters(CEditorRegistry::Instance()->GetFilters());
	if (!dlg.exec())
		return;

	QString filter = dlg.selectedNameFilter();
	CAbstractEditor *ed = NULL;
	if (CEditorRegistry::Instance()->CountByFilter(filter) > 1)
	{
		QString edName = CSelectEditorDialog::Execute(
			CEditorRegistry::Instance()->GetEditorsForFilter(filter));
		if (edName.isEmpty())
			return;

		ed = CEditorRegistry::Instance()->CreateEditorByName(edName);
	}
	else
		ed = CEditorRegistry::Instance()->CreateEditorByFilter(filter);

	if (ed)
	{
		QString filePath = dlg.selectedFiles()[0];
		Tabs->Add(ed);
		ed->Open(filePath);
	}
}

void CMainWindow::on_actionSave_triggered()
{
	CAbstractEditor *ed = Tabs->GetActiveEditor();
	if (!ed)
		return;

	if (ed->isNew())
	{
		QString fn = CFileSaveDialog::Execute(ed->GetFilters());
		if (fn.isEmpty())
			return;

		ed->SaveAs(fn);
	}
	else
		ed->Save();
}

void CMainWindow::on_actionSave_As_triggered()
{
	CAbstractEditor *ed = Tabs->GetActiveEditor();
	if (!ed)
		return;

	QString fn = CFileSaveDialog::Execute(ed->GetFilters());
	if (fn.isEmpty())
		return;

	ed->SaveAs(fn);
}


void CMainWindow::on_menuEngine_aboutToShow()
{
	bool engineRunning = EngineThread.isRunning();
	ui->actionStart_engine->setDisabled(engineRunning);
	ui->actionStop_engine->setDisabled(!engineRunning);
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
