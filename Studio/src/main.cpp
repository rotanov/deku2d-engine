#include <QtGui/QApplication>
#include "cmainwindow.h"
#undef main
int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	CMainWindow w;
	w.show();
	return a.exec();
}
