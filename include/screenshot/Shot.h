/*
 * @Author: string
 * @Date: 2024-03-02 10:39:38
 * @LastEditors: string
 * @LastEditTime: 2024-03-05 14:29:51
 * @FilePath: \ScreenShot\include\screenshot\Shot.h
 * @Description: 
 * 
 * Copyright (c) 2024 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once
#include <Windows.h>
#include <QObject>
#include <QThread>
#include <memory>
#include <pthread.h>
#include<QImage>
#include <stdio.h>
#include<string>
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
using std::shared_ptr;
using cv::Mat;



QImage cvMat2QImage(const cv::Mat &mat);
// 截图线程
class Screenshot : public QObject
{
    Q_OBJECT
public:
    Screenshot();
    double static getZoom();
    double zoom;
    Mat old_shot;
    Mat tmp1, tmp2;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 用于停止的锁
    bool is_running = false;
    Mat getScreenshot();
    Mat getScreenshot(int x, int y, int width, int height);
    Mat concat_pic(Mat &a, Mat &b, int shot_type,bool is_show=false);
    void set_is_running(bool b);

private:
    int m_width;
    int m_height;
    HDC m_screenDC;
    HDC m_compatibleDC;
    HBITMAP m_hBitmap;
    LPVOID m_screenshotData = nullptr;
public slots:
    void start_job(int x, int y, int width, int height, int shot_type);
signals:
    void send_pic(cv::Mat &a, cv::Mat &b);
};

// 截图线程的接口
class ShotIO : public QObject{
    Q_OBJECT

    public:
        shared_ptr<Screenshot> sp_shot;
        shared_ptr<QThread> sp_thread;
        ShotIO(){          
            sp_thread = shared_ptr<QThread>(new QThread());
            sp_shot = shared_ptr<Screenshot>(new Screenshot());
            sp_shot->moveToThread(sp_thread.get());
            connect(this, &ShotIO::start_job, sp_shot.get(), &Screenshot::start_job);
            sp_thread->start();
        }
        void start_shot(int x, int y, int width, int height, int shot_type){
            sp_shot->is_running = true;
            emit start_job(x, y, width, height, shot_type);
        }
        void end_shot(){
            sp_shot->set_is_running(false);
        };
        void get_now_img(){
            
        };
        ~ShotIO(){
            sp_thread->requestInterruption();
            sp_thread->quit();
            sp_thread->wait();
        }
    signals:
        void start_job(int x, int y, int width, int height, int shot_type);
    public slots:
        // void getMat(cv::Mat &mat, cv::Mat &b){
        //     static int i = 0;
        //     auto img2 = cvMat2QImage(b);
        //     auto img = cvMat2QImage(mat);
        //     printf("start_to_write\n");
        //     img.save("./test" + QString::number(i++) + ".jpg");
        //     img2.save("./test.jpg");
        //     // static int i = 0;
        //     // auto res = sp_shot->concat_pic(b, mat);
        //     // auto img2 = cvMat2QImage(res);
        //     // img2.save("F:/c/C++/ScreenShot/test.jpg");
        //     // auto img = cvMat2QImage(mat);
        //     // img.save("F:/c/C++/ScreenShot/test" + QString::number(i++) + ".jpg");
        // }
};
extern ShotIO shot_io;
