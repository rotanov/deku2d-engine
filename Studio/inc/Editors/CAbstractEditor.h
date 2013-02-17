#ifndef CABSTRACTEDITOR_H
#define CABSTRACTEDITOR_H

#include <QObject>
#include <QString>
#include <QStringList>

class QWidget;

class CAbstractEditor : public QObject
{
	Q_OBJECT
public:
	explicit CAbstractEditor();
	virtual ~CAbstractEditor();

	virtual QString GetName() = 0;
	virtual QStringList GetFilters() = 0;

	virtual QWidget* GetWidget() = 0;

	QString GetFileName();

	void Open(const QString &AFileName);
	void Save();
	void SaveAs(const QString &AFileName);

	virtual bool isModified() = 0;
	bool isNew();

protected:
	virtual void InternalOpen(const QString &AFileName) = 0;
	virtual void InternalSave(const QString &AFileName) = 0;

	void UpdateLabel();

	QString FileName;

private:
	bool New;

signals:
	void LabelChanged(CAbstractEditor *ASender, const QString &ANewLabel, const QString &ANewToolTip);
};

#endif // CABSTRACTEDITOR_H
