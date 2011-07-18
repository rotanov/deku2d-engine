#ifndef CENGINELUACONSOLE_H
#define CENGINELUACONSOLE_H

#include <QWidget>
#include <QLineEdit>

#include "2de_Core.h"

class QPlainTextEdit;

class CConsoleInputLine : public QLineEdit
{
Q_OBJECT
public:
	CConsoleInputLine(QWidget *parent = 0);

protected:
	void keyPressEvent(QKeyEvent *event);

signals:
	void InputSent(const QString &AInput);

private slots:
	void UpdateHistory(const QString &AText);
	void SendInput();

private:
	void NewHistoryItem();
	QStringList History;
	QStringList::iterator CurrentHistoryItem;
};

class CEngineLuaConsole : public QWidget, public CObject
{
Q_OBJECT
public:
	explicit CEngineLuaConsole(QWidget *parent = 0);

signals:

public slots:

private slots:
	void SendInput(const QString &AInput);

private:
	void ProcessEvent(const CEvent &AEvent);
	QPlainTextEdit *Display;
	CConsoleInputLine *Input;
};

#endif // CENGINELUACONSOLE_H
