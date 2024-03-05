/*
 * @Author: string
 * @Date: 2024-03-04 17:21:09
 * @LastEditors: string
 * @LastEditTime: 2024-03-05 10:51:55
 * @FilePath: \ScreenShot\include\screenshot\ImgBox.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once
#include <QWidget>
#include <QString>
#include<QPixmap>
#include<QLabel>
#include<QPropertyAnimation>
#include<QGraphicsOpacityEffect>
class ImgBox: public QWidget{
    Q_OBJECT
    public:
        ImgBox(QWidget *parent, QString img_path);
        QString img_path;
        QLabel *label;
        QPixmap *pic;
        QWidget *img_widget;
        QWidget *mask_widget;
        QPropertyAnimation * in_ani;
        QGraphicsOpacityEffect *effect;
        void resizeEvent(QResizeEvent *event);
        void enterEvent(QEnterEvent *event);
        void leaveEvent(QEvent* event);
        void showEvent(QShowEvent *event);
    signals:
        void delete_img(QWidget*);

};