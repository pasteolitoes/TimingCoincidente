#include "mainmenu.h"
#include <Windows.h>
#include <QApplication>
#include <QObject>
#include <QStyle>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainMenu* w = new MainMenu;

    qDebug() << "this shit started";

    if(!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
    {
        qDebug() << "Não setou o processo para High Priority";
    }

    w->setAttribute(Qt::WA_DeleteOnClose);

    QObject::connect(&a, SIGNAL(lastWindowClosed()),w,SLOT(close()));

    w->setGeometry(QStyle::alignedRect(Qt::LeftToRight,
                                       Qt::AlignCenter,
                                       w->size(),
                                       qApp->desktop()->availableGeometry()
                                       )
                   );
    w->show();

    return a.exec();
}
