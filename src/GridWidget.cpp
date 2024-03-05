#include<screenshot/GridWidget.h>
#include<QWidget>

GRidWidget::GRidWidget(double _h2w, double _min_wid, double _max_wid, int _space):
    now_width(200),
    h2w(_h2w),
    column(1),
    min_w(_min_wid),
    max_w(_max_wid),
    space(_space)
{

}

void GRidWidget::add_widget(QWidget* widget)
{
    widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    widget_list.push_back(QPair<QWidget*, bool>(widget, true));
    widget2index[widget] = widget_list.size() - 1;
    QPropertyAnimation *tmp = new QPropertyAnimation(widget, "geometry");
    tmp->setDuration(300);
    anim_list.push_back(tmp);
    widget->setFixedSize(now_width, now_width*h2w);
}
void GRidWidget::delete_widget(int index)
{
    widget_list[index].first->hide();
    delete widget_list[index].first;
    widget_list.remove(index, 1);
    anim_list.remove(index, 1);
    anim_adjust();
}

void GRidWidget::resizeEvent(QResizeEvent *event)
{

    if ((this->width() - (column - 1)*space) / column < min_w)
    {
        if (column > 1)
            --column;
    }
    else if ((this->width() - (column - 1)*space) / column > max_w)
    {
        if ((this->width() - (column - 1)*space) / (column + 1) > min_w)
            ++column;
    }
    now_width = (this->width() - (column - 1)*space) / column;
    now_height = now_width * h2w;
    if (first) {
        adjust();
        first = false;
    } else
        anim_adjust();
}
void GRidWidget::adjust()
{
    int x = 0, y = 0;
    for(const QPair<QWidget*, bool>&item: widget_list)
    {
        if (!item.second){
            item.first->hide();
            continue;
        }
        item.first->show();
        item.first->move(x*now_width + x*space, y*now_height + y*space);
        item.first->setFixedSize(now_width, now_height);
        if (++x == column) {
            x = 0;
            ++y;
        }
    }
    this->resize(this->width(), x == 0? y*(now_height+space) : (y+1)*(now_height+space));
}
void GRidWidget::set_is_show(const int &index,const bool &_is_show)
{
    widget_list[index].second = _is_show;
}

void GRidWidget::anim_adjust()
{
    int x = 0, y = 0;
    for(int i = 0; i < widget_list.size(); ++i)
    {
        if (!widget_list[i].second){
            widget_list[i].first->hide();
            continue;
        }
        widget_list[i].first->show();
        anim_list[i]->setStartValue(widget_list[i].first->geometry());
        anim_list[i]->setEndValue(QRect(QPoint(x*now_width + x*space, y*now_height + y*space), QSize(now_width, now_height)));
        widget_list[i].first->setFixedSize(now_width, now_height);
        if (++x == column) {
            x = 0;
            ++y;
        }
        anim_list[i]->start();
    }
    this->resize(this->width(), x == 0? y*(now_height+space) : (y+1)*(now_height+space));
}
