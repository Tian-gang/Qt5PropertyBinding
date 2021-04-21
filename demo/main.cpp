#include <QProperty.h>

#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QProperty<bool> p1{false};
    QProperty<bool> p2{Qt::makePropertyBinding([&] { return p1.value(); })};
    qDebug() << p2;
    p1 = true;
    qDebug() << p2;

    return a.exec();
}
