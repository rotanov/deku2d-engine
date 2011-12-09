#include "CEngineViewport.h"

#include <QPainter>
#include <QPixmap>

CEngineViewport::CEngineViewport(QWidget *parent) :
    QWidget(parent)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
}

void CEngineViewport::paintEvent(QPaintEvent *e)
{
	SDL_LockSurface(Surface);
	QPixmap pxm;
	pxm.loadFromData((uchar *)Surface->pixels, Surface->w * Surface->h * Surface->format->BytesPerPixel);

//	QImage im(static_cast<uchar*>(Surface->pixels), Surface->w, Surface->h, QImage::Format_RGB32);
	QPainter p(this);
//	p.drawImage(0, 0, im);
	p.drawPixmap(0,0, 320, 240, pxm);
	SDL_UnlockSurface(Surface);
}

void CEngineViewport::showEvent(QShowEvent *e)
{
	QWidget::showEvent(e);
	CEngine::Instance()->Lock();
	//Surface = GLWindow->GetSurface();
	CEngine::Instance()->Unlock();
}
