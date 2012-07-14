#include "CCodeEditorWidget.h"

#include <QGridLayout>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qscilexerlua.h>

CCodeEditorWidget::CCodeEditorWidget(CAbstractEditor *AEditor) : CEditorWidget(AEditor)
{
	SCI = new QsciScintilla;
	SCI->setIndentationsUseTabs(true);
	SCI->setIndentationGuides(true);
	SCI->setAutoIndent(true);
	SCI->setFolding(QsciScintilla::BoxedTreeFoldStyle);
	SCI->setMarginType(1, QsciScintilla::NumberMargin);
	SCI->setLexer(new QsciLexerLua);
	//SCI->setAnnotationDisplay(QsciScintilla::AnnotationBoxed);
	SCI->setAutoCompletionSource(QsciScintilla::AcsAll);
	SCI->setAutoCompletionThreshold(2);
	SCI->setBraceMatching(QsciScintilla::SloppyBraceMatch);

	//connect(SCI, SIGNAL(textChanged()), SCI, SLOT(autoCompleteFromAll()));
	//connect(SCI, SIGNAL(textChanged()), SCI, SLOT(callTip()));

	setLayout(new QGridLayout);
	layout()->addWidget(SCI);
}

void CCodeEditorWidget::focusInEvent(QFocusEvent *AEvent)
{
	QWidget::focusInEvent(AEvent);
	SCI->setFocus();
}
