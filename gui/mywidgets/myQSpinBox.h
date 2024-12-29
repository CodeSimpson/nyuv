#ifndef __MYSPINBOX_H__
#define __MYSPINBOX_H__

#include <QSpinBox>

class mySpinBox : public QSpinBox
{
public:
    mySpinBox(QWidget *parent) : QSpinBox(parent){}

    mySpinBox(QWidget *parent, 
            const int &step = 1,
            const int &min = 0,
            const int &max = 100,
            const int &value = 0,
            const char *shuffix="") : QSpinBox(parent)
    {
        setSingleStep(step);
        setMinimum(min);
        setMaximum(max);
        setValue(value);
        setSuffix(shuffix);

        setFixedHeight(22);
    }

    ~mySpinBox(){}
};

#endif