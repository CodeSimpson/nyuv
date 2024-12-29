/*
 * @Author: your name
 * @Date: 2020-09-01 11:01:09
 * @LastEditTime: 2020-09-01 14:48:48
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /nyuv/gui/mywidgets/myPrefer.h
 */
#ifndef __MYPREFER__
#define __MYPREFER__

#include <QDebug>
#include <QWidget>
#include <QtWidgets/QPushButton>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QApplication>
#include <QDesktopWidget>

#include "myQSpinBox.h"

class myPrefer : public QWidget
{
    Q_OBJECT
public:
    explicit myPrefer(const QString &title);

protected:
private:
    void setting();

private slots:
    void OnApplyBtnClicked();
    void OnCancelBtnClicked();

private:
    QPushButton *m_cancel_btn;
    QPushButton *m_apply_btn;
    
    mySpinBox *m_edit_zoom;
};

#endif