#include "cluasyntaxhighlighter.h"

CLuaSyntaxHighlighter::CLuaSyntaxHighlighter(QTextDocument *AParent) : QSyntaxHighlighter(AParent)
{
}

// almost verbatim taken from:
// http://trac.ta3d.org/browser/trunk/src/3DMEditor/src/luaeditor.cpp
// that is some GPLv2 project
// will be rewritten later, anyway

void CLuaSyntaxHighlighter::highlightBlock(const QString &AText)
{
	if (AText.isEmpty())
		return;
	// type qualifiers
	{
		QTextCharFormat tokenFormat;
		tokenFormat.setForeground(QColor::fromRgb(0x7F,0x0,0x0));

		QRegExp token("\\blocal\\b");
		colorize(AText, tokenFormat, token);
	}
	// other keywords
	{
		QTextCharFormat tokenFormat;
		QPen pen;
		pen.setWidthF(1.0);
		pen.setColor(QColor::fromRgb(0x7F,0x7F,0x0));
		tokenFormat.setTextOutline(pen);

		QRegExp token("\\bdo\\b"
			      "|\\bend\\b"
			      "|\\brepeat\\b"
			      "|\\buntil\\b"
			      "|\\bif\\b"
			      "|\\bthen\\b"
			      "|\\belse\\b"
			      "|\\belseif\\b"
			      "|\\bfor\\b"
			      "|\\bwhile\\b"
			      "|\\bfunction\\b"
			      "|\\breturn\\b"
			      "|\\bbreak\\b"
			      "|\\band\\b"
			      "|\\bor\\b"
			      "|\\bnot\\b"
			      "|\\bin\\b");
		colorize(AText, tokenFormat, token);
	}
	// Values
	{
		QTextCharFormat tokenFormat;
		QPen pen;
		pen.setWidthF(1.0);
		pen.setColor(QColor::fromRgb(0x0,0x0,0x7F));
		tokenFormat.setTextOutline(pen);

		QRegExp token("\\btrue\\b"
			      "|\\bfalse\\b"
			      "|\\bnil\\b"
			      "|\\b\\d+\\b"
			      "|\\b0x\\d+\\b"
			      "|\\b0X\\d+\\b");
		colorize(AText, tokenFormat, token);
	}
	// Strings
	{
		QTextCharFormat tokenFormat;
		tokenFormat.setForeground(QColor::fromRgb(0,0x7F,0));

		colorizeSingleLine(AText, tokenFormat, QRegExp("\""), QRegExp("[^\\\\]\""));
		// doesn't work well, needs actual parsing...
		colorizeSingleLine(AText, tokenFormat, QRegExp("'"), QRegExp("[^\\\\]'"));
	}
	// Comments
	{
		QTextCharFormat tokenFormat;
		QPen pen;
		pen.setWidthF(1.0);
		pen.setColor(QColor::fromRgb(0x7F,0x7F,0x7F));
		tokenFormat.setTextOutline(pen);

		colorizeSingleLine(AText, tokenFormat, QRegExp("--"), QRegExp("\\n"));
	}

}

void CLuaSyntaxHighlighter::colorize(const QString &AText, QTextCharFormat &ATokenFormat, QRegExp AExpression)
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

void CLuaSyntaxHighlighter::colorizeSingleLine(const QString &AText, QTextCharFormat &ATokenFormat, QRegExp AStartExpression, QRegExp AEndExpression)
{
	int startIndex = 0;
	startIndex = AText.indexOf(AStartExpression);

	while (startIndex >= 0)
	{
		int endIndex = AText.indexOf(AEndExpression, startIndex);
		int commentLength;
		if (endIndex == -1)
		{
			commentLength = AText.length() - startIndex;
		}
		else
		{
			commentLength = endIndex - startIndex
					+ AEndExpression.matchedLength();
		}
		setFormat(startIndex, commentLength, ATokenFormat);
		startIndex = AText.indexOf(AStartExpression,
					  startIndex + commentLength);
	}
}
