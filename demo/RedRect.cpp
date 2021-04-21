#include "RedRect.h"

#include <QPainter>

RedRect::RedRect(QWidget* parent) : QWidget(parent)
{
    m_widgetChangedHandler = m_width.subscribe(std::function([this] { resize(m_width, height()); }));
    m_heightChangedHandler = m_height.subscribe(std::function([this] { resize(width(), m_height); }));
    m_alphaChangedHandler = m_alpha.subscribe(std::function([this] { update(); }));
}

void RedRect::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    QColor paintColor(Qt::red);
    paintColor.setAlphaF(m_alpha);

    painter.fillRect(rect(), paintColor);
}
