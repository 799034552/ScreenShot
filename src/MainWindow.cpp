#include<screenshot/MainWindow.h>
#include<screenshot/AIcon.h>
#include<screenshot/Shot.h>
#include<screenshot/AboutItem.h>
#include<string>
#include<screenshot/ImgBox.h>
#include<QFile>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), main_widget(new QWidget()),wrap_widget(new QWidget(this)),wrap_layout(new QVBoxLayout())
{
    // 先添加主窗口
    main_widget->setObjectName("main_widget");
    this->setCentralWidget(wrap_widget);
    wrap_widget->setLayout(wrap_layout);
    wrap_layout->addWidget(main_widget);
    wrap_layout->setContentsMargins(30, 30, 30, 30); // 有maigin才正常显示阴影，不然会覆盖
    Init(); // 设置外观
    this->resize(600,500);
}

void MainWindow::Init()
{
    // //添加组件
    add_wight();
    // main_layout->addStretch(1);   

    // 创建蒙版
    my_mask = shared_ptr<Mask>(new Mask(this));
    connect(my_mask.get(), &Mask::get_pic, this, [&](QString _path){
        this->add_item(_path, true);
        contain->anim_adjust();
        qDebug()<<"有新文件"<<_path;
    });

    // 设置背景
    main_widget->setStyleSheet(QString::asprintf("QWidget#main_widget{background-color:#00FFFFFF;border:1.5px solid #686868;border-radius: %dpx;background-color: %s;}", BORDER_RADIUS, BG_COLOR));

    // 设置阴影
    QGraphicsDropShadowEffect *windowShadow = new QGraphicsDropShadowEffect(this);
    windowShadow->setBlurRadius(30);
    windowShadow->setColor(QColor(0, 0, 0));
    windowShadow->setOffset(0, 0);
    main_widget->setGraphicsEffect(windowShadow);
    
}

// 添加关闭按钮
void MainWindow::add_close_btn(){
    auto *lo = new QHBoxLayout();
    main_layout->addLayout(lo);

    // 创建按钮
    auto *button_min = new QPushButton();
    button_min->setFixedSize(12,12); // 固定尺寸
    button_min->setStyleSheet("QPushButton{background-color: #c2c2c2; border-radius: 6px;} QPushButton:hover{background-color: #45f978} QPushButton:pressed{background-color: #ffb11b}");
    auto *button_max = new QPushButton();
    button_max->setFixedSize(12,12); // 固定尺寸
    button_max->setStyleSheet("QPushButton{background-color: #c2c2c2; border-radius: 6px;} QPushButton:hover{background-color: #f9bf45} QPushButton:pressed{background-color: #ffb11b}");
    auto *button_close = new QPushButton();
    button_close->setFixedSize(12,12); // 固定尺寸
    button_close->setStyleSheet("QPushButton{background-color: #c2c2c2; border-radius: 6px;} QPushButton:hover{background-color: #f95445} QPushButton:pressed{background-color: #ffb11b}");

    lo->addStretch(1);
    lo->addWidget(button_min);
    lo->addWidget(button_max);
    lo->addWidget(button_close);
    lo->addSpacing(10);

    connect(button_min, &QPushButton::clicked, this, [=](){
        this->showMinimized();
    });
    connect(button_max, &QPushButton::clicked, this, max_btn_click);
    connect(button_close, &QPushButton::clicked, this, [=](){
        qDebug()<<main_widget->rect();
        this->close();
    });
    
    this->setWindowFlag(Qt::FramelessWindowHint); //设置为无边框
    this->setAttribute(Qt::WA_TranslucentBackground); //没有设置背景的给他变为透明的
    main_widget->setMouseTracking(true);
    wrap_widget->setMouseTracking(true);
    this->setMouseTracking(true);

}

// 添加组件
void MainWindow::add_wight() {
    // 设置主窗口的layout
    main_layout = new QVBoxLayout();
    main_widget->setLayout(main_layout);
    // 设置关闭按钮
    add_close_btn();

    // logo栏
    QWidget *logo = new QWidget();
    QHBoxLayout *HLayout = new QHBoxLayout();
    QLabel *logo_text = new QLabel("万向截图");
    QFont titleFont = QFont("Microsoft Sans Serif", 20, 5);
    logo_text->setFont(titleFont);
    logo_text->setStyleSheet("font-family: '微软雅黑';font-weight: normal;color: rgb(100,100,100);font-weight: lighter;border-style:none;border-width:0px;margin-left:1px;");
    AIcon *setting = new AIcon(QString(":/icons/settings.svg"));

    connect(setting, &AIcon::click, this, [=](){
        about_page->resize(main_widget->size());
        about_page->slide_in(is_max);
    });

    TwoBtn *create_new_btn = new TwoBtn(this);
    create_new_btn->setText("开始截图");
    create_new_btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    create_new_btn->setMinimumHeight(30);
    connect(create_new_btn, &TwoBtn::left_click, this, [&](){
        my_mask->show();
        my_mask->shot_type = 0;
    });
    connect(create_new_btn, &TwoBtn::right_click, this, [&](){
        my_mask->show();
        my_mask->shot_type = 1;
    });


    HLayout->addWidget(logo_text);
    HLayout->addWidget(setting);
    HLayout->addStretch(1);
    HLayout->addWidget(create_new_btn,5);
    logo->setLayout(HLayout);
    logo->setMaximumHeight(50);
    main_layout->addWidget(logo);

    // 搜索框
    QHBoxLayout *search_layout = new QHBoxLayout();
    InputBox *input_box = new InputBox();
    input_box->setMinimumSize(300, 50);
    input_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    search_layout->addStretch(1);
    search_layout->addWidget(input_box, 5);
    search_layout->addStretch(1);
    main_layout->addLayout(search_layout);
    auto tip_label = new QLabel("截图历史:", this);
    tip_label->setStyleSheet("font-family: '微软雅黑';font-weight: normal;font-size:15px;color: rgb(100,100,100);");
    main_layout->addWidget(tip_label);

    //主页
    // auto ttt = new ImgBox(this, "F:/a.jpg");
    // main_layout->addWidget(ttt);
    // ttt->setFixedSize(300,300);
    // 截图历史页面
    scroll_contain = new QScrollArea();
    scroll_contain->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    contain = new GRidWidget(1, 200, 400, 10);
    scroll_contain->setWidget(contain);
    contain->setStyleSheet("background-color:none;");
    scroll_contain->setStyleSheet("background-color:rgb(251, 251, 251);border:none");
    scroll_contain->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll_contain->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    contain->resize(600,700);
    main_layout->addWidget(scroll_contain);
    connect(input_box, &InputBox::text_change, this, &MainWindow::text_change);

    //测试数据
    // add_item("F:/a.jpg");
    // add_item("F:/b.jpg");
    // add_item("F:/c.jpg");
    // add_item("F:/d.jpg");
    // add_item("F:/f.jpg");
    get_history();

    // 滑动页面
    about_page.reset(new SlidePage("About", main_widget));
    about_page->setMouseTracking(true);
    about_page->addWidget(new AboutItem("author", "string"));
    about_page->addWidget(new AboutItem("version", "0.1"));
    about_page->end_add();
    about_page->raise();
    about_page->hide();

}
// 读取截图历史文件
void MainWindow::get_history(){
    QFile file("./his.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        while (!in.atEnd()) {
            auto file_path = in.readLine();
            add_item(file_path, false);
        }
        file.close();
    }
}
// 写入截图历史文件
void MainWindow::write_his(){
    // 创建一个 QFile 对象，并指定文件路径
    QFile file("./his.txt");

    // 尝试以只写方式打开文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return;
    }

    // 使用 QTextStream 写入文件内容
    QTextStream out(&file);
    for (const auto &s: file_data) {
        out << s +"\n"; // 使用 operator<<() 操作符写入文件内容
    }

    // 关闭文件
    file.close();
}
// 文本发生变化
void MainWindow::text_change(const QString &s)
{
    for(int i = 0; i < file_data.size(); ++i)
    {
        if(file_data[i].indexOf(s) != -1)
            contain->set_is_show(i, true);
        else
            contain->set_is_show(i, false);
    }
    contain->anim_adjust();
}

// 添加截图
bool MainWindow::add_item(QString path, bool is_write){
    if (!QFile::exists(path))
        return false;
    auto tmp = new ImgBox(this, path);
    tmp->setParent(contain);
    contain->add_widget(tmp);
    file_data.push_back(path);
    connect(tmp, &ImgBox::delete_img, this, delete_item);
    if (is_write)
        write_his();
    return true;

}

// 寻找是那个截图发出的信号
int MainWindow::find_item_i(QWidget *w)
{
    for(int i = 0; i < contain->widget_list.size(); ++i)
    {
        if (contain->widget_list[i].first == w)
            return i;
    }
    return -1;
}

// 删除历史截图记录
void MainWindow::delete_item(QWidget * w){
    int i = find_item_i(w);
    contain->delete_widget(i);
    qDebug()<<file_data;
    file_data.erase(file_data.begin()+i, file_data.begin()+i+1);
    qDebug()<<i;
    qDebug()<<file_data;
    write_his();
}

void MainWindow::max_btn_click() {
    is_max = !is_max;
    int margin = is_max? 0: 30;
    // 先变这个
    wrap_layout->setContentsMargins(margin, margin,margin,margin);
    if (is_max){
        main_widget->setStyleSheet(QString::asprintf("QWidget#main_widget{background-color:#00FFFFFF;border-radius: %dpx;background-color: %s;}", 0, BG_COLOR));
        this->showMaximized();
    }
    else{
        main_widget->setStyleSheet(QString::asprintf("QWidget#main_widget{background-color:#00FFFFFF;border:1.5px solid #686868;border-radius: %dpx;background-color: %s;}", BORDER_RADIUS, BG_COLOR));
        this->showNormal();
    }
}

// 设置遮罩
void MainWindow::set_mask(){
    int radius = is_max? 0: BORDER_RADIUS - 3;
    QPainterPath path;
    path.addRoundedRect(main_widget->rect(), radius , radius);
    QRegion mask(path.toFillPolygon().toPolygon());
    main_widget->setMask(mask);
}
void MainWindow::resizeEvent(QResizeEvent *event){
    set_mask();
    contain->resize(scroll_contain->width(), contain->height());
}

void MainWindow::showEvent(QShowEvent *event){
    set_mask();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    // qDebug()<<event->pos();
    if(event->button() == Qt::LeftButton){
        if (mouse_state == M_NONE){
            // 不在边缘并且超出范围就不处理
            if (event->pos().x() < main_widget->geometry().x() || event->pos().y() < main_widget->geometry().y())
                return;
            else if (event->pos().x() > main_widget->geometry().bottomRight().rx() || event->pos().y() > main_widget->geometry().bottomRight().ry())
                return;
            
        }
        is_press.reset(new QPoint((event->globalPosition() - this->frameGeometry().topLeft()).toPoint()));
        width_size = this->size();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(is_max) return;
    if (is_press) {
        if (mouse_state == M_NONE)
            this->move((event->globalPosition() - *is_press).toPoint()); //窗口移动
        else
        {
            auto d = (event->globalPosition() - this->frameGeometry().topLeft()).toPoint() - *is_press;
            if(mouse_state & M_LEFT) {
                this->move(event->globalPosition().x() - is_press->x(), this->frameGeometry().y());
                this->resize(-d.x() + this->width(), this->height());
            }
            if(mouse_state & M_TOP) {
                this->move(this->frameGeometry().x(), event->globalPosition().y() - is_press->y());
                this->resize(this->width(), -d.y() + this->height());
            }
            if(mouse_state & M_RIGHT) {
                this->resize(d.x() + width_size.width(), this->height());
            }
            if(mouse_state & M_BOTTOM) {
                this->resize(this->width(), d.y() + width_size.height());
            }
        }

    }
   else {
        mouse_state = M_NONE;
        if (abs(event->pos().y() - main_widget->pos().y() - main_widget->height()) < 5)
            mouse_state |= M_BOTTOM;
        if (abs(event->pos().y() - main_widget->pos().y()) < 5)
            mouse_state |= M_TOP;
        if (abs(event->pos().x() - main_widget->pos().x()) < 5)
            mouse_state |= M_LEFT;
        if (abs(event->pos().x() - main_widget->pos().x() - main_widget->width()) < 5)
            mouse_state |= M_RIGHT;
        if (mouse_state == last_mouse_s) return;
        last_mouse_s = mouse_state;
        switch (mouse_state) {
            case M_NONE:
                setCursor(Qt::ArrowCursor); break;

            case M_TOP:
            case M_BOTTOM:
                setCursor(Qt::SizeVerCursor); break;

            case M_LEFT:
            case M_RIGHT:
                setCursor(Qt::SizeHorCursor); break;

            case M_TOP|M_RIGHT:
            case M_BOTTOM|M_LEFT:
                setCursor(Qt::SizeBDiagCursor); break;

            case M_TOP|M_LEFT:
            case M_BOTTOM|M_RIGHT:
                setCursor(Qt::SizeFDiagCursor); break;
        }
    }

}