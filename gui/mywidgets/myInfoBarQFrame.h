#ifndef __MYINFOBARQFRAME_H__
#define __MYINFOBARQFRAME_H__

#include <QFrame>
#include <QGridLayout>

class myInfoBarQFrame : public QFrame
{
public:
    myInfoBarQFrame(QGridLayout *pLayout, QWidget *parent)
    : QFrame(parent)
    {
        pLayout->setSpacing(2);                             // 设置布局中控件之间的间距
        pLayout->setContentsMargins(5, 5, 5, 5);            // 设置上下左右边距
        setContentsMargins(0, 0, 0, 0);
        setAutoFillBackground(true);                        // 是否自动填充背景
        setFrameShape(QFrame::Box);
        setFrameShadow(QFrame::Sunken);                     // 设置边框阴影效果 Plain：无阴影效果 Raised：凸起阴影效果 Sunken：凹陷阴影效果
        setLayout(pLayout);                                 // 设置myInfoBarQFrame对象的布局管理器为 pLayout
    }

    myInfoBarQFrame(QWidget *parent)
    : QFrame(parent)
    {
        setContentsMargins(0, 0, 0, 0);
        setAutoFillBackground(true);
        setFrameShape(QFrame::Box);
        setFrameShadow(QFrame::Sunken);
    }
};

#endif