#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H
#include <QVector>
#include <QPair>
#include <unordered_map>
#include<QWidget>
#include<QPropertyAnimation>
using std::unordered_map;
class GRidWidget: public QWidget
{
public:
    GRidWidget(double _h2w, double _min_wid=200, double _max_width=300, int _space=5);
    void add_widget(QWidget*);
    void delete_widget(int index);

    void resizeEvent(QResizeEvent *event);
    void set_is_show(const int &index,const bool &_is_show);
    void anim_adjust();
    QVector<QPair<QWidget*, bool>> widget_list;
private:
    QVector<QPropertyAnimation*> anim_list;
    unordered_map<QWidget*, int> widget2index;
    double now_width;
    double now_height;
    double h2w;
    int row;
    int column;
    double min_w;
    double max_w;
    int space;
    bool first = true;
    void adjust();


};

#endif // GRIDWIDGET_H
