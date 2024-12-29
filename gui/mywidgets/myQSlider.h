#ifndef __MYQSLIDER_H__
#define __MYQSLIDER_H__

#include <QSlider>

class myQSlider : public QSlider
{
public:
    myQSlider(QWidget *parent) : QSlider(parent){}

    myQSlider(QWidget *parent, 
            const int &step = 1,
            const int &min = 0,
            const int &max = 100,
            const int &value = 0) : QSlider(Qt::Horizontal, parent)
    {
        setSingleStep(step);
        setMinimum(min);
        setMaximum(max);
        setValue(value);
        setFixedHeight(22);
    }

    ~myQSlider(){}
};

#endif