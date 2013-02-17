#ifndef CCOMPONENTTREEWIDGET_H
#define CCOMPONENTTREEWIDGET_H

#include <QTreeWidget>

#include <2de_GameObject.h>

using Deku2D::CGameObject;

class CComponentTreeWidget : public QTreeWidget
{
Q_OBJECT
public:
	explicit CComponentTreeWidget(QWidget *parent = 0);

	void BuildTree(CGameObject *AGameObject, QTreeWidgetItem *AItem);

protected:
	void mousePressEvent(QMouseEvent *event);
	void dropEvent(QDropEvent *event);

private:
	CGameObject *DragSource;
};

#endif // CCOMPONENTTREEWIDGET_H
