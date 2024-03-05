#include <QApplication>
#include <screenshot/MainWindow.h>

int main(int argc, char *argv[]) {
    setbuf(stdout, nullptr);
    QApplication app(argc, argv); // 创建QApplication实例

    MainWindow w; // 创建QMainWindow实例
    w.setWindowTitle("Qt Application Example");
    // w.setWindowFlag(Qt::FramelessWindowHint); //设置为无边框
    // w.setAttribute(Qt::WA_TranslucentBackground); //没有设置背景的给他变为透明的
    w.show(); // 显示主窗口

    return app.exec(); // 进入事件循环
}