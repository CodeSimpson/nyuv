#ifndef __MYFILETREE_H__
#define __MYFILETREE_H__

#include "defines.h"
#include "autointerface.h"
#include "myInfoBarQFrame.h"

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
#include <QStack>

#include <tuple>
#include <utility>

class LocalFileSystemViewer : public QTreeView
{
    Q_OBJECT
public:
    LocalFileSystemViewer(const QString &rootPath = QString(), QWidget *parent = nullptr);

    bool setRootPath(const QString &rootPath);
    bool setRootPath(const QDir &rootPath);
    QString getRootPath() const { return m_dir; };

    static QString geteElidedText(QFont font, QString str, int MaxWidth);
    static QString getRealText();
    /*
     * 功能：初始化文件树视图目录编辑栏
    */
    myInfoBarQFrame *initPathBox();

    void next();
    void last();

signals:
    void fileItemDoubleClicked(const QString &filePath);
    void dirItemDoubleClicked(const QString &dirPath);

private slots:
    void onDoubleClicked(const QModelIndex &index);
    /*
     * 功能：首先检查项目的有效性，如果项目无效，则不执行任何操作；如果项目有效，则检查该项目是否已经展开；如果项目已展开，则将其折叠起来；否则，展开该项目。
    */
    void onClicked(const QModelIndex &index);
    void onClicked(const bool&);
    /*
     * 功能：回到上一步文件路径
    */
    void onBackClicked();
    void onUpClicked();
    /*
     * 功能：向下一步文件路径前进
    */
    void onGoClicked();
    /*
     * 功能：回车键，如果文本编辑框中的路径是目录，则修改根目录
    */
    void onEnterClicked();
    /*
     * 功能：当目录编辑栏中文本发生变化时调用，作用未知
    */
    void onPathEdited(const QString &rootPath);
    void slotTreeMenu(const QPoint &pos);

private:
    void changeRootPath(const QString &rootPath);
    void keyPressEvent(QKeyEvent *event);
    /*
     * 功能：更新文件树目录编辑栏
    */
    void updatePathEdit(const QString &rootPath);

private:
    QFileSystemModel *m_fs;
    QString m_dir;                              // 保存根目录路径
    QWidget *mp_parent;
    QModelIndex m_index;

    QStack<QString> m_dirs;                     // m_dirs.top()始终为当前根目录
    QStack<QString> m_o_dirs;                   // 之前进入的路径

    QPushButton *m_previous_btn;                // 返回上一步路径
    QPushButton *m_up_btn;                      // 返回上一级路径
    QPushButton *m_go_btn;                      // 进入下一步路径
    QPushButton *m_enter_btn;
    QLineEdit *m_pathname;

    size_t m_path_lenght_last;
};

#endif