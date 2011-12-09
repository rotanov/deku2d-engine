#include "CCodeEditorWidget.h"

#include <QGridLayout>

#include "cluasyntaxhighlighter.h"

CCodeEditorWidget::CCodeEditorWidget(QWidget *parent) :
	QWidget(parent)
{
	TextEdit = new QPlainTextEdit;
	setLayout(new QGridLayout);
	layout()->addWidget(TextEdit);
	SyntaxHighlighter = new CLuaSyntaxHighlighter(TextEdit->document());
}
