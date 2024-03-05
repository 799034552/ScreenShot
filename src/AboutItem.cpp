#include<screenshot/AboutItem.h>
#include<QLayout>
#include<QLabel>
AboutItem::AboutItem(QString key, QString val)
{
    QHBoxLayout *layout = new QHBoxLayout();
    QLabel *_key = new QLabel(key);
    layout->setContentsMargins(10,7,10,7);
    _key->setStyleSheet("QLabel{margin:0;font-size:16px;background-color:rgba(0,0,0,0);  color:black; font-family: Corbel;}");
    QLabel *_val = new QLabel(val);
    _val->setStyleSheet("QLabel{margin:0;font-size:16px;background-color:rgba(0,0,0,0); color:#5c5c5c; font-family: Corbel;}");
    layout->addWidget(_key);
    layout->addStretch();
    layout->addWidget(_val);
    this->setLayout(layout);
    this->setStyleSheet("QWidget{background-color:rgb(255,255,255);border-style:none; border-radius: 5px;}\nQWidget:hover{background-color:rgb(245,245,245); border-radius: 5px;}");
//    this->setStyleSheet("background-color:rgb(245,245,245);border:1px solid black; border-radius: 5px;");

}
