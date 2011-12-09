#ifndef CCODEEDITORWIDGET_H
#define CCODEEDITORWIDGET_H

#include <QWidget>

#include <QPlainTextEdit>

class QSyntaxHighlighter;

class CCodeEditorWidget : public QWidget
{
Q_OBJECT
public:
	explicit CCodeEditorWidget(QWidget *parent = 0);

signals:

public slots:

private:
	QPlainTextEdit *TextEdit;
	QSyntaxHighlighter *SyntaxHighlighter;

};

#endif // CCODEEDITORWIDGET_H
