#include "cmainwindow.h"
#include "ui_cmainwindow.h"

#include <QTabWidget>
#include <QPlainTextEdit>
#include <QDockWidget>
#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>

#include <QTreeWidget>
#include <QTableWidget>

#include <QWindowsStyle>

#include "cluasyntaxhighlighter.h"
#include "CCodeEditorWidget.h"
#include "CEngineLuaConsole.h"
#include "CLogViewer.h"
#include "CEngineViewport.h"

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


	Tabs = new QTabWidget;
	setCentralWidget(Tabs);
	Tabs->setTabsClosable(true);
	Tabs->setMovable(true);
	Tabs->setContextMenuPolicy(Qt::DefaultContextMenu);
	//Tabs->addTab(new CCodeEditorWidget, QIcon(), "Code Editor");

	QsciScintilla *sci = new QsciScintilla;
	sci->setIndentationsUseTabs(true);
	sci->setIndentationGuides(true);
	sci->setAutoIndent(true);
	sci->setFolding(QsciScintilla::BoxedTreeFoldStyle);
	sci->setMarginType(1, QsciScintilla::NumberMargin);
	sci->setLexer(new QsciLexerLua);
	//sci->setAnnotationDisplay(QsciScintilla::AnnotationBoxed);
	sci->setAutoCompletionSource(QsciScintilla::AcsAll);
	sci->setAutoCompletionThreshold(2);
	sci->setBraceMatching(QsciScintilla::SloppyBraceMatch);
	//connect(sci, SIGNAL(textChanged()), sci, SLOT(autoCompleteFromAll()));
	//connect(sci, SIGNAL(textChanged()), sci, SLOT(callTip()));
	Tabs->addTab(sci, QIcon(), "Code Editor");

	CEngineViewport *port = new CEngineViewport(this);
	Tabs->addTab(port, QIcon(), "Engine Viewport");


	QDockWidget *EngineConsoleDock = new QDockWidget("Engine Console");
	EngineConsole = new CEngineLuaConsole;
	EngineConsoleDock->setWidget(EngineConsole);
	addDockWidget(Qt::BottomDockWidgetArea, EngineConsoleDock);

	QDockWidget *LogViewerDock = new QDockWidget("Log Viewer");
	LogViewer = new CLogViewer;
	LogViewerDock->setWidget(LogViewer);
	addDockWidget(Qt::BottomDockWidgetArea, LogViewerDock);

	QDockWidget *ComponentTreeDock = new QDockWidget("Component Tree");
	ComponentTree = new QTreeWidget;
	connect(ComponentTree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
		this, SLOT(ComponentTreeItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
	ComponentTree->setColumnCount(1);
	//ComponentTree->setHeaderLabel("");
	ComponentTree->setHeaderHidden(true);
	ComponentTree->setStyle(new QWindowsStyle);
	ComponentTreeDock->setWidget(ComponentTree);
	addDockWidget(Qt::LeftDockWidgetArea, ComponentTreeDock);

	PropertyEdit = new QTableWidget(0, 2);
	PropertyEdit->setHorizontalHeaderLabels(QStringList() << "Property" << "Value");
	PropertyEdit->verticalHeader()->setHidden(true);
	QDockWidget *PropertyEditDock = new QDockWidget("Property Edit");
	PropertyEditDock->setWidget(PropertyEdit);
	addDockWidget(Qt::RightDockWidgetArea, PropertyEditDock);
	connect(PropertyEdit, SIGNAL(itemChanged(QTableWidgetItem*)), this,
		SLOT(PropertyEditItemChanged(QTableWidgetItem*)));

	ui->mainToolBar->setWindowTitle("Toolbar");

	QMenu *toolWindowsMenu = createPopupMenu();
	toolWindowsMenu->setTitle("Tool windows");
	ui->menuView->addMenu(toolWindowsMenu);

	//LuaSyntaxHL = new CLuaSyntaxHighlighter(ui->scriptText->document());
}

CMainWindow::~CMainWindow()
{
	delete ui;
	//delete LuaSyntaxHL;
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

void BuildTree(CGameObject *go, QTreeWidgetItem *item)
{
	QTreeWidgetItem *cur = new QTreeWidgetItem((QTreeWidget *)NULL, QStringList(QString::fromStdString(go->GetName())));
	cur->setToolTip(0, QString::fromStdString(go->GetClassName()));
	if (go->IsPrototype())
	{
		QFont f(cur->font(1));
		f.setWeight(QFont::Bold);
		cur->setFont(0, f);
	}
	cur->setData(0, Qt::UserRole, QVariant::fromValue((void *)go));
	item->addChild(cur);

	for (int i = 0; i < go->GetChildCount(); ++i)
	{
		BuildTree(go->GetChild(i), cur);
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
		BuildTree(CEngine::Instance()->RootGameObject, ComponentTree->invisibleRootItem());
		CEngine::Instance()->Unlock();
	}
}

void CMainWindow::ComponentTreeItemChanged(QTreeWidgetItem *ACurrent, QTreeWidgetItem *APrevious)
{
	if (!ACurrent)
		return;

	CGameObject *go = NULL;
	go = (CGameObject *) ACurrent->data(0, Qt::UserRole).value<void *>();

	PropertyEdit->setRowCount(3);
	PropertyEdit->setItem(0, 0, new QTableWidgetItem("Name"));
	PropertyEdit->setItem(0, 1, new QTableWidgetItem(QString::fromStdString(go->GetName())));
	PropertyEdit->setItem(1, 0, new QTableWidgetItem("Class"));
	PropertyEdit->setItem(1, 1, new QTableWidgetItem(QString::fromStdString(go->GetClassName())));
	PropertyEdit->setItem(2, 0, new QTableWidgetItem("Is Prototype"));
	PropertyEdit->setItem(2, 1, new QTableWidgetItem(go->IsPrototype() ? "true" : "false"));

}

#include <QMessageBox>

void CMainWindow::PropertyEditItemChanged(QTableWidgetItem *AItem)
{
	if (AItem->column() < 1)
		return;

	QString property = PropertyEdit->item(AItem->row(), 0)->text();
	CEngine::Instance()->Lock();
	if (property == "Name" && ComponentTree->currentItem())
	{
//		QMessageBox::information(this, "0", "Current: " + ComponentTree->currentItem()->text(0));
		string oldName = ComponentTree->currentItem()->text(0).toStdString();
		string newName = AItem->text().toStdString();
		if (oldName != newName)
		{
			Factory->Rename(oldName, newName);
			ComponentTree->currentItem()->setText(0, QString::fromStdString(newName));
		}
	}
	CEngine::Instance()->Unlock();
}
