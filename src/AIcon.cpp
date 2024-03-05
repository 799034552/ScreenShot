#include <screenshot/AIcon.h>
#include <QPropertyAnimation>
#include <QPainter>

AIcon::AIcon(const QString &url, const int &width, const int &height, bool is_animate)
{
    pic.reset(new QPixmap(url));
    this->setMinimumSize(width, height);
    this->setMaximumSize(width, height);
    if (is_animate)
    {
        animation.reset(new QPropertyAnimation(this, "rotate_angle"));
        animation->setDuration(250);
        animation->setStartValue(0);
        animation->setEndValue(90);
        animation->setEasingCurve(QEasingCurve::Linear);
    }
    bg_color = &normal_color;
}

void AIcon::paintEvent(QPaintEvent*)
{

    QPainter bgPainter(this);
    bgPainter.setRenderHints(QPainter::Antialiasing);
    bgPainter.setPen(Qt::NoPen);
    bgPainter.setBrush(*bg_color);
    bgPainter.drawRoundedRect(this->rect(), radius, radius);
    QPainter pixmapPainter(this);
    pixmapPainter.setRenderHints(QPainter::Antialiasing);
    pixmapPainter.setRenderHints(QPainter::LosslessImageRendering);
    pixmapPainter.setPen(Qt::NoPen);
    pixmapPainter.translate(width() / 2, height() / 2);
    pixmapPainter.rotate(rotation);
    pixmapPainter.translate(-width() / 2, -height() / 2);
    int w = size_rate * width();
    int h = size_rate * height();
    pixmapPainter.drawPixmap(width() / 2 - w / 2, height() / 2 - h / 2, w, h, *pic);
}

void AIcon::enterEvent(QEnterEvent *)
{
    bg_color = &hover_color;
    if (animation)
        animation->start();
    else
        update();


}
void AIcon::leaveEvent(QEvent* )
{

    if (animation) animation->stop();
    rotation = 0;
    bg_color = &normal_color;
    update();
}
void AIcon::mousePressEvent(QMouseEvent* )
{
    size_rate = 0.9;
    update();
}
void AIcon::mouseReleaseEvent(QMouseEvent* )
{
    size_rate = 1;
    update();
    emit click();
}
