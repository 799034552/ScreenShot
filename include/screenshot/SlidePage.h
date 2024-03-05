#ifndef SLIDEPAGE_H
#define SLIDEPAGE_H
#include <QWidget>
#include <memory>
#include <QPropertyAnimation>
#include <screenshot/AIcon.h>
#include<QLayout>
#include<QAnimationGroup>
#include<QParallelAnimationGroup>
using std::shared_ptr;
class SlidePage : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double m_opacity MEMBER opacity NOTIFY opacity_change)
private:
    QWidget *parent = nullptr;
    QString title;
    int opacity = 100;
    QVBoxLayout *layout = nullptr;
    shared_ptr<QParallelAnimationGroup> out_animation = nullptr;
    shared_ptr<QPropertyAnimation> mask_out = nullptr;
    shared_ptr<QPropertyAnimation> page_out = nullptr;
    shared_ptr<QParallelAnimationGroup> in_animation = nullptr;
    shared_ptr<QPropertyAnimation> mask_int = nullptr;
    shared_ptr<QPropertyAnimation> page_int = nullptr;
    bool is_max = false;
    float rate;
    int min_width;
    QRect margin;
    static constexpr int dur = 300;
    void Init();
    void add_widget();
protected:
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
public:
    shared_ptr<QWidget> contain_page;
//    SlidePage(const QString &title, QWidget *parent);
    SlidePage(const QString &title, QWidget *parent, float _rate = 0.3, int min_width = 300, QRect margin = QRect(30,60,30,30));
    virtual void slide_in(bool is_max);
    virtual void slide_out();
    void addWidget(QWidget* widget);
    void addLayout(QLayout* layout, const int & strecth = 0);
    void end_add() {layout->addStretch();}

signals:
    void opacity_change();

private slots:
    void get_opacity_change();





};

#endif // SLIDEPAGE_H
