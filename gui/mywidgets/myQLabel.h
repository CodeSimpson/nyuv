#ifndef __MYQLABEL_H__
#define __MYQLABEL_H__

#include "defines.h"

#include <QLabel>
#include <QWheelEvent>

class myQLabel : public QLabel
{
public:
    myQLabel(QWidget *parent) : QLabel(parent){}

    myQLabel(QWidget *parent,
            const char *text,
            const Qt::Alignment &align = Qt::AlignCenter,
            const int &fsize = 8) : QLabel(text, parent)
    {
        QFont font("黑体", fsize, QFont::Normal);
        setAlignment(align);
        setFont(font);
        setMinimumSize(QSize(1, 1));
        setBaseSize(QSize(1, 1));
        setMargin(0);
    }

    ~myQLabel(){}

    const QPoint &pos(){
        return m_pt;
    }
    const BOOL empty(){
        return (size() == QSize(0, 0));
    }

protected:
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE
    {
        m_pt = e->pos();
    }

    // void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE
    // {
    //    qDebug() << e->pos();
    // }

private:
    QPoint m_pt;
};

#endif