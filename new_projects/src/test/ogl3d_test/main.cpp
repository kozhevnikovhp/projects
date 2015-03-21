#include <QtGui/QApplication>
#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	cMainWindow w;
    w.show();

    return a.exec();
}
