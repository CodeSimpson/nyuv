#include "tinyxml2.h"
#include "mainwindow.h"
#include "autointerface.h"
#include "myQWidget.h"
#include "myQScrollArea.h"
#include "myQLabel.h"
#include "update.h"
#include "myPrefer.h"

#undef __LOGTAG__
#define __LOGTAG__ "MAINWINDOW"
#include "log.h"

#include <QWidget>
#include <QImage>
#include <QFileDialog>
#include <QPixmap>
#include <QAction>
#include <QMessageBox>
#include <QDebug>
#include <QScrollArea>
#include <QGridLayout>
#include <QErrorMessage>
#include <QApplication>
#include <QStandardPaths>
#include <QDragEnterEvent>
#include <QFrame>
#include <QDesktopServices>

#include <iostream>
#include <vector>

using namespace tinyxml2;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    std::vector<std::string> pathes = autocomplete_load(CONFIG_TYPE::PATH);
    m_home = QString::fromStdString(pathes[0]);

    /* load format list */
    loadRawSet();

    /* init main window */
    initMainWindow();

    /* init ui */
    initUiComponent();

    /* init resource */
    initImageResource();

    /* create m_imageviewer */
    m_imageviewer = new QImageViewer();
    m_imageviewer_list.append(m_imageviewer);

    mp_update->check(true);

    m_tmpviewer = nullptr;
}

void MainWindow::initImageResource(void)
{
    mp_label_image->clear();
    setWindowTitle(tr("nyuv"));
}

static QString lessStr(QString str, int size)
{
    // return QFileInfo(str).fileName().mid(0, size) + "...";
    return str;
}

void MainWindow::loadImageResource(const bool &isauto)
{
    if (isauto)
    {
        if (m_imageviewer_list.size() <= 1 ||
            (m_imageviewer_list.size() > 1 &&
             (!m_imageviewer->isSameFormat(m_tmpviewer) ||
              !(m_imageviewer->rawinfo == m_tmpviewer->rawinfo))))
        {
            m_setting->parse(m_imageviewer->filename);
        }
        settingTriggered(m_setting->value());
    }
    else
    {
        m_setting->updateFit();
        mp_label_image->setPixmap(m_imageviewer->pixmap);
        mp_label_image->resize(m_imageviewer->size);
        QString filename_title = QFileInfo(m_imageviewer->filename).fileName();
        if (filename_title.isEmpty())
        {
            filename_title = tr("nyuv");
        }
        else
        {
            filename_title += tr(" - nyuv");
        }
        setWindowTitle(filename_title);
        mp_widget_tab->setTabText(mp_widget_tab->currentIndex(), lessStr(m_imageviewer->filename, 20));
        mp_widget_tab->setTabIcon(mp_widget_tab->currentIndex(), QIcon(m_imageviewer->pixmap));
    }
}

int MainWindow::getAL()
{
    return 150;
}

void MainWindow::setLight(const int &l)
{
    RESULT ret = m_imageviewer->setLight(l);
    if (ret != RESULT::SUCCESS){
        return;
    }
    loadImageResource(false);
}

void MainWindow::openActionTriggered(void)
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Select image:"), m_home, tr("Images (*)"));
    fileTreeTriggered(filename);
}

void MainWindow::addToAction(void)
{
    m_tmpviewer = m_imageviewer;
    m_imageviewer = new QImageViewer(this);
    m_imageviewer_list.append(m_imageviewer);
    if (mp_widget_tab->count() < 1)
    {
        mp_widget_tab->addTab("default");
    }
    mp_widget_tab->addTab("default");
    if (!m_imageviewer_list[0]->filename.isEmpty())
        mp_widget_tab->setTabText(0, lessStr(m_imageviewer_list[0]->filename, 20));
    mp_widget_tab->setCurrentIndex(mp_widget_tab->count() - 1);
    mp_widget_tab->setTabIcon(0, QIcon(m_imageviewer_list[0]->pixmap));

    m_imageviewer->setZoom(m_tmpviewer->getZoom());
    m_imageviewer->justSetRaw(m_tmpviewer->rawinfo);
    m_imageviewer->justSetAngle(m_tmpviewer->angle);
    m_setting->setInfo(m_imageviewer->rawinfo);
    m_setting->setStats(m_imageviewer->getStats());
    m_setting->setZoom(m_imageviewer->getZoom());
}

void MainWindow::preferAction(void)
{
    QWidget *prefer = new myPrefer("偏好设置");
}

void MainWindow::tabCloseAction(int index)
{
    int origin_index = mp_widget_tab->currentIndex();
    int origin_count = mp_widget_tab->count() - 1;
    mp_widget_tab->removeTab(index);
    SAFEDEL(m_imageviewer_list[index]);
    m_imageviewer_list.removeAt(index);
    if ((origin_index <= index) && (origin_index < origin_count))
    {
        tabChoseAction(origin_index);
    }
    else
    {
        tabChoseAction(origin_index - 1);
    }
}

void MainWindow::tabChoseAction(int index)
{
    m_imageviewer = m_imageviewer_list[index];
    m_setting->setZoom(m_imageviewer->getZoom());
    loadImageResource(false);
    m_setting->setInfo(m_imageviewer->rawinfo);
    m_setting->setStats(m_imageviewer->getStats());
}

void MainWindow::tabMoveAction(int from, int to)
{
    m_imageviewer_list.move(from, to);
    tabChoseAction(to);
}

void MainWindow::openFolderActionTriggered(void)
{
    QFileDialog *fd = new QFileDialog(this);
    fd->setFileMode(QFileDialog::DirectoryOnly);
    if (fd->exec() == QFileDialog::Accepted)
    {
        auto dir = fd->directory();
        m_filetree->setRootPath(dir);
    }
    delete fd;
}

void MainWindow::saveActionTriggered(void)
{
    if (m_imageviewer->isEmpty())
    {
        QMessageBox::critical(this,
                              tr("错误"),
                              tr("非法文件!"));
        return;
    }
    RESULT ret = m_imageviewer->saveAs();
}

void MainWindow::closeActionTriggered(void)
{
    initImageResource();
    m_imageviewer->closeImageFile();
    mp_widget_tab->setTabText(mp_widget_tab->currentIndex(), "");
    mp_widget_tab->setTabIcon(mp_widget_tab->currentIndex(), QIcon());
}

void MainWindow::setResetAction(void)
{
    m_setting->resetFit(Qt::Unchecked);
    m_setting->setZoom(100);
    m_imageviewer->resetALL();
    loadImageResource();
}

void MainWindow::updateAction(void)
{
    mp_update->check();
}

void MainWindow::helpAction(void)
{
    QMessageBox message(QMessageBox::Question,
                        tr("帮助"),
                        tr("将会打开浏览器\n是否打开帮助?"),
                        QMessageBox::Help | QMessageBox::No,
                        NULL);
    if (message.exec() == QMessageBox::Help)
    {
        QDesktopServices::openUrl(QUrl("https://git.n.xiaomi.com/caibingcheng/nyuv/-/wikis/pages"));
    }
}

void MainWindow::lastActionTriggered(void)
{
    RESULT ret = m_imageviewer->last();
    if (ret != RESULT::SUCCESS)
    {
        mp_label_image->clear();
        QMessageBox::critical(this,
                              tr("错误"),
                              tr(parseResult(ret).c_str()));
        return;
    }

    // loadImageResource();
    m_filetree->last();
}

void MainWindow::nextActionTriggered(void)
{
    RESULT ret = m_imageviewer->next();
    if (ret != RESULT::SUCCESS)
    {
        mp_label_image->clear();
        QMessageBox::critical(this,
                              tr("错误"),
                              tr(parseResult(ret).c_str()));
        return;
    }

    // loadImageResource();
    m_filetree->next();
}

void MainWindow::toLeftActionTriggered(void)
{
    if (m_imageviewer->isEmpty())
        return;
    RESULT ret = m_imageviewer->spinToLeft();
    if (ret != RESULT::SUCCESS)
    {
        mp_label_image->clear();
        QMessageBox::critical(this,
                              tr("错误"),
                              tr(parseResult(ret).c_str()));
        return;
    }

    loadImageResource(false);
}

void MainWindow::toRightActionTriggered(void)
{
    if (m_imageviewer->isEmpty())
        return;
    RESULT ret = m_imageviewer->spinToRight();
    if (ret != RESULT::SUCCESS)
    {
        mp_label_image->clear();
        QMessageBox::critical(this,
                              tr("错误"),
                              tr(parseResult(ret).c_str()));
        return;
    }

    loadImageResource(false);
}

void MainWindow::flipActionTriggered()
{
    if (m_imageviewer->isEmpty())
        return;
    RESULT ret = m_imageviewer->flip();
    if (ret != RESULT::SUCCESS)
    {
        mp_label_image->clear();
        QMessageBox::critical(this,
                              tr("错误"),
                              tr(parseResult(ret).c_str()));
        return;
    }

    loadImageResource(false);
}

void MainWindow::setZoomAction(const int &zoom)
{
    if (m_imageviewer->isEmpty())
        return;

    m_setting->resetFit(Qt::Unchecked);
    RESULT ret = m_imageviewer->setZoom(zoom);
    if (ret != RESULT::SUCCESS)
    {
        mp_label_image->clear();
        QMessageBox::critical(this,
                              tr("错误"),
                              tr(parseResult(ret).c_str()));
        return;
    }

    loadImageResource(false);
}

void MainWindow::setBP(const bool &bp)
{
    if (m_imageviewer->isEmpty())
        return;
    RESULT ret = m_imageviewer->setBP(bp);
    if (ret != RESULT::SUCCESS)
    {
        mp_label_image->clear();
        QMessageBox::critical(this,
                              tr("错误"),
                              tr(parseResult(ret).c_str()));
        return;
    }

    loadImageResource(false);
}

void MainWindow::toEnlargeActionTriggered(void)
{
    if (m_imageviewer->isEmpty())
        return;

    m_setting->setZoom(m_imageviewer->setAGetZoom(1));
    setZoomAction(getZoom());
}

void MainWindow::toLessenActionTriggered(void)
{
    if (m_imageviewer->isEmpty())
        return;

    m_setting->setZoom(m_imageviewer->setAGetZoom(-1));
    setZoomAction(getZoom());
}

void MainWindow::deleteActionTriggered(void)
{
    if (!QFile(m_imageviewer->filename).exists())
    {
        return;
    }

    QMessageBox message(QMessageBox::Warning,
                        tr("删除"),
                        tr("是否删除") + m_imageviewer->filename + tr(" ?"),
                        QMessageBox::Yes | QMessageBox::No,
                        NULL);
    if (message.exec() == QMessageBox::No)
    {
        return;
    }

    RESULT ret = m_imageviewer->delImageFile();
    if (ret != RESULT::SUCCESS)
    {
        QMessageBox::critical(this,
                              tr("失败"),
                              tr("无法删除!"));
        return;
    }

    initImageResource();
}

void MainWindow::setQImageViewerWidget(void)
{
    /* label show image */
    mp_label_image = new myQLabel(this);
    myQScrollArea *imageScrollArea = new myQScrollArea(this);
    imageScrollArea->setAlignment(Qt::AlignCenter);                 // 设置中心对齐，默认为左上角对齐
    imageScrollArea->setFrameShape(QFrame::NoFrame);
    imageScrollArea->setWidget(mp_label_image);

    QGridLayout *mainLayout = new QGridLayout();
    mainLayout->setSpacing(1);                                      // 设置布局中控件之间的间距
    mainLayout->setMargin(2);                                       // 设置布局边缘与父控件之间的间距
    mainLayout->addWidget(mp_widget_tab, 0, 0);                     // QGridLayout::addWidget()并不会发生所有权的转移。
    mainLayout->addWidget(imageScrollArea, 1, 0);
    mp_widget_central->setLayout(mainLayout);                       // QWidget::setLayout()会转移所有权，一个QWidget组件只能拥有一个布局管理器，如果mp_widget_central已经安装了一个布局管理器，则不被允许安装另一个

    mp_widget_tab->setTabsClosable(true);
    mp_widget_tab->setAutoHide(true);
    mp_widget_tab->setMovable(true);
    mp_widget_tab->setChangeCurrentOnDrag(false);
    mp_widget_tab->setIconSize(QSize(32, 24));
    mp_widget_tab->setElideMode(Qt::ElideLeft);                     // 如何隐藏标签栏的文本
    mp_widget_tab->setShape(QTabBar::RoundedNorth);                 // 设置标签栏的形状
}

RESULT MainWindow::loadRawSet()
{
    RESULT ret = RESULT::SUCCESS;

    XMLDocument doc;
    doc.LoadFile(LIBLIST);

    //开始解析xml
    XMLElement *nodelist = doc.RootElement();

    XMLElement *node = nodelist->FirstChildElement("NODE");
    while (node)
    {
        QString source = node->FirstChildElement("source")->GetText();

        m_rawset.append(source);

        node = node->NextSiblingElement();
    }
    return ret;
}

void MainWindow::setWindowComponet(void)
{
    mp_action_open = new QAction(tr("打开"), this);
    mp_action_open->setShortcut(QKeySequence::Open);
    mp_action_open->setStatusTip(tr("打开文件"));
    mp_action_open->setIcon(QIcon(":/statics/add.png"));

    mp_action_add = new QAction(tr("添加视窗"), this);
    mp_action_add->setStatusTip(tr("添加视窗"));
    mp_action_add->setIcon(QIcon(":/statics/addTo.png"));

    mp_action_folder = new QAction(tr("打开文件夹"), this);
    mp_action_folder->setStatusTip(tr("打开文件夹"));
    mp_action_folder->setIcon(QIcon(":/statics/open.png"));

    mp_action_save = new QAction(tr("另存为"), this);
    mp_action_save->setShortcut(QKeySequence::SaveAs);
    mp_action_save->setStatusTip(tr("另存为..."));
    mp_action_save->setIcon(QIcon(":/statics/save.png"));

    mp_action_close = new QAction(tr("关闭"), this);
    mp_action_close->setShortcut(QKeySequence::Close);
    mp_action_close->setStatusTip(tr("关闭文件"));
    mp_action_close->setIcon(QIcon(":/statics/close.png"));

    mp_action_last = new QAction(tr("上一个"), this);
    mp_action_last->setStatusTip(tr("上一个"));
    mp_action_last->setIcon(QIcon(":/statics/left.png"));

    mp_action_next = new QAction(tr("下一个"), this);
    mp_action_next->setStatusTip(tr("下一个"));
    mp_action_next->setIcon(QIcon(":/statics/right.png"));

    mp_action_toleft = new QAction(tr("左转"), this);
    mp_action_toleft->setStatusTip(tr("左转"));
    mp_action_toleft->setIcon(QIcon(":/statics/toLeft.png"));

    mp_action_toright = new QAction(tr("右转"), this);
    mp_action_toright->setStatusTip(tr("右转"));
    mp_action_toright->setIcon(QIcon(":/statics/toRight.png"));

    mp_action_flip = new QAction(tr("镜像"), this);
    mp_action_flip->setStatusTip(tr("镜像"));
    mp_action_flip->setIcon(QIcon(":/statics/flip.png"));

    mp_action_enlarge = new QAction(tr("放大"), this);
    mp_action_enlarge->setStatusTip(tr("放大"));
    mp_action_enlarge->setIcon(QIcon(":/statics/large.png"));

    mp_action_lessen = new QAction(tr("缩小"), this);
    mp_action_lessen->setStatusTip(tr("缩小"));
    mp_action_lessen->setIcon(QIcon(":/statics/small.png"));

    mp_action_delete = new QAction(tr("删除"), this);
    mp_action_delete->setStatusTip(tr("删除"));
    mp_action_delete->setIcon(QIcon(":/statics/clear.png"));
    mp_action_delete->setShortcut(QKeySequence::Delete);

    QAction *exitAction = new QAction(tr("退出"), this);
    exitAction->setStatusTip(tr("退出"));
    exitAction->setIcon(QIcon(":/statics/quit.png"));
    exitAction->setShortcut(QKeySequence::Quit);

    mp_action_reset = new QAction(tr("重置"), this);
    mp_action_reset->setStatusTip(tr("重置"));
    mp_action_reset->setIcon(QIcon(":/statics/reset.png"));

    mp_action_update = new QAction(tr("检查更新"), this);
    mp_action_update->setStatusTip(tr("检查更新"));
    mp_action_update->setIcon(QIcon(":/statics/update.png"));

    mp_action_help = new QAction(tr("使用帮助"), this);
    mp_action_help->setStatusTip(tr("使用帮助"));
    mp_action_help->setIcon(QIcon(":/statics/help.png"));

    mp_action_prefer = new QAction(tr("偏好"), this);
    mp_action_prefer->setStatusTip(tr("偏好"));
    mp_action_prefer->setIcon(QIcon(":/statics/prefer.png"));

    QMenu *fileMenu = mp_bar_menu->addMenu(tr("文件"));                         // 将带有标题的新QMenu附加到菜单栏，菜单栏将拥有该菜单的所有权，返回新菜单
    fileMenu->addAction(mp_action_open);
    fileMenu->addAction(mp_action_folder);
    fileMenu->addAction(mp_action_save);
    fileMenu->addSeparator();
    fileMenu->addAction(mp_action_close);
    fileMenu->addAction(mp_action_delete);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);
    mp_bar_menu->addSeparator();

    QMenu *operationMenu = mp_bar_menu->addMenu(tr("操作"));
    operationMenu->addAction(mp_action_add);
    operationMenu->addSeparator();
    operationMenu->addAction(mp_action_last);
    operationMenu->addAction(mp_action_next);
    operationMenu->addSeparator();
    operationMenu->addAction(mp_action_toleft);
    operationMenu->addAction(mp_action_toright);
    operationMenu->addAction(mp_action_flip);
    operationMenu->addSeparator();
    operationMenu->addAction(mp_action_enlarge);
    operationMenu->addAction(mp_action_lessen);
    operationMenu->addAction(mp_action_reset);
    operationMenu->addSeparator();
    mp_bar_menu->addSeparator();

    // QMenu *configMenu = mp_bar_menu->addMenu(tr("设置"));
    // configMenu->addAction(mp_action_prefer);
    // mp_bar_menu->addSeparator();

    QMenu *helpMenu = mp_bar_menu->addMenu(tr("帮助"));
    helpMenu->addAction(mp_action_help);
    helpMenu->addSeparator();
    helpMenu->addAction(mp_action_update);

    mp_bar_tool->addAction(mp_action_open);
    mp_bar_tool->addAction(mp_action_folder);
    mp_bar_tool->addAction(mp_action_save);
    mp_bar_tool->addAction(mp_action_close);
    mp_bar_tool->addSeparator();
    mp_bar_tool->addAction(mp_action_add);
    mp_bar_tool->addSeparator();
    mp_bar_tool->addAction(mp_action_last);
    mp_bar_tool->addAction(mp_action_next);
    mp_bar_tool->addSeparator();
    mp_bar_tool->addAction(mp_action_toleft);
    mp_bar_tool->addAction(mp_action_toright);
    mp_bar_tool->addAction(mp_action_flip);
    mp_bar_tool->addAction(mp_action_enlarge);
    mp_bar_tool->addAction(mp_action_lessen);
    mp_bar_tool->addAction(mp_action_reset);
    mp_bar_tool->addSeparator();
    mp_bar_tool->addAction(mp_action_delete);
    mp_bar_tool->setMovable(false);

    connect(mp_action_open, SIGNAL(triggered(bool)), this, SLOT(openActionTriggered()));
    connect(mp_action_folder, SIGNAL(triggered(bool)), this, SLOT(openFolderActionTriggered()));
    connect(mp_action_save, SIGNAL(triggered(bool)), this, SLOT(saveActionTriggered()));
    connect(mp_action_close, SIGNAL(triggered(bool)), this, SLOT(closeActionTriggered()));
    connect(mp_action_last, SIGNAL(triggered(bool)), this, SLOT(lastActionTriggered()));
    connect(mp_action_next, SIGNAL(triggered(bool)), this, SLOT(nextActionTriggered()));
    connect(mp_action_toleft, SIGNAL(triggered(bool)), this, SLOT(toLeftActionTriggered()));
    connect(mp_action_toright, SIGNAL(triggered(bool)), this, SLOT(toRightActionTriggered()));
    connect(mp_action_flip, SIGNAL(triggered(bool)), this, SLOT(flipActionTriggered()));
    connect(mp_action_enlarge, SIGNAL(triggered(bool)), this, SLOT(toEnlargeActionTriggered()));
    connect(mp_action_lessen, SIGNAL(triggered(bool)), this, SLOT(toLessenActionTriggered()));
    connect(mp_action_delete, SIGNAL(triggered(bool)), this, SLOT(deleteActionTriggered()));
    connect(mp_action_reset, SIGNAL(triggered(bool)), this, SLOT(setResetAction()));
    connect(mp_action_update, SIGNAL(triggered(bool)), this, SLOT(updateAction()));
    connect(mp_action_help, SIGNAL(triggered(bool)), this, SLOT(helpAction()));
    connect(mp_action_add, SIGNAL(triggered(bool)), this, SLOT(addToAction()));
    connect(mp_action_prefer, SIGNAL(triggered(bool)), this, SLOT(preferAction()));
    connect(mp_widget_tab, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseAction(int)));
    connect(mp_widget_tab, SIGNAL(currentChanged(int)), this, SLOT(tabChoseAction(int)));
    connect(mp_widget_tab, SIGNAL(tabMoved(int, int)), this, SLOT(tabMoveAction(int, int)));
    connect(exitAction, SIGNAL(triggered(bool)), this, SLOT(close()));

    connect(m_filetree, SIGNAL(fileItemDoubleClicked(const QString &)), this, SLOT(fileTreeTriggered(const QString &)));
    connect(m_setting, SIGNAL(processBtnClicked(const IMAGEINFO &)), this, SLOT(settingTriggered(const IMAGEINFO &)));

    m_filetree->setCursor(QCursor(Qt::PointingHandCursor));             // 设置文件树组件的光标形状，这里设置为手型
    m_filetree->setMouseTracking(true);

    QGridLayout *layout_left = new QGridLayout();
    layout_left->addWidget(m_filetree->initPathBox(), 0, 0, 1, 1);      // initPathBox返回框架组件，就是文件目录上面那部分
    layout_left->addWidget(m_filetree, 1, 0, 1, 1);
    mp_bar_left->addWidget(new myInfoBarQFrame(layout_left, this));     // 左工具栏添加文件目录框架，包含文件目录栏和文件目录编辑栏
    mp_bar_right->addWidget(m_setting);
    mp_bar_right->setFixedWidth(180);
}

void MainWindow::filetreeShowAction(QString &str)
{
    mp_bar_left->setStatusTip(str);                                     // 更新左工具栏的状态提示
    string home = str.toStdString();
    std::vector<std::string> pathes;
    pathes.emplace_back(home);
    autocomplete_dump(pathes, CONFIG_TYPE::PATH);
}

void MainWindow::fileTreeTriggered(const QString &filename)
{
    RESULT ret = m_imageviewer->openImageFile(filename);
    if (ret != RESULT::SUCCESS)
    {
        mp_label_image->clear();
        QMessageBox::critical(this,
                              tr("错误"),
                              tr(parseResult(ret).c_str()));
        return;
    }

    // 设置自动亮度复选框为未选状态
    m_setting->resetAL(Qt::Unchecked);
    loadImageResource();
}

void MainWindow::settingTriggered(const IMAGEINFO &imgInfo)
{
    RESULT ret = m_imageviewer->setRaw(imgInfo);
    if (ret != RESULT::SUCCESS)
    {
        mp_label_image->clear();
        QMessageBox::critical(this,
                              tr("错误"),
                              tr(parseResult(ret).c_str()));
        return;
    }

    m_setting->resetAL(Qt::Unchecked);
    loadImageResource(false);
}

void MainWindow::initUiComponent(void)
{
    /* set menu and toolbar */
    setWindowComponet();

    /* image show widget */
    setQImageViewerWidget();
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    e->acceptProposedAction();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    loadImageResource(false);
}

void MainWindow::dropEvent(QDropEvent *e)
{
    QList<QUrl> urls = e->mimeData()->urls();
    if (urls.isEmpty())
        return;

    QString filename = urls.first().toLocalFile();
    if (filename.isEmpty())
        return;

    fileTreeTriggered(filename);
}

void MainWindow::initMainWindow(void)
{
    setAcceptDrops(true);                                           // 设置当前QWidget对象是否接受拖放事件
    mp_update = new Update(this);
    // mp_update->check();

    mp_bar_menu = new QMenuBar(this);                               // 构造子对象实例mp_bar_menu，传入父对象指针，管理子对象生命周期
    mp_bar_menu->setAcceptDrops(false);
    setMenuBar(mp_bar_menu);                                        // 将菜单栏mp_bar_menu设置为当前主窗口的菜单栏，没有则当前主窗口不显示菜单栏

    mp_bar_tool = new QToolBar(this);
    mp_bar_tool->setAcceptDrops(false);
    addToolBar(mp_bar_tool);                                        // 将工具栏mp_bar_tool添加到主窗口中，默认在窗口上部，没有则当前主窗口不显示该工具栏mp_bar_tool

    mp_bar_left = new QToolBar(this);
    mp_bar_left->setAcceptDrops(false);
    // addToolBar(Qt::ToolBarArea::LeftToolBarArea, mp_bar_left);

    mp_bar_right = new QToolBar(this);
    mp_bar_right->setAcceptDrops(false);
    addToolBar(Qt::ToolBarArea::RightToolBarArea, mp_bar_right);

    mp_widget_tab = new QTabBar(this);
    mp_widget_central = new myQWidget(this);
    // setCentralWidget(mp_widget_central);

    mp_bar_status = new QStatusBar(this);
    setStatusBar(mp_bar_status);

    mp_bar_left->setStyleSheet("QToolBar {border: 1px solid gray;}");       // 设置QToolBar的样式表，边框设置为灰色，宽度为 1 像素，并且边框样式为实线。
    mp_bar_right->setStyleSheet("QToolBar {border: 1px solid gray;}");
    mp_bar_status->setStyleSheet("QStatusBar {border: 1px solid gray;}");   // 字符串中的 "QStatusBar" 需要和当前类对应，样式表的用法参考：https://doc.qt.io/archives/qt-5.9/stylesheet.html

    mp_splitter_left = new QSplitter(Qt::Horizontal, this);
    mp_splitter_left->addWidget(mp_bar_left);
    mp_splitter_left->addWidget(mp_widget_central);                         // 这里将中心组件和左工具栏都加入分割器中，可以调整俩个窗口的大小
    mp_splitter_left->setStretchFactor(0, 1);
    mp_splitter_left->setStretchFactor(1, 9);
    setCentralWidget(mp_splitter_left);                                     // 设置主窗口的中央部件为mp_splitter_left

    m_setting = new ImageInformationBar(m_rawset, this);
    m_filetree = new LocalFileSystemViewer(m_home, this);
}
