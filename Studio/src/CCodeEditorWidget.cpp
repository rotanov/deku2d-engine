#include "CCodeEditorWidget.h"

#include <QGridLayout>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>

CCodeEditorWidget::CCodeEditorWidget(QWidget *parent) : QWidget(parent)
{
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

	setLayout(new QGridLayout);
	layout()->addWidget(sci);
}
