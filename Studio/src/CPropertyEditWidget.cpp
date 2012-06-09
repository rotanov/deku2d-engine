#include "inc/CPropertyEditWidget.h"

#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QVBoxLayout>

#include <2de_Engine.h>

CPropertyEditWidget::CPropertyEditWidget(QWidget *parent) : QWidget(parent)
{
	Object = NULL;

	PropertyTable = new QTableWidget(0, 2);
	PropertyTable->setHorizontalHeaderLabels(
		QStringList() << "Property" << "Value");
	PropertyTable->verticalHeader()->setHidden(true);

	connect(
		PropertyTable,
		SIGNAL(itemChanged(QTableWidgetItem*)), this,
		SLOT(PropertyEditItemChanged(QTableWidgetItem*))
	);

	setLayout(new QVBoxLayout);
	layout()->addWidget(PropertyTable);
}

void CPropertyEditWidget::AddProperty(const string &AName, const string &AValue)
{
	int newRowIdx = PropertyTable->rowCount();
	PropertyTable->setRowCount(newRowIdx + 1);
	QTableWidgetItem *nameItem =
		new QTableWidgetItem(QString::fromStdString(AName));
	nameItem->setFlags(
		nameItem->flags() & !Qt::ItemIsEditable |
		Qt::ItemIsSelectable | Qt::ItemIsEnabled
	);
	PropertyTable->setItem(newRowIdx, 0, nameItem);
	PropertyTable->setItem(newRowIdx, 1, new QTableWidgetItem(
		QString::fromStdString(AValue)));
}

void CPropertyEditWidget::SetObject(CGameObject *AObject)
{
	Object = AObject;

	PropertyTable->setRowCount(0);

	if (Object)
	{
		AddProperty("Name", AObject->GetName());
		AddProperty("Class", AObject->GetClassName());
		AddProperty("Is Prototype",
			AObject->IsPrototype() ? "true" : "false");
	}
}

void CPropertyEditWidget::PropertyEditItemChanged(QTableWidgetItem *AItem)
{
	if (AItem->column() < 1 || !Object)
		return;

	QString property = PropertyTable->item(AItem->row(), 0)->text();
	CEngine::Instance()->Lock();
	if (property == "Name")
	{
		string newName = AItem->text().toStdString();
		if (Object->GetName() != newName)
			Object->SetName(newName);
		// TODO: send some kind of notification that
		//       object's name changed
	}
	CEngine::Instance()->Unlock();
}
