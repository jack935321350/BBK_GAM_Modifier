#include "colorbutton.h"

ColorButton::ColorButton(QWidget *parent) : QPushButton(parent)
{

}

ColorButton::~ColorButton()
{

}

void ColorButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::black);
    painter.setBrush(_color);
    painter.drawRect(rect());
}

void ColorButton::mousePressEvent(QMouseEvent *e)
{
    QPushButton::mousePressEvent(e);
}

void ColorButton::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton) {
        QColorDialog colorDlg(this);
        QColor color = colorDlg.getColor();
        if(color.isValid()) {
            _color = color;
            update();
        }
    }
    QPushButton::mouseReleaseEvent(e);
}
