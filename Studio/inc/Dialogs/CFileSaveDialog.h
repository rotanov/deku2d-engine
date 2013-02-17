#ifndef CFILESAVEDIALOG_H
#define CFILESAVEDIALOG_H

#include <QFileDialog>

class CFileSaveDialog : public QFileDialog
{
	Q_OBJECT
public:
	explicit CFileSaveDialog();

	static QString Execute(const QStringList &AFilters);
};

#endif // CFILESAVEDIALOG_H
