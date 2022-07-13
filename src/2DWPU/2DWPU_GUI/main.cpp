#include "gui_2dwpu.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GUI_2DWPU w;
	w.show();
	return a.exec();
}
