/*
 * @Author: string
 * @Date: 2024-03-03 21:44:35
 * @LastEditors: string
 * @LastEditTime: 2024-03-04 11:27:38
 * @FilePath: \ScreenShot\include\screenshot\TwoBtn.h
 * @Description: 放上去后左右两边不同颜色的button
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#include<QPushButton>
#include<QWidget>
#include<QLabel>
class TwoBtn: public QLabel{
    Q_OBJECT
    public:
        int radius = 6;
        QPushButton *left_btn, *right_btn;
        TwoBtn(QWidget *par);
        void enterEvent(QEnterEvent *event);
        void leaveEvent(QEvent* event);
    signals:
        void left_click();
        void right_click();

};
