#ifndef CENGINEVIEWPORT_H
#define CENGINEVIEWPORT_H

#include <QWidget>

#include <2de_Engine.h>

using namespace Deku2D;

class CEngineViewport : public QWidget
{
Q_OBJECT
public:
	explicit CEngineViewport(QWidget *parent = 0);

signals:

public slots:

protected:
	virtual void paintEvent(QPaintEvent *e);
	virtual void showEvent(QShowEvent *e);

private:
	SDL_Surface *Surface;
//	QPainter Painter;

};

#endif // CENGINEVIEWPORT_H
