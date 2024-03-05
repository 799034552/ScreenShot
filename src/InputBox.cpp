#include<screenshot/InputBox.h>
#include<QLayout>
InputBox::InputBox(float _radius,const QString &PlaceholderText ,const qreal border_margin,const int &_font_size):radius(_radius),font_size(_font_size)
{
    setAttribute(Qt::WA_StyledBackground);
    radius = 10;
    QVBoxLayout *layout = new QVBoxLayout();
    background = new QWidget(this);
    background->lower();
    background->setStyleSheet("background-color: #74b9ff");

    editor = new cus_QLineEdit();
    layout->addWidget(editor);
    editor->setAlignment(Qt::AlignCenter);
    this->setLayout(layout);
    this->setStyleSheet(QString::asprintf("border-radius: %fpx;", radius));
    layout->setContentsMargins(border_margin,border_margin,border_margin,border_margin);
    editor->setStyleSheet(QString::asprintf("font-family: Corbel;font-size:%dpx;border: 2px solid rgb(100,100,100);background-color: rgb(255,255,255); border-radius: %fpx;", font_size, radius));
    //editor->setStyleSheet(QString::asprintf("font-family: Corbel;font-size:15px;border: 2px solid #0984e3;background-color: rgb(255,255,255); border-radius: %fpx;", radius));
    editor->setAlignment(Qt::AlignLeft);
    editor->setTextMargins(5,3,5,3);
    editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    editor->setPlaceholderText(PlaceholderText);

    //设置动画
    slide_in = new QPropertyAnimation(this, "m_bg_width");
    slide_in->setDuration(200);
    slide_in->setEndValue(1);
    slide_out = new QPropertyAnimation(this, "m_bg_width");
    slide_out->setDuration(200);
    slide_out->setEndValue(0);
    connect(editor, &cus_QLineEdit::mouse_enter, this, [=]{
        slide_in->setStartValue(bg_width);
        slide_in->start();
    });
    connect(editor, &cus_QLineEdit::mouse_leave, this, [=]{
        slide_out->setStartValue(bg_width);
        slide_out->start();
    });
    connect(this, &InputBox::bg_width_change, this, [=] {
        background->resize(this->width() * bg_width, this->height());
    });
    connect(editor, &cus_QLineEdit::focus_in, this, [=]{
        editor->setStyleSheet(QString::asprintf("font-family: Corbel;font-size:%dpx;border: 2px solid #0984e3;background-color: rgb(255,255,255); border-radius: %fpx;", font_size, radius));
    });
    connect(editor, &cus_QLineEdit::focus_out, this, [=]{
        editor->setStyleSheet(QString::asprintf("font-family: Corbel;font-size:%dpx;border: 2px solid rgb(100,100,100);background-color: rgb(255,255,255); border-radius: %fpx;", font_size, radius));

    });
    connect(editor, &QLineEdit::textChanged, this, &InputBox::text_change);

    bg_width = 0;
    emit bg_width_change();
}


void InputBox::resizeEvent(QResizeEvent *event)
{
    background->resize(background->width(), this->height());
}

void cus_QLineEdit::enterEvent(QEnterEvent *event)
{
    emit mouse_enter();
}
void cus_QLineEdit::leaveEvent(QEvent* event)
{
    emit mouse_leave();
}
void cus_QLineEdit::focusInEvent(QFocusEvent *e)
{
    emit focus_in();
    return QLineEdit::focusInEvent(e);
}
void cus_QLineEdit::focusOutEvent(QFocusEvent *e)
{
    emit focus_out();
    return QLineEdit::focusOutEvent(e);
}
