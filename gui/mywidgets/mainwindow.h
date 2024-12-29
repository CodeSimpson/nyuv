#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QLabel>
#include <QFileInfoList>
#include <QSplitter>

#include "myQImageviewer.h"
#include "myFileTree.h"
#include "myInfoBar.h"
#include "myInfoBarQFrame.h"
#include "update.h"
#include "myQLabel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow()
    {
        delete m_filetree;
        delete m_setting;
        for (auto &d : m_imageviewer_list)
        {
            SAFEDEL(d);
        }
    }

    QSize getImgViewSize() const
    {
        return mp_widget_central->frameSize();
    }

    QSize getImgLabelSize() const
    {
        return m_imageviewer->origin_real_size;
    }

    int getZoom() const
    {
        return m_imageviewer->getZoom();
    }

    void setHome(const QString &home)
    {
        m_home = home;
        m_filetree->setRootPath(m_home);
    }

    void setBP(const bool &);
    STR getHF();
    int getAL();
    void setLight(const int&);

    QImage getImage() const
    {
        return m_imageviewer->image;
    }

    QImageViewer *getImageViewer() const
    {
        return m_imageviewer;
    }

private:
    QMenuBar *mp_bar_menu;
    QToolBar *mp_bar_tool;
    QToolBar *mp_bar_left;
    myInfoBarQFrame *mp_frame_left;
    QSplitter *mp_splitter_left;
    QToolBar *mp_bar_right;
    QWidget *mp_widget_central;
    QTabBar *mp_widget_tab;
    QStatusBar *mp_bar_status;
    QLabel *mp_label_image;
    LocalFileSystemViewer *m_filetree;
    ImageInformationBar *m_setting;

    /* new class */
    QImageViewer *m_imageviewer;
    QImageViewer *m_tmpviewer;
    QList<QImageViewer *> m_imageviewer_list;

    void initMainWindow(void);
    void initUiComponent(void);
    void setQImageViewerWidget(void);
    void setWindowComponet(void);
    void initImageResource(void);
    void loadImageResource(const bool &isauto = true);
    RESULT loadImageResource(REQUESTINFO &);
    RESULT loadRawSet();

    QAction *mp_action_open;
    QAction *mp_action_folder;
    QAction *mp_action_close;
    QAction *mp_action_last;
    QAction *mp_action_next;
    QAction *mp_action_toleft;
    QAction *mp_action_toright;
    QAction *mp_action_flip;
    QAction *mp_action_enlarge;
    QAction *mp_action_lessen;
    QAction *mp_action_delete;
    QAction *mp_action_save;
    QAction *mp_action_reset;
    QAction *mp_action_update;
    QAction *mp_action_help;
    QAction *mp_action_add;
    QAction *mp_action_prefer;

    QStringList m_rawset;
    QString m_home;

    Update *mp_update;

public slots:
    void openActionTriggered(void);
    void openFolderActionTriggered(void);
    void saveActionTriggered(void);
    void closeActionTriggered(void);
    void lastActionTriggered(void);
    void nextActionTriggered(void);
    void toLeftActionTriggered(void);
    void toRightActionTriggered(void);
    void flipActionTriggered(void);
    void toEnlargeActionTriggered(void);
    void toLessenActionTriggered(void);
    void deleteActionTriggered(void);
    void fileTreeTriggered(const QString &);
    void settingTriggered(const IMAGEINFO &);
    void setZoomAction(const int &);
    void setResetAction(void);
    void updateAction(void);
    void helpAction(void);
    void filetreeShowAction(QString &str);
    void addToAction(void);
    void preferAction(void);
    void tabCloseAction(int);
    void tabChoseAction(int);
    void tabMoveAction(int, int);

protected:
    void dragEnterEvent(QDragEnterEvent *) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event);

private:
    friend ImageInformationBar;
    friend QImageViewer;
};

#endif // MAINWINDOW_H
