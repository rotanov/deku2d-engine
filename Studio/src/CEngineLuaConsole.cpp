#include "CEngineLuaConsole.h"

#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QScrollBar>

#include "2de_Engine.h"

CConsoleInputLine::CConsoleInputLine(QWidget *parent) : QLineEdit(parent)
{
	connect(this, SIGNAL(textChanged(QString)), SLOT(UpdateHistory(QString)));
	connect(this, SIGNAL(returnPressed()), SLOT(SendInput()));
	NewHistoryItem();
}

void CConsoleInputLine::keyPressEvent(QKeyEvent *event)
{
	int k = event->key();

	if (k == Qt::Key_Up)
	{
		if (CurrentHistoryItem != History.begin())
		{
			--CurrentHistoryItem;
			setText(*CurrentHistoryItem);
		}
	}
	else if (k == Qt::Key_Down)
	{
		if (CurrentHistoryItem != --History.end())
		{
			++CurrentHistoryItem;
			setText(*CurrentHistoryItem);
		}
	}

	QLineEdit::keyPressEvent(event);
}

void CConsoleInputLine::UpdateHistory(const QString &AText)
{
	if (CurrentHistoryItem == --History.end())
		*CurrentHistoryItem = AText;
}

void CConsoleInputLine::SendInput()
{
	if (CurrentHistoryItem != --History.end())
		*(--History.end()) = text();

	NewHistoryItem();
	emit InputSent(text());
	clear();
}

void CConsoleInputLine::NewHistoryItem()
{
	History.append("");
	CurrentHistoryItem = --History.end();
}

CEngineLuaConsole::CEngineLuaConsole(QWidget *parent) : QWidget(parent)
{
	Display = new QPlainTextEdit;
	Display->setWordWrapMode(QTextOption::WrapAnywhere);
	Display->setReadOnly(true);

	Input = new CConsoleInputLine;

	setLayout(new QVBoxLayout);
	layout()->addWidget(Display);
	layout()->addWidget(Input);

	connect(Input, SIGNAL(InputSent(QString)), SLOT(SendInput(QString)));
}

void CEngineLuaConsole::SendInput(const QString &AInput)
{
	Display->appendHtml("<b>&gt;</b> " + Qt::escape(AInput));

	if (CEngine::Instance()->isRunning())
	{
		SDL_mutex *mutex = CEngine::Instance()->BigEngineLock;
		SDL_mutexP(mutex);
		CLuaVirtualMachine::Instance()->RunString(AInput.toStdString());
		SDL_mutexV(mutex);
	}
	else
	{
		Display->appendHtml("<font color=red>Engine is not running.</font>");
	}
	Display->verticalScrollBar()->setValue(Display->verticalScrollBar()->maximum());
}

void CEngineLuaConsole::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() != "ConsoleOutput")
		return;

	QString text = QString::fromStdString(AEvent.GetData<string>("Text"));

	if (AEvent.IsDataExists("Type") && AEvent.GetData<string>("Type") == "Error")
		Display->appendHtml("<font color=red>" + Qt::escape(text) + "</font>");
	else
		Display->appendPlainText(text);
}
