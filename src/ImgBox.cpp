/*
 * @Author: string
 * @Date: 2024-03-04 17:21:12
 * @LastEditors: string
 * @LastEditTime: 2024-03-05 14:13:16
 * @FilePath: \ScreenShot\src\ImgBox.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include<screenshot/ImgBox.h>
#include<QLabel>
#include<QLayout>
#include<QPushButton>
#include <QFileInfo>
#include <QDesktopServices>

ImgBox::ImgBox(QWidget *parent, QString _img_path):QWidget(parent),img_path(_img_path){
    // 设置当前的layout
    auto layout = new QVBoxLayout();
    this->setLayout(layout);
    img_widget = new QWidget(this);
    QFileInfo fileInfo(_img_path);
    auto file_name_label = new QLabel(fileInfo.fileName());
    file_name_label->setStyleSheet("font-family: '微软雅黑';font-weight: normal;font-size:18px;");
    file_name_label->setAlignment(Qt::AlignCenter);
    layout->addWidget(img_widget);
    layout->addWidget(file_name_label);
    layout->setContentsMargins(0,0,0,0);
    // 设置图片模块
    pic = new QPixmap(_img_path);
    label = new QLabel(img_widget);
    auto img_layout = new QVBoxLayout();
    img_widget->setLayout(img_layout);
    img_layout->addWidget(label);
    // img_layout->setContentsMargins(0,0,0,0);
    img_layout->setContentsMargins(5,2,5,2);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setPixmap(*pic);
    img_widget->setStyleSheet("background-color:#f0f0f0;border-radius:15px");
    //遮罩与删除层
    mask_widget = new QWidget(this);
    mask_widget->setStyleSheet("background-color:rgba(0,0,0, 50);border-radius:15px");
    auto mask_layout = new QVBoxLayout();
    mask_widget->setLayout(mask_layout);
    mask_layout->addStretch(1);
    mask_layout->setContentsMargins(10,2,10,10);
    auto mask_but_layout = new QHBoxLayout();
    auto path_btn = new QPushButton("打开路径",mask_widget);
    auto delete_btn = new QPushButton("删除",mask_widget);
    path_btn->setCursor(QCursor(Qt::PointingHandCursor));
    delete_btn->setCursor(QCursor(Qt::PointingHandCursor));
    path_btn->setFlat(true);
    delete_btn->setFlat(true);
    mask_but_layout->addWidget(path_btn, 1);
    mask_but_layout->addWidget(delete_btn, 1);
    mask_but_layout->setContentsMargins(0,0,0,0);
    mask_layout->addLayout(mask_but_layout);
    path_btn->setMinimumHeight(30);
    path_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    delete_btn->setMinimumHeight(30);
    delete_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    path_btn->setStyleSheet("QPushButton{background-color:#409eff;border-radius:5px;font-family: '微软雅黑';font-weight: bold;font-size:15px;color:white;}\n"
                            "QPushButton:hover{background-color:#409eff;border-radius:5px;font-family: '微软雅黑';font-weight: bold;font-size:15px;color:white;}");
    delete_btn->setStyleSheet("QPushButton{background-color:#f56c6c;border-radius:5px;font-family: '微软雅黑';font-weight: bold;font-size:15px;color:white}QPushButton:hover{}");
    effect = new QGraphicsOpacityEffect(mask_widget);
    effect->setOpacity(0);
    mask_widget->setGraphicsEffect(effect);
    //设置动画
    in_ani = new QPropertyAnimation(effect, "opacity");
    in_ani->setDuration(500);
    in_ani->setStartValue(0.0);
    in_ani->setEndValue(0.99);
    // 绑定事件
    auto dir_path = QUrl::fromLocalFile(QFileInfo(_img_path).absolutePath());
    connect(path_btn, &QPushButton::clicked, this, [=]{
        QDesktopServices::openUrl(dir_path);
    });
    connect(delete_btn, &QPushButton::clicked, this, [&]{
        emit this->delete_img(this);
    });
}

void ImgBox::resizeEvent(QResizeEvent *event){
    label->setPixmap(pic->scaled(label->size().width(), label->size().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    mask_widget->resize(img_widget->size());
    mask_widget->setWindowOpacity(0);
}

void ImgBox::showEvent(QShowEvent *event){
    label->setPixmap(pic->scaled(label->size().width(), label->size().height(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    mask_widget->resize(img_widget->size());
}
void ImgBox::enterEvent(QEnterEvent *event){
    // mask_widget->show();
    in_ani->start();
}
void ImgBox::leaveEvent(QEvent* event){
    if (in_ani->state() == QAbstractAnimation::Running)
        in_ani->stop();
    effect->setOpacity(0.0);
}