/*
 * @Author: string
 * @Date: 2024-02-29 23:24:45
 * @LastEditors: string
 * @LastEditTime: 2024-03-05 11:13:49
 * @FilePath: \ScreenShot\include\screenshot\MainWindow.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#include <QMainWindow>
#include <QObject>
#include <QColor>
#include <QString>
#include <QMouseEvent>
#include <QLabel>
#include <QPoint>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <memory>
#include <QGraphicsDropShadowEffect>
#include <Vector>
#include <string>
#include <QDebug>
#include <QPushButton>
#include <QIcon>
#include <QScrollArea>
#include <QPainterPath>
#include <screenshot/Mask.h>
#include <screenshot/SlidePage.h>
#include <screenshot/TwoBtn.h>
#include <screenshot/InputBox.h>
#include<screenshot/GridWidget.h>
#include<QVector>
using std::pair;
using std::shared_ptr;
using std::string;
using std::vector;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    // ~MainWindow(){};
private:
    // 常量定义
    static constexpr char BG_COLOR[] = "rgb(251,251,251)";
    static constexpr int BORDER_RADIUS = 20;
    
    QWidget* wrap_widget, *main_widget;
    QVBoxLayout *wrap_layout,*main_layout;
    enum {
        M_LEFT = 1,
        M_TOP = 1 << 1,
        M_BOTTOM = 1 << 2,
        M_RIGHT = 1 << 3,
        M_NONE = 0
    };
    int mouse_state = M_NONE, last_mouse_s = M_NONE;
    
//     // 变量定义
    shared_ptr<QPoint> is_press = nullptr;
    QSize width_size;
    bool is_max = false;
    shared_ptr<Mask> my_mask = nullptr;
    shared_ptr<SlidePage> about_page = nullptr;
    QScrollArea *scroll_contain;
    GRidWidget *contain;
//     QScrollArea *scroll_contain = nullptr;
//     QWidget *contain_ = nullptr;

//     // 外观函数
    void Init();
    void add_close_btn(); // 去除边框并添加关闭并添加移动
    void add_wight();
    void set_mask(); // 将圆矩形部分变为透明
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event) { is_press = nullptr; }
    void max_btn_click();
    void resizeEvent(QResizeEvent *event);
    void showEvent(QShowEvent *event);


    // 业务函数
    bool add_item(QString path, bool is_write=false);
    void delete_item(QWidget *);
    int find_item_i(QWidget *w);
    void text_change(const QString &s);
    QVector<QString> file_data;
    void get_history(); //获取历史记录并加入
    void write_his();
//     void run_cmd(QString cmd);
//     void text_change(const QString &s);
//     void cmd_init();

//     void item_add(vector<string> &v);
//     int find_item_i(QWidget *w);

// private slots:
//     void item_click(QWidget *w, bool is_show);
//     void item_edit(QWidget *w);
//     void item_delete(QWidget *w);

};
