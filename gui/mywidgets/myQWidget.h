#ifndef __MYQWIDGET__
#define __MYQWIDGET__

#include <QDebug>
#include <QWidget>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QFrame>

#include "mainwindow.h"

class myQWidget : public QFrame
{
public:
    myQWidget(QWidget *parent) : QFrame(parent)
    {
        mp_parent = (MainWindow *)parent;
        setBackgroundRole(QPalette::Dark);
        setFrameShape(QFrame::Box);
        setFrameShadow(QFrame::Sunken);
    }
    ~myQWidget() {}

protected:
    // void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;
    // void dropEvent(QDropEvent *) Q_DECL_OVERRIDE;
private:
    MainWindow *mp_parent;
};


#endif