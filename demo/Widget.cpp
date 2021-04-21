#include "Widget.h"

#include <QApplication>
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QResizeEvent>
#include <QScreen>

#include "RedRect.h"

Widget::Widget(QWidget* parent) : QWidget(parent)
{
    resize(800, 600);
    auto redRect = new RedRect(this);

    redRect->widthPropery().setBinding([this] { return m_width / 2; });
    redRect->heightPropery().setBinding([this] { return (int)(m_height * 3.0 / 4); });
    redRect->alphaPropery().setBinding([this] {
        auto screenSize = qApp->primaryScreen()->size();
        return (m_width * m_height) / static_cast<qreal>(screenSize.width() * screenSize.height());
    });
}

void Widget::resizeEvent(QResizeEvent* event)
{
    m_width = event->size().width();
    m_height = event->size().height();

    QWidget::resizeEvent(event);
}
