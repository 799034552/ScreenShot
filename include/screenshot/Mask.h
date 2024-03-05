#pragma once
#include<QWidget>
#include<QKeyEvent>
#include<screenshot/Shot.h>
#include<QPushButton>
#include<QLabel>
class Mask: public QWidget{
    Q_OBJECT
    public:
        Mask(QWidget *parent);
        QWidget *par;
        bool is_press, is_press_end;
        bool is_running;
        int shot_type = 0;
        QPoint mouse_point;
        QPoint press_point;
        QPoint end_point;
        QPoint widget_point;
        QPixmap yes, stop;
        QWidget *control_box = nullptr; //控制框
        QWidget *show_box = nullptr; //展示框
        QLabel *show_label = nullptr;
        QPushButton *imageButton_2 = nullptr;
        void paintEvent(QPaintEvent* event) override;
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void keyPressEvent(QKeyEvent *event);
        void enterEvent(QEnterEvent *event);
        void leaveEvent(QEvent* event);
        void reset();
        void showEvent(QShowEvent *event);
        void control_box_init();
        void show_box_init();
        void set_show_pic(QImage &pic);
        QImage res_img;

    public slots:
        void getMat(cv::Mat &mat, cv::Mat &b){
            static int i = 0;
            res_img = cvMat2QImage(b);
            // 将每一步截图都保存下来
            // auto img = cvMat2QImage(mat);
            // printf("start_to_write\n");
            // img.save("./test" + QString::number(i++) + ".jpg");
            // res_img.save("./test.jpg");
            set_show_pic(res_img);
        }
    signals:
        void get_pic(QString path);

};