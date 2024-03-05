#include<screenshot/SlidePage.h>
#include<QGraphicsDropShadowEffect>
#include<QMouseEvent>
SlidePage::SlidePage(const QString &_title, QWidget *_parent, float _rate, int _min_width, QRect _margin)
    :QWidget(_parent),
     parent(_parent),
    title(_title),
    rate(_rate),
    min_width(_min_width),
    margin(_margin)
{
    connect(this,SIGNAL(opacity_change()) , this, SLOT(get_opacity_change()));
    setAttribute(Qt::WA_StyledBackground);
    Init();
}

void SlidePage::Init()
{
    // 设置内容框
    contain_page.reset(new QWidget(this));
    contain_page->setMouseTracking(true);
    contain_page->setStyleSheet("background-color: rgb(255,255,255); border-radius: 20px;margin;");
    QGraphicsDropShadowEffect *windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(30);
    windowShadow->setColor(QColor(0, 0, 0));
    windowShadow->setOffset(0, 0);
    contain_page->setGraphicsEffect(windowShadow);
    contain_page->move(0,0);
    this->setStyleSheet(QString::asprintf("background-color: rgba(100,100,100, %d);",opacity));


    // 设置退出动画
    out_animation.reset(new QParallelAnimationGroup(this));
    mask_out.reset(new QPropertyAnimation(this, "m_opacity"));
    mask_out->setDuration(dur);
    page_out.reset(new QPropertyAnimation(contain_page.get(), "pos"));
    page_out->setDuration(dur);
    out_animation->addAnimation(mask_out.get());
    out_animation->addAnimation(page_out.get());
    connect(out_animation.get(), &QPropertyAnimation::finished, this,[=](){
        this->hide();
    });

    // 设置进入动画
    in_animation.reset(new QParallelAnimationGroup(this));
    mask_int.reset(new QPropertyAnimation(this, "m_opacity"));
    mask_int->setDuration(dur);
    mask_int->setStartValue(0);
    mask_int->setEndValue(100);
    page_int.reset(new QPropertyAnimation(contain_page.get(), "pos"));
    page_int->setDuration(dur);
    in_animation->addAnimation(mask_int.get());
    in_animation->addAnimation(page_int.get());

    add_widget();

}

void SlidePage::add_widget()
{

    layout = new QVBoxLayout();
    QHBoxLayout *title_layout = new QHBoxLayout();
    QLabel *tit = new QLabel(title);

    QFont titleFont = QFont("Microsoft Sans Serif", 20);
    tit->setFont(titleFont);
    tit->setStyleSheet("color: rgb(100,100,100);font-weight: lighter;border-style:none;border-width:0px;margin-left:1px;");
    title_layout->setContentsMargins(11,0,11,30);
    layout->addLayout(title_layout);
    layout->setSpacing(0);
    layout->setContentsMargins(margin.x(), margin.y(), margin.width(), margin.height());
    AIcon *icon = new AIcon(":/assert/icons/back.svg", 20, 20, false);
    title_layout->addWidget(icon);
    title_layout->addWidget(tit);
    contain_page->setLayout(layout);
    connect(icon, &AIcon::click,this, [=]{slide_out();});
}
void SlidePage::addWidget(QWidget* widget)
{
    layout->addWidget(widget);
}
void SlidePage::addLayout(QLayout* _layout, const int &strecth)
{
    layout->addLayout(_layout, strecth);
}
void SlidePage::resizeEvent(QResizeEvent *event)
{
    contain_page->resize(this->width() * rate < min_width ? min_width : this->width() * rate, this->height());
}

void SlidePage::mousePressEvent(QMouseEvent *event)
{

    printf("mask click %d, %d\n", event->pos().x(), contain_page->width());
    if (event->pos().x() < contain_page->width())
        return QWidget::mousePressEvent(event);
    else
        slide_out();

}

void SlidePage::get_opacity_change()
{
    this->setStyleSheet(QString::asprintf("background-color: rgba(100,100,100,%d); border-radius: 20px;",opacity));
}

void SlidePage::slide_in(bool _is_max)
{
    if (_is_max)
    {
        contain_page->setStyleSheet("background-color: rgb(255,255,255); border-radius: 0px;");
    }

    if(in_animation->state() == QParallelAnimationGroup::Running) return;
    if(out_animation->state() == QParallelAnimationGroup::Running)
    {
        out_animation->stop();
    }

    page_int->setEndValue(0);
    if (contain_page->pos().x() == 0)
        page_int->setStartValue(contain_page->pos() - QPoint(contain_page->width(), 0));
    else
        page_int->setStartValue(contain_page->pos());
    mask_int->setStartValue(opacity);
    mask_int->setEndValue(100);
    in_animation->start();
    this->show();
}

void SlidePage::slide_out()
{
    if(out_animation->state() == QParallelAnimationGroup::Running) return;
    if(in_animation->state() == QParallelAnimationGroup::Running)
    {
        in_animation->stop();
    }

    page_out->setStartValue(contain_page->pos());
    page_out->setEndValue(contain_page->pos() - QPoint(contain_page->width(), 0));
    mask_out->setStartValue(opacity);
    mask_out->setEndValue(0);
    out_animation->start();
}
