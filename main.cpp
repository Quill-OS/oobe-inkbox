#include "oobewindow.h"

#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    oobewindow w;
    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    w.resize(screenGeometry.width(), screenGeometry.height());
    w.showFullScreen();
    return a.exec();
}
