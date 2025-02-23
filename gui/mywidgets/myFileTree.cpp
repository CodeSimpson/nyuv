#include "myFileTree.h"
#include "defines.h"
#include "autointerface.h"
#include "mainwindow.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtGui/QRegExpValidator>
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QFileSystemModel>
#include <QtWidgets/QHeaderView>
#include <QDebug>
#include <QFileIconProvider>

#include <tuple>
#include <utility>

using std::cout;
using std::endl;

class myQFileIconProvider : public QFileIconProvider
{
public:
    myQFileIconProvider(QWidget *parent) : QFileIconProvider() {}

    QIcon icon(const QFileInfo &info) const
    {
        if (info.isDir())
        {
            return QIcon(":/statics/folder.png");
        }
        else
        {
            QString suffix = info.suffix();
            if (suffix == "jpg" || suffix == "bmp" || suffix == "png" || suffix == "gif" || suffix == "jpeg")
            {
                return QIcon(":/statics/image.png");
                // QPixmap pixmap(info.filePath());
                // pixmap.scaled(32, 24);
                // return QIcon(pixmap);
            }
            else
            {
                return QIcon(":/statics/rawImg.png");
            }
        }
    }
};

LocalFileSystemViewer::LocalFileSystemViewer(const QString &rootPath, QWidget *parent)
    : QTreeView(parent), mp_parent(parent), m_pathname(nullptr), m_path_lenght_last(0)
{
    static const char *style = "QTreeView{background-color:white;outline:0px;}"
                               "QTreeView::branch:closed:has-children:!has-siblings,QTreeView::branch:closed:has-children:has-siblings{background-color:white;border-image:transparent;image:url(:/statics/fclose.png);}"
                               "QTreeView::branch:open:has-children:!has-siblings,QTreeView::branch:open:has-children:has-siblings{background-color:white;border-image:transparent;image:url(:/statics/fopen.png);}"
                               "QTreeView::branch{background-color:white;}"
                               "QTreeView::branch:selected{background-color:white;}"
                               "QTreeView::item{background-color:white;}"
                               "QTreeView::item:hover{background:rgb(200, 200, 200);color:rgb(255, 255, 255)}"
                               "QTreeView::item:selected{background:rgb(150, 150, 150);}";
    setStyleSheet(style);
    header()->hide();               // 隐藏树视图的标题

    // init file system model
    m_fs = new QFileSystemModel(this);
    // m_fs->sort(QDir::Type);
    m_fs->setFilter(QDir::AllDirs | QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    QStringList file_filter;
    vector<string> filter = autocomplete_load(CONFIG_TYPE::FILTER);
    for (auto f : filter)
    {
        file_filter.append(QString::fromStdString(f));
    }
    m_fs->setNameFilterDisables(false);                     // 设置未通过名称过滤器的文件是否被过滤或隐藏
    m_fs->setNameFilters(file_filter);
    m_fs->setIconProvider(new myQFileIconProvider(this));   // 为目录模型设置文件图标的提供程序。

    // init tree view data
    setModel(m_fs);
    // setContextMenuPolicy(Qt::CustomContextMenu);

    // connect signals and slots
    connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(onDoubleClicked(const QModelIndex &)));
    connect(this, SIGNAL(clicked(const QModelIndex &)),     // 单击需要相应的只有文件展开还是折叠操作
            this, SLOT(onClicked(const QModelIndex &)));
    // connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
    //         this, SLOT(slotTreeMenu(const QPoint &)));

    // get valid root directory
    const QString root_path = rootPath.isNull() ? QDir::homePath() : rootPath;

    // set root path dependent attributes(properties)
    changeRootPath(root_path);
}

myInfoBarQFrame *LocalFileSystemViewer::initPathBox()
{
    QFont font("黑体", 8, QFont::Normal);

    m_pathname = new QLineEdit(this);
    updatePathEdit(m_dirs.isEmpty() ? QString("") : m_dirs.top());
    m_previous_btn = new QPushButton(this);
    m_up_btn = new QPushButton(this);
    m_go_btn = new QPushButton(this);
    m_enter_btn = new QPushButton(this);
    m_pathname->setFont(font);
    m_previous_btn->setFont(font);
    m_previous_btn->setMaximumSize(20, 20);
    m_previous_btn->setIcon(QIcon(":/statics/previous.png"));
    m_previous_btn->setToolTip(tr("后退"));
    m_go_btn->setFont(font);
    m_go_btn->setMaximumSize(20, 20);
    m_go_btn->setIcon(QIcon(":/statics/gopath.png"));
    m_go_btn->setToolTip(tr("前进"));
    m_up_btn->setFont(font);
    m_up_btn->setMaximumSize(20, 20);
    m_up_btn->setIcon(QIcon(":/statics/upath.png"));
    m_up_btn->setToolTip(tr("上一级"));
    m_enter_btn->setFont(font);
    m_enter_btn->setMaximumSize(20, 20);
    m_enter_btn->setIcon(QIcon(":/statics/enterpath.png"));
    m_enter_btn->setToolTip(tr("进入"));

    connect(m_previous_btn, SIGNAL(clicked()), this, SLOT(onBackClicked()));
    connect(m_up_btn, SIGNAL(clicked()), this, SLOT(onUpClicked()));
    connect(m_go_btn, SIGNAL(clicked()), this, SLOT(onGoClicked()));
    connect(m_enter_btn, SIGNAL(clicked()), this, SLOT(onEnterClicked()));
    connect(m_pathname, SIGNAL(textEdited(const QString &)), this, SLOT(onPathEdited(const QString &)));
    connect(m_pathname, SIGNAL(returnPressed()), this, SLOT(onEnterClicked()));

    QGridLayout *pLayout_apply = new QGridLayout();
    pLayout_apply->addWidget(m_previous_btn, 0, 0, 1, 1);
    pLayout_apply->addWidget(m_go_btn, 0, 1, 1, 1);
    pLayout_apply->addWidget(m_up_btn, 0, 2, 1, 1);
    pLayout_apply->addWidget(m_pathname, 0, 3, 1, 3);
    pLayout_apply->addWidget(m_enter_btn, 0, 6, 1, 1);

    return new myInfoBarQFrame(pLayout_apply, this);
}

bool LocalFileSystemViewer::setRootPath(const QString &rootPath)
{
    QDir root_dir = QDir(rootPath);
    return setRootPath(root_dir);
}

bool LocalFileSystemViewer::setRootPath(const QDir &rootPath)
{
    if (!rootPath.exists())
    {
        return false;
    }

    changeRootPath(rootPath.path());
    return true;
}

static QString realText = "";
QString LocalFileSystemViewer::geteElidedText(QFont font, QString str, int MaxWidth)
{
    realText = str;
    QFontMetrics fontWidth(font);
    int width = fontWidth.width(str);
    if (width >= MaxWidth)
    {
        str = fontWidth.elidedText(str, Qt::ElideRight, MaxWidth);
    }
    return str;
}
QString LocalFileSystemViewer::getRealText()
{
    return realText;
}

void LocalFileSystemViewer::onBackClicked()
{
    if (m_dirs.size() < 2)
        return;
    // current dir move to
    auto old_root_path = m_dirs.top();
    m_o_dirs.push(old_root_path);
    m_dirs.pop();

    // go to previous dir
    auto root_path = m_dirs.top();
    m_dirs.pop();
    changeRootPath(root_path);
}
void LocalFileSystemViewer::onUpClicked()
{
    if (m_dirs.isEmpty())
        return;
    auto dir = QDir(m_dirs.top());
    bool cdup = dir.cdUp();                         // QDir::cdUp()：通过从 QDir 的当前目录向上移动一个目录来更改目录。
    if (!cdup)
        return;
    changeRootPath(dir.path());
}
void LocalFileSystemViewer::onGoClicked()
{
    if (m_o_dirs.size() < 1)
        return;
    // current dir move to
    auto root_path = m_o_dirs.top();
    m_o_dirs.pop();

    changeRootPath(root_path);
}
void LocalFileSystemViewer::onEnterClicked()
{
    if (QFileInfo(m_pathname->text()).isDir())
        changeRootPath(m_pathname->text());
}
void LocalFileSystemViewer::onPathEdited(const QString &rootPath)
{
    if (m_path_lenght_last > rootPath.size())
    {
        m_path_lenght_last = rootPath.size();
        return;
    }
    QDir dir = QDir(rootPath);
    QDir dir_dir = QDir(QFileInfo(rootPath).absolutePath());        // 绝对路径，不包含文件名
    QStringList candidates = dir_dir.entryList(QDir::AllDirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);    // 获取指定目录下的所有文件夹的名称，不包含文件名，QDir::AllDirs表示获取所有子目录，QDir::NoSymLinks表示不获取符号链接，
                                                                                                            // QDir::NoDotAndDotDot表示不获取"."和".."这两个特殊目录。
    QStringList placeholders;
    for (QString &s : candidates)
    {
        int index = s.indexOf(dir.dirName());                       // QDir::dirName()：返回路径中的最后一个目录名称或文件名
        if (index == 0)
        {
            placeholders.push_back(s);
        }
    }
    if (!placeholders.isEmpty())
    {
        int s = m_pathname->text().size();
        m_pathname->setText(dir_dir.path() + "/" + placeholders[0]);
        int e = m_pathname->text().size();
        m_pathname->setSelection(s, e);                             // 从位置s选择文本，长度为e，选中后干啥？？？
    }
    m_path_lenght_last = m_pathname->text().size();
    // setSelection
}
void LocalFileSystemViewer::onClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    if (isExpanded(index))
    {
        collapse(index);                    // 折叠项目
    }
    else
    {
        expand(index);                      // 展开项目
    }
}
void LocalFileSystemViewer::onClicked(const bool &triggered)
{
    onClicked(currentIndex());
}

void LocalFileSystemViewer::onDoubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    m_index = index;
    const QString clicked_path = m_fs->filePath(index);

    setCurrentIndex(index);
    if (m_fs->isDir(index))
    {
        changeRootPath(clicked_path);
        emit dirItemDoubleClicked(clicked_path);
    }
    else
    {
        emit fileItemDoubleClicked(clicked_path);                       // 文件树视图中触发signal，主窗口MainWindow也能接收到
    }
}

void LocalFileSystemViewer::slotTreeMenu(const QPoint &pos)
{
    QMenu menu;

    QModelIndex index = currentIndex();
    if (index.isValid())
    {
        if (m_fs->isDir(index))
        {
            // QStringLiteral title = isExpanded(index) ? "折叠" : "展开";
            menu.addAction(QStringLiteral("折叠"), this, SLOT(onClicked(const bool &)));
        }
        menu.addSeparator();
        // menu.addAction(QStringLiteral("折叠"), this, SLOT(onClicked(const QModelIndex &)));
    }
    menu.exec(QCursor::pos());
}

void LocalFileSystemViewer::updatePathEdit(const QString &rootPath)
{
    if (!m_pathname)
        return;
    QDir path = QDir(rootPath);
    if (!rootPath.isEmpty())
    {
        if (rootPath[rootPath.size() - 1] != '/')
        {
            m_pathname->setText(rootPath + "/");
        }
        else
        {
            m_pathname->setText(rootPath);
        }
    }
    else
    {
        m_pathname->setText("");
    }
    m_path_lenght_last = m_pathname->text().size();
}

void LocalFileSystemViewer::changeRootPath(const QString &rootPath)
{
    if (rootPath.isEmpty())
        return;
    m_fs->setRootPath(rootPath);
    m_dir = QDir::cleanPath(rootPath);          // 用于规范化路径字符串，避免在不同操作系统或环境下出现路径不一致的问题。
    static_cast<MainWindow *>(mp_parent)->filetreeShowAction(m_dir);

    // set root index from given root path
    const QModelIndex rootIndex = m_fs->index(QDir::cleanPath(rootPath));
    if (rootIndex.isValid())
    {
        setRootIndex(rootIndex);
    }

    // only show the filename column
    for (int i = 1; i < header()->count(); i++) // header()：用于获取树形视图的列标题头
    {
        hideColumn(i);
    }

    // save previous path
    if (m_dirs.isEmpty() || m_dir != m_dirs.top())
        m_dirs.push(m_dir);

    // set some window properties
    updatePathEdit(m_dirs.top());
    setWindowTitle(m_dirs.top());               // setWindowTitle()接口是属于QWidget的成员函数，QTreeView是QWidget的子类
}

void LocalFileSystemViewer::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Left:
    case Qt::Key_Up:
        last();
        break;

    case Qt::Key_Right:
    case Qt::Key_Down:
        next();
        break;

    default:
        break;
    }
}

void LocalFileSystemViewer::next()
{
    QModelIndex index;
    index = indexBelow(m_index);
    if (!m_fs->isDir(index))
    {
        onDoubleClicked(index);
    }
}

void LocalFileSystemViewer::last()
{
    QModelIndex index;
    index = indexAbove(m_index);
    if (!m_fs->isDir(index))
    {
        onDoubleClicked(index);
    }
}