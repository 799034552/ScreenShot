#include<screenshot/Mask.h>
#include<QApplication>
#include<QScreen>
#include<QPainter>
#include<QPainterPath>
#include<QLayout>
#include<QLabel>
#include<QPushButton>
#include<string>
#include<screenshot/Shot.h>
#include<QLayout>
#include<QFileDialog>
Mask::Mask(QWidget* parent):QWidget(parent),par(parent),
    yes(":/icons/yes.svg"),stop(":/icons/stop.svg") {
    // 无边框且全屏
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    auto screenSize = QApplication::primaryScreen()->size();
    setGeometry(0, 0, screenSize.width(), screenSize.height());
    this->setMouseTracking(true);
    reset();
    control_box_init();
    show_box_init();
    connect(shot_io.sp_shot.get(), &Screenshot::send_pic, this,getMat);
}
void Mask::show_box_init(){
    show_box = new QWidget(par);
    show_box->setObjectName("show_box");
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(2,2,2,2);
    show_box->setLayout(layout);
    show_box->setStyleSheet("QWidget#show_box{background-color:#fcfefe; border:3px solid #4270ee}");
    show_label = new QLabel(show_box);
    show_label->setText("asdfhikasdbciasud");
    show_label->setFixedSize(200,200);
    layout->addWidget(show_label);
    show_box->hide();
    show_label->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    show_box->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    // show_box->setAttribute(Qt::WA_TranslucentBackground);
}

void Mask::set_show_pic(QImage &pic){
    int newWidth = 190; // 新宽度
    int newHeight = 190; // 新高度
    QPixmap pixmap = QPixmap::fromImage(pic);
    pixmap = pixmap.scaled(newWidth, newHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    show_label->setPixmap(pixmap);
}

void Mask::control_box_init(){
    control_box = new QWidget(par);
    control_box->setObjectName("control_box");
    auto *layout = new QHBoxLayout();
    layout->setContentsMargins(2,2,2,2);
    control_box->setLayout(layout);
    control_box->resize(60, 30);

    QPixmap pixmap(":/icons/no.svg"); // 替换为您图片的路径
    QPushButton *imageButton = new QPushButton(control_box);
    imageButton->setIcon(QIcon(pixmap));
    imageButton->setIconSize(QSize(24,24));
    imageButton->setFixedSize(30,30);
    imageButton->setStyleSheet("QPushButton { border: none; padding: 0; margin: 0; background-color:#fcfefe} QPushButton:hover{background-color:#d5d6d5}");
    layout->addWidget(imageButton);
    layout->addSpacing(0);

    imageButton_2 = new QPushButton(control_box);
    imageButton_2->setIcon(yes);
    imageButton_2->setIconSize(QSize(24,24));
    imageButton_2->setFixedSize(30,30);
    imageButton_2->setStyleSheet("QPushButton { border: none; padding: 0; margin: 0; background-color:#fcfefe} QPushButton:hover{background-color:#d5d6d5}");
    layout->addWidget(imageButton_2);

    connect(imageButton, &QPushButton::clicked, this, [=]{
        reset();
        hide();
        shot_io.end_shot();
    });
    connect(imageButton_2, &QPushButton::clicked, this, [=]{
        if (!is_running){
            // 开始截图
            is_running = true;
            imageButton_2->setIcon(stop);
            qDebug()<<press_point<<end_point;
            shot_io.start_shot(press_point.x()+2, press_point.y()+2, end_point.x() - press_point.x()-2, end_point.y() - press_point.y()-2, shot_type);
            show_box->show();
            if (press_point.y() > 210)
                show_box->move(press_point.x(), press_point.y() - 210);
            else if (press_point.x() > 210)
                show_box->move(press_point.x() - 210, press_point.y());
            else if (rect().width() - end_point.x() > 210)
                show_box->move(end_point.x() + 10, press_point.y());
        }
        else{
            shot_io.end_shot();
            reset();
            hide();
            // 弹出保存图片的框
            QString filePath = QFileDialog::getSaveFileName(nullptr, "Save Image", QDir::homePath(), "Images (*.png *.jpg *.bmp)");

            // 如果用户取消选择或未提供文件名，则返回
            if (filePath.isEmpty()) {
                return;
            }
            res_img.save(filePath);
            emit get_pic(filePath);
        }
    });
    control_box->setStyleSheet("QWidget#control_box{background-color:#fcfefe; border:2px solid #4270ee}");
    control_box->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
}


void Mask::paintEvent(QPaintEvent* event){
    QPainter painter(this);
    // 设置整个窗口的背景为半透明黑色
    if (!is_press_end)
        painter.fillRect(rect(), QColor(0, 0, 0, 128));

    // 开始按下
    if (is_press){
        // 定义一个透明的矩形区域（"孔"）
        QRect transparentHole(press_point, end_point); // 位置和大小根据需要调整

        // 设置蒙版，使得除了"孔"的部分外，其余部分接收事件
        // setMask((QRegion(rect()) - QRegion(transparentHole)).translated(-geometry().topLeft()));
        if (is_press_end){
            setMask(QRegion(transparentHole.adjusted(-10, -10, 10, 10)) - QRegion(transparentHole.adjusted(10, 10, -10, -10)));
        } else
            setMask(QRegion(rect()) - QRegion(transparentHole.adjusted(10, 10, -10, -10)));
        
        
        // 清除"孔"的区域，使其完全透明
        QPainterPath path;
        path.addRect(transparentHole);
        painter.setCompositionMode(QPainter::CompositionMode_Clear);
        painter.fillPath(path, Qt::transparent);

        // 画一个矩形
        int border_width = 5;
        QPainter painter(this);
        painter.setPen(QPen(QColor("#4270ee"), border_width)); // 应用画笔
        painter.drawRect(transparentHole.adjusted(-border_width / 2, -border_width / 2, border_width /2, border_width/2)); // 矩形的位置和大小
        // 画圆形
        painter.setRenderHint(QPainter::Antialiasing);
        int circle_width = 9;
        painter.setBrush(QBrush(QColor("#4270ee")));
        painter.setPen(QPen(Qt::white, 2));
        painter.drawEllipse(transparentHole.topLeft().x()-border_width / 2 - circle_width / 2, transparentHole.topLeft().y()-border_width / 2 - circle_width / 2, circle_width, circle_width);
        painter.drawEllipse(transparentHole.topRight().x()+border_width / 2 - circle_width / 2, transparentHole.topRight().y()-border_width / 2 - circle_width / 2, circle_width, circle_width);
        painter.drawEllipse(transparentHole.bottomLeft().x()-border_width / 2 - circle_width / 2, transparentHole.bottomLeft().y()+border_width / 2 - circle_width / 2, circle_width, circle_width);
        painter.drawEllipse(transparentHole.bottomRight().x()+border_width / 2 - circle_width / 2, transparentHole.bottomRight().y()+border_width / 2 - circle_width / 2, circle_width, circle_width);
    }

    //还没有按下
    if (!is_press){
        QPainter painter(this);
        painter.setPen(QPen(QColor("#4270ee"), 2));
        painter.drawLine(mouse_point.x(), 0, mouse_point.x(), this->rect().height());
        painter.drawLine(0, mouse_point.y(), this->rect().width(), mouse_point.y());
    }

}

void Mask::mousePressEvent(QMouseEvent *event){
    if (event->button() == Qt::LeftButton) {
        is_press = true;
        press_point = event->pos();
    } else if (event->button() == Qt::RightButton){
        reset();
        this->hide();
    }


}
void Mask::mouseMoveEvent(QMouseEvent *event){
    if (!is_press_end){
        end_point = event->pos();
    }
    mouse_point = event->pos();
    update();
}
void Mask::mouseReleaseEvent(QMouseEvent *event){
    if (!is_press_end){
        is_press_end = true;
        end_point = event->pos();
    }
    control_box->show();
    control_box->move(end_point.x() - control_box->size().width(), end_point.y() + 10);

    // show_box->show();
    // show_box->move(press_point.x(), press_point.y() - 120);
    // show_box->move(0,0);
    update();
    
}
void Mask::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape){
        shot_io.end_shot();
        this->hide();
    }
    else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return){
        if (!(is_press && is_press_end))
            return;
        // 开始截图逻辑
        shot_io.start_shot(press_point.x() + 3,press_point.y() + 3,
            end_point.x() - press_point.x() -3,end_point.y() - press_point.y() -3,shot_type);
    }
}

void Mask::enterEvent(QEnterEvent *event){
    setCursor(Qt::CrossCursor);
}
void Mask::leaveEvent(QEvent* event){
    setCursor(Qt::ArrowCursor);
}

void Mask::showEvent(QShowEvent *event){
    // 获取控件全局的坐标
    widget_point = mapToGlobal(QPoint(0,0));
    reset();
    par->hide();
}

void Mask::reset() {
    is_press = false;
    is_press_end = false;
    
    setMask(QRegion());
    if (control_box)
        control_box->hide();
    if (show_box)
        show_box->hide();
    if (imageButton_2)
        imageButton_2->setIcon(yes);
    is_running = false;
    par->show();
}


