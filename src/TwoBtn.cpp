/*
 * @Author: string
 * @Date: 2024-03-03 21:44:52
 * @LastEditors: string
 * @LastEditTime: 2024-03-04 17:18:10
 * @FilePath: \ScreenShot\src\TwoBtn.cpp
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#include<screenshot/TwoBtn.h>
#include<QHBoxLayout>
#include<QPainterPath>
TwoBtn::TwoBtn(QWidget *par):QLabel(par){
    this->setStyleSheet(QString::asprintf("QLabel{font-family:'微软雅黑';font-weight: normal;background-color: #f0f0f0;font-size:20px;border-radius: %dpx;text-align: center}", radius));
    this->setText("开始截图");
    this->setAlignment(Qt::AlignCenter);
    auto layout = new QHBoxLayout();
    this->setLayout(layout);
    layout->setSpacing(0);
    left_btn = new QPushButton(this);
    right_btn = new QPushButton(this);
    layout->addWidget(left_btn);
    
    layout->addWidget(right_btn);
    layout->setContentsMargins(0,0,0,0);
    left_btn->setText("叠加模式");
    right_btn->setText("覆盖模式");
    left_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    right_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    left_btn->hide();
    right_btn->hide();
    connect(left_btn, &QPushButton::clicked, this, [&]{
        emit this->left_click();
    });
    connect(right_btn, &QPushButton::clicked, this, [&]{
        emit this->right_click();
    });
}
void TwoBtn::enterEvent(QEnterEvent *event){
    QString hover_status = "QPushButton::hover{background-color: #39BAE8}QPushButton:pressed{background-color: rgb(223, 249, 255)}";
    left_btn->setStyleSheet(QString::asprintf("QPushButton{font-family: '微软雅黑';font-weight: normal;background-color: #B9EDF8;border:none;border-radius:%dpx;margin-right:-%dpx;font-size:20px;color:white}", radius, radius) + hover_status);
    right_btn->setStyleSheet(QString::asprintf("QPushButton{font-family: '微软雅黑';font-weight: normal;background-color: #B9EDF8;border:none;border-radius:%dpx;margin-left:-%dpx;font-size:20px;color:white}", radius, radius) + hover_status);
    left_btn->show();
    right_btn->show();
}
void TwoBtn::leaveEvent(QEvent* event){
    left_btn->hide();
    right_btn->hide();
}