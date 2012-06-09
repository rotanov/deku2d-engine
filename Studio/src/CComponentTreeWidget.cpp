#include "CComponentTreeWidget.h"

#include <2de_GameObject.h>
#include <2de_Engine.h>

#include <QDropEvent>
#include <QWindowsStyle>

using Deku2D::CGameObject;
using Deku2D::CEngine;

CComponentTreeWidget::CComponentTreeWidget(QWidget *parent) :
	QTreeWidget(parent)
{
	setAcceptDrops(true);

	setColumnCount(1);
	setHeaderHidden(true);
	setStyle(new QWindowsStyle);
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::InternalMove);
}

void CComponentTreeWidget::BuildTree(CGameObject *AGameObject,
	QTreeWidgetItem *AItem)
{
	QTreeWidgetItem *cur = new QTreeWidgetItem(
		(QTreeWidget *)NULL,
		QStringList(QString::fromStdString(AGameObject->GetName()))
	);
	cur->setToolTip(0, QString::fromStdString(AGameObject->GetClassName()));
	if (AGameObject->IsPrototype())
	{
		QFont f(cur->font(1));
		f.setWeight(QFont::Bold);
		cur->setFont(0, f);
	}
	cur->setData(0, Qt::UserRole, QVariant::fromValue((void *)AGameObject));
	AItem->addChild(cur);

	for (unsigned i = 0; i < AGameObject->GetChildCount(); ++i)
	{
		BuildTree(AGameObject->GetChild(i), cur);
	}
}

void CComponentTreeWidget::mousePressEvent(QMouseEvent *event)
{
	QTreeWidget::mousePressEvent(event);

	if (selectedItems().isEmpty())
		return;

	DragSource = (CGameObject *)
		selectedItems().first()->data(0, Qt::UserRole).value<void *>();
}

void CComponentTreeWidget::dropEvent(QDropEvent *event)
{
	QTreeWidget::dropEvent(event);

	CGameObject *target = (CGameObject *)
		itemAt(event->pos().x(), event->pos().y())->
			data(0, Qt::UserRole).value<void *>();

	CEngine::Instance()->Lock();
	target->Attach(DragSource);
	CEngine::Instance()->Unlock();
}

