#include <QDebug>
#include <QWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QApplication>
#include <QDesktopWidget>

#include "myQLabel.h"
#include "myQSpinBox.h"
#include "myQSlider.h"
#include "myPrefer.h"

#undef __LOGTAG__
#define __LOGTAG__ "PREFER"
#include "log.h"

myPrefer::myPrefer(const QString &title) : QWidget(nullptr)
{
    setFixedSize(460, 400);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowTitle(title);
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);
    QDesktopWidget *desktop = QApplication::desktop();
    setFixedSize(size());
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    setting();

    show();
}

void myPrefer::setting()
{
    m_cancel_btn = new QPushButton(tr("取消"), this);
    m_apply_btn = new QPushButton(tr("应用"), this);
    connect(m_cancel_btn, SIGNAL(clicked()), this, SLOT(OnCancelBtnClicked()));
    connect(m_apply_btn, SIGNAL(clicked()), this, SLOT(OnApplyBtnClicked()));
    m_cancel_btn->setGeometry(240, 360, 100, 32);
    m_apply_btn->setGeometry(350, 360, 100, 32);

    auto lable_zoom = new myQLabel(this, "默认zoom", Qt::AlignRight, 10);
    m_edit_zoom = new mySpinBox(this, 1, 1, 500, 100, "%");
    m_edit_zoom->setFixedWidth(60);

    QGridLayout *pLayout = new QGridLayout();
    pLayout->addWidget(lable_zoom, 1, 1, 1, 1);
    pLayout->addWidget(m_edit_zoom, 1, 2, 1, 2);

    setLayout(pLayout);
}

void myPrefer::OnApplyBtnClicked()
{
    LOGI("prefer changed.");
}

void myPrefer::OnCancelBtnClicked()
{
    LOGI("nothing changed.");
    delete this;
}