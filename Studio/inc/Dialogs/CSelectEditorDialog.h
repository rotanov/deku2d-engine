#ifndef CSELECTEDITORDIALOG_H
#define CSELECTEDITORDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui
{
	class CSelectEditorDialog;
}

class CSelectEditorDialog : public QDialog
{
	Q_OBJECT
public:
	explicit CSelectEditorDialog(QWidget *parent = 0);
	~CSelectEditorDialog();

	void FillEditors(const QStringList &AEditors);
	QString GetSelectedEditor();

	static QString Execute(const QStringList &AEditors);

protected:
	void focusInEvent(QFocusEvent *AEvent);
	bool eventFilter(QObject *AWatchedObject, QEvent *AEvent);

private:
	Ui::CSelectEditorDialog *ui;
	
private slots:
	void on_editorsList_doubleClicked(const QModelIndex &index);
};

#endif // CSELECTEDITORDIALOG_H
