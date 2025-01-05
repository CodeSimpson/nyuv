#ifndef __MYQSCROLLAREA__
#define __MYQSCROLLAREA__

#include <QScrollArea>
#include <QDebug>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QWidget>
#include <QDragEnterEvent>
#include <QMimeData>

#include "mainwindow.h"
#include "myQLabel.h"

/************* 提供窗口组件的滚动视图 **************/
class myQScrollArea : public QScrollArea
{
public:
    myQScrollArea(QWidget *parent) : QScrollArea(parent)
    {
        mp_parent = (MainWindow *)parent;
        setBackgroundRole(QPalette::Dark);
    }
    ~myQScrollArea() {}

protected:
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent *e) Q_DECL_OVERRIDE;

private:
    QPoint m_presspos;
    bool m_ispressed;
    MainWindow *mp_parent;
};

void myQScrollArea::mouseMoveEvent(QMouseEvent *e)
{
    if (!m_ispressed)
    {
        return;
    }

    QPoint pt = e->pos();

    int dx = m_presspos.x() - pt.x();
    int dy = m_presspos.y() - pt.y();
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + dx);
    verticalScrollBar()->setValue(verticalScrollBar()->value() + dy);
    m_presspos = pt;
}
void myQScrollArea::mousePressEvent(QMouseEvent *e)
{
    setCursor(Qt::ClosedHandCursor);

    m_ispressed = true;
    m_presspos = e->pos();
}
void myQScrollArea::mouseReleaseEvent(QMouseEvent *e)
{
    setCursor(Qt::ArrowCursor);

    Q_UNUSED(e);
    m_ispressed = false;

    m_presspos.setX(0);
    m_presspos.setY(0);
}
void myQScrollArea::wheelEvent(QWheelEvent *e)
{
    myQLabel *label = (myQLabel *)widget();
    if (label->empty())
    {
        return;
    }

    QSize size_last = label->size();
    QPoint label_pos_last = label->pos();

    if (e->delta() < 0)
    {
       mp_parent->toLessenActionTriggered();
    }
    else if (e->delta() > 0)
    {
       mp_parent->toEnlargeActionTriggered();
    }

    QSize size_now = label->size();
    QPoint label_pos_now = label->pos();
    QPoint target_pos = QPoint(size_now.width() * label_pos_last.x() / size_last.width(), size_now.height() * label_pos_last.y() / size_last.height());
    QPoint dy_pos = target_pos - label_pos_now;

    if (dy_pos != QPoint(0, 0))
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + dy_pos.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() + dy_pos.y());
    }
}

#endif