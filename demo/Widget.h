#ifndef WIDGET_H
#define WIDGET_H

#include <QProperty.h>

#include <QWidget>
#include <functional>

class Widget : public QWidget
{
    Q_OBJECT
   public:
    explicit Widget(QWidget* parent = nullptr);

   protected:
    void resizeEvent(QResizeEvent* event) override;

   private:
    QProperty<int> m_width;
    QProperty<int> m_height;
};

#endif  // WIDGET_H
