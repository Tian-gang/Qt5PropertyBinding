#ifndef REDRECT_H
#define REDRECT_H

#include <QProperty.h>

#include <QWidget>

class RedRect : public QWidget
{
    Q_OBJECT
   public:
    explicit RedRect(QWidget* parent = nullptr);

    QPropertyAlias<int> widthPropery() { return &m_width; }
    QPropertyAlias<int> heightPropery() { return &m_height; }
    QPropertyAlias<qreal> alphaPropery() { return &m_alpha; }

   protected:
    void paintEvent(QPaintEvent* event) override;

   private:
    QProperty<int> m_width{100};
    QProperty<int> m_height{100};
    QProperty<qreal> m_alpha{1.0};

    using changeHandlerFuncType = QPropertyChangeHandler<std::function<void()>>;
    changeHandlerFuncType m_widgetChangedHandler{[] {}};
    changeHandlerFuncType m_heightChangedHandler{[] {}};
    changeHandlerFuncType m_alphaChangedHandler{[] {}};
};

#endif  // REDRECT_H
