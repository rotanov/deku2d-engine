#include "CLogViewer.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QTextCodec>
#include <QScrollBar>

CLogSyntaxHighlighter::CLogSyntaxHighlighter(QTextDocument *AParent) : QSyntaxHighlighter(AParent)
{
}

void CLogSyntaxHighlighter::highlightBlock(const QString &AText)
{
	if (AText.isEmpty())
		return;

	{
		QTextCharFormat fmt;
		fmt.setBackground(QColor::fromRgb(255, 220, 220));

		QRegExp re("^.*\\[ERROR\\].*$");
		colorize(AText, fmt, re);
	}
	{
		QTextCharFormat fmt;
		fmt.setForeground(QColor::fromRgb(50, 100, 220));
		fmt.setFontItalic(true);

		QRegExp re("^\\[[^\\]]*\\] \\[[^\\]]*\\] ");
		colorize(AText, fmt, re);
	}
}

void CLogSyntaxHighlighter::colorize(const QString &AText, QTextCharFormat &ATokenFormat, QRegExp AExpression)
{
	int startIndex = 0;
	startIndex = AText.indexOf(AExpression);

	while (startIndex >= 0)
	{
		int tokenLength = AExpression.matchedLength();
		setFormat(startIndex, tokenLength, ATokenFormat);
		startIndex = AText.indexOf(AExpression, startIndex + tokenLength);
	}
}

CLogViewer::CLogViewer(QWidget *parent) : QWidget(parent)
{
	Display = new QPlainTextEdit;
	CLogSyntaxHighlighter *hl = new CLogSyntaxHighlighter(Display->document());
	Display->setReadOnly(true);
	Display->setMaximumBlockCount(100);
	Display->setCenterOnScroll(true);

	setLayout(new QVBoxLayout);
	layout()->addWidget(Display);

	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
}

void CLogViewer::ProcessEvent(const CEvent &AEvent)
{
	if (AEvent.GetName() != "LogOutput")
		return;
	string text = AEvent.GetData<string>("Text");

//	bool scrollNeeded = Display->verticalScrollBar()->value() == Display->verticalScrollBar()->maximum();
	//Display->appendPlainText(QString::fromStdString(text));
	Display->appendPlainText(QString::fromStdString(text));
	//Display->centerCursor();
//	if (scrollNeeded)
//		Display->verticalScrollBar()->setValue(Display->verticalScrollBar()->maximum());
//	Display->update();
}
