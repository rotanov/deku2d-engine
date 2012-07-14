#include "Dialogs/CFileSaveDialog.h"

CFileSaveDialog::CFileSaveDialog() :
	QFileDialog(0, "Save As...")
{
	setFileMode(QFileDialog::AnyFile);
	setLabelText(QFileDialog::Accept, "Save");

	// shit, Qt doesn't ask for confirmation...
	// TODO: workaround somehow, probably write own confirmation dialog
	setConfirmOverwrite(true);
	setOption(QFileDialog::DontConfirmOverwrite, false);
}

QString CFileSaveDialog::Execute(const QStringList &AFilters)
{
	CFileSaveDialog dlg;
	dlg.setNameFilters(AFilters);
	if (!dlg.exec())
		return "";

	return dlg.selectedFiles()[0];
}
