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
    myInfoBarQFrame *initPathBox();

    void next();
    void last();

signals:
    void fileItemDoubleClicked(const QString &filePath);
    void dirItemDoubleClicked(const QString &dirPath);

private slots:
    void onDoubleClicked(const QModelIndex &index);
    void onClicked(const QModelIndex &index);
    void onClicked(const bool&);
    void onBackClicked();
    void onUpClicked();
    void onGoClicked();
    void onEnterClicked();
    void onPathEdited(const QString &rootPath);
    void slotTreeMenu(const QPoint &pos);

private:
    void changeRootPath(const QString &rootPath);
    void keyPressEvent(QKeyEvent *event);
    void updatePathEdit(const QString &rootPath);

private:
    QFileSystemModel *m_fs;
    QString m_dir;
    QWidget *mp_parent;
    QModelIndex m_index;

    QStack<QString> m_dirs;
    QStack<QString> m_o_dirs;

    QPushButton *m_previous_btn;
    QPushButton *m_up_btn;
    QPushButton *m_go_btn;
    QPushButton *m_enter_btn;
    QLineEdit *m_pathname;

    size_t m_path_lenght_last;
};

#endif