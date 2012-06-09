#ifndef CPROPERTYEDITWIDGET_H
#define CPROPERTYEDITWIDGET_H

#include <QWidget>

#include <2de_GameObject.h>

using namespace Deku2D;

class QTableWidget;
class QTableWidgetItem;

class CPropertyEditWidget : public QWidget
{
Q_OBJECT
public:
	explicit CPropertyEditWidget(QWidget *parent = 0);

	void SetObject(CGameObject *AObject);

private:
	void AddProperty(const string &AName, const string &AValue);

	QTableWidget *PropertyTable;
	CGameObject *Object;

private slots:
	void PropertyEditItemChanged(QTableWidgetItem *AItem);

};

#endif // CPROPERTYEDITWIDGET_H
