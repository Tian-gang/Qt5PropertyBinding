#include <QProperty.h>

#include <QCoreApplication>
#include <QDebug>

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    QProperty<int> a1(4);
    QProperty<int> a2(3);
    QProperty<int> a3;
    auto a4 = new QProperty<int>(5);
    QPropertyAlias<int> p1(a4);

    a3.setBinding([&]() { return a2 + a1; });
    p1.setBinding([&]() { return a1 * a2; });
    auto handler = a2.onValueChanged([&]() { qDebug() << "onValueChanged" << a2; });

    a2 = 10;
    qDebug() << a3;
    qDebug() << p1;
    qDebug() << p1.isValid();
    delete a4;

    qDebug() << p1.isValid();
    qDebug() << p1;
    p1 = 10;
    qDebug() << p1;

    return a.exec();
}
