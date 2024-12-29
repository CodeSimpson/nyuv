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
        pLayout->setSpacing(2);
        pLayout->setContentsMargins(5, 5, 5, 5);
        setContentsMargins(0, 0, 0, 0);
        setAutoFillBackground(true);
        setFrameShape(QFrame::Box);
        setFrameShadow(QFrame::Sunken);
        setLayout(pLayout);
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