#ifndef CLOGVIEWER_H
#define CLOGVIEWER_H

#include <QWidget>

#include <QSyntaxHighlighter>

#include <2de_Engine.h>

using namespace Deku2D;

class QPlainTextEdit;

class CLogSyntaxHighlighter : public QSyntaxHighlighter
{
public:
	CLogSyntaxHighlighter(QTextDocument *AParent);
protected:
	void highlightBlock(const QString &AText);
	void colorize(const QString &AText, QTextCharFormat &ATokenFormat, QRegExp AExpression);
};

class CLogViewer : public QWidget, public CObject
{
Q_OBJECT
public:
	explicit CLogViewer(QWidget *parent = 0);

signals:

public slots:

private:
	void ProcessEvent(const CEvent &AEvent);
	QPlainTextEdit *Display;

};

#endif // CLOGVIEWER_H
