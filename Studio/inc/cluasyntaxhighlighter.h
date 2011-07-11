#ifndef CLUASYNTAXHIGHLIGHTER_H
#define CLUASYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class QPlainTextEdit;

class CLuaSyntaxHighlighter : public QSyntaxHighlighter
{
public:
	CLuaSyntaxHighlighter(QTextDocument *AParent);
protected:
	void highlightBlock(const QString &AText);
	void colorize(const QString &AText, QTextCharFormat &ATokenFormat, QRegExp AExpression);
	void colorizeSingleLine(const QString &AText, QTextCharFormat &ATokenFormat, QRegExp AStartExpression, QRegExp AEndExpression);
};

#endif // CLUASYNTAXHIGHLIGHTER_H
