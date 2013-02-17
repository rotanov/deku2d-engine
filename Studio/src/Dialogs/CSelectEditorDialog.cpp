#include "Dialogs/CSelectEditorDialog.h"
#include "ui_CSelectEditorDialog.h"

#include <QList>
#include <QKeyEvent>

CSelectEditorDialog::CSelectEditorDialog(QWidget *parent) :
	QDialog(parent), ui(new Ui::CSelectEditorDialog)
{
	ui->setupUi(this);
	ui->editorsList->installEventFilter(this);
}

CSelectEditorDialog::~CSelectEditorDialog()
{
	delete ui;
}

void CSelectEditorDialog::FillEditors(const QStringList &AEditors)
{
	for (QStringList::const_iterator it = AEditors.begin(); it != AEditors.end(); ++it)
		ui->editorsList->addItem(*it);

	ui->editorsList->setCurrentRow(0);
}

QString CSelectEditorDialog::GetSelectedEditor()
{
	const QList<QListWidgetItem *> &sel = ui->editorsList->selectedItems();
	if (sel.count() < 1)
		return "";

	return sel[0]->text();
}

QString CSelectEditorDialog::Execute(const QStringList &AEditors)
{
	CSelectEditorDialog dlg;
	dlg.FillEditors(AEditors);

	if (!dlg.exec())
		return "";

	return dlg.GetSelectedEditor();
}

void CSelectEditorDialog::focusInEvent(QFocusEvent *AEvent)
{
	QDialog::focusInEvent(AEvent);
	ui->editorsList->setFocus();
}

bool CSelectEditorDialog::eventFilter(QObject *AWatchedObject, QEvent *AEvent)
{
	QDialog::eventFilter(AWatchedObject, AEvent);

	if (AWatchedObject != ui->editorsList)
		return false;

	if (AEvent->type() == QEvent::KeyPress)
	{
		QKeyEvent *e = (QKeyEvent *) AEvent;
		if (e->key() == Qt::Key_Return)
		{
			if (ui->editorsList->selectedItems().count() > 0)
				accept();
			return true;
		}
	}


	return false;
}

void CSelectEditorDialog::on_editorsList_doubleClicked(const QModelIndex &index)
{
	accept();
}
