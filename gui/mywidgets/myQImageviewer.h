#ifndef QIMAGEVIEWER_H
#define QIMAGEVIEWER_H

#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QDir>
#include <QSize>
#include <QFileInfo>
#include <QFileInfoList>
#include <string>
#include <functional>

#include "defines.h"
#include "stats.h"

/** 保存image buffer和image格式信息，同时支持基础的图片编辑操作 **/
class QImageViewer : public QWidget
{
    Q_OBJECT
public:
    explicit QImageViewer(QWidget *parent = 0);
    explicit QImageViewer(QWidget *parent,QString &caption,QString &dir,QString &filer);
    ~QImageViewer();

    QWidget *parent;

    int index;
    int angle;
    QSize size;
    QSize origin_size;
    QSize origin_real_size;
    int size_ratio = 100;
    const int size_ratio_step = 1;
    QString filename;                               // 包含文件名的绝对路径
    QString path;
    QDir dir;                                       // 管理文件系统的目录和文件，它提供了一组方法，可以获取目录下的文件和子目录，创建和删除目录，修改文件名和路径等
    QFileInfo fileInfo;                             // 用于操作文件路径的类。它可以获取文件的各种信息，比如文件名、文件路径、文件大小、创建时间、修改时间等等
    QFileInfoList fileInfoList;

    QPixmap pixmap;
    QImage image;
    QImage origin_image;

    REQUESTINFO rawRequest;
    std::string rawType;                            // 图像格式string
    char cfilename[1024];                           // 图像文件路径

    IMAGEINFO rawinfo;                              // 图像格式、宽、高、stride等信息
    STATS_INFO m_stats;

    void setStats(const STATS_INFO& stats) { m_stats = stats; }
    STATS_INFO getStats() { return m_stats; }

    /*
     * 功能：process为true时加载并转换图像buffer，默认为true
    */
    RESULT setRaw(const IMAGEINFO &imgInfo, const bool& process=true);
    RESULT justSetRaw(const IMAGEINFO &imgInfo);
    RESULT justSetAngle(const int &angle);
    BOOL isSameFormat(QImageViewer *viewer);

    /* 通过弹窗确定filename并加载filename路径下不同格式的图片文件信息到fileInfoList中 */
    RESULT openImageFile(const QString &caption,const QString &dir,const QString &filer);
    /* 加载filename路径下不同格式的图片文件信息到fileInfoList中 */
    RESULT openImageFile(const QString &filename);
    /* 通过默认弹窗确定filename并加载filename路径下不同格式的图片文件信息到fileInfoList中 */
    RESULT openImageFile();

    RESULT saveAs();

    /* close file */
    RESULT closeImageFile(void);

    /* delete file */
    RESULT delImageFile(void);

    /* file skipping */
    RESULT last(void);
    RESULT next(void);

    /* change image */
    RESULT zoomIn(void);
    RESULT zoomOut(void);
    RESULT spinToRight(void);
    RESULT spinToLeft(void);
    RESULT flip(void);

    int getZoom(void);
    RESULT setZoom(const int &);
    int setAGetZoom(const int &);

    const BOOL &isEmpty() const { return m_empty; }
    RESULT resetALL(void);

    RESULT setBP(const bool&);
    RESULT setLight(const int &);

private:
    /* init param */
    void initImageResource(void);

    /* 通过默认弹窗确定filename并加载filename路径下不同格式的图片文件信息到fileInfoList中 */
    RESULT loadImageResource(void);
    /* 通过弹窗确定filename并加载filename路径下不同格式的图片文件信息到fileInfoList中 */
    RESULT loadImageResource(const QString &caption,const QString &dir,const QString &filer);
    /* 加载filename路径下不同格式的图片文件信息到fileInfoList中 */
    RESULT loadImageResource(const QString &ifilename);

    /*
     * 功能：加载filename路径下不同格式的图片文件信息到fileInfoList中
    */
    int getFileInfoList(void);
    int getFileCurIndex(void);
    /*
     * 功能：加载filename路径下的图片buffer到image中
    */
    RESULT upgradeFileInfo(QString &filename,int angle,int sizeScale);
    /*
     * 功能：加载普通图到成员变量image中
    */
    RESULT loadNormalImg(QString &filename,int angle,int sizeScale);
    /*
     * 功能：转换并加载raw图到成员变量image中
    */
    RESULT loadRawImg(QString &filename,int angle,int sizeScale);
    RESULT setAngleAndScale(const int &angle, const int &sizeScale);
    RESULT indexLoop(const std::function<void(int &)>&);

public slots:

private:
    struct DEFFENCEREPRO
    {
        bool is_init = false;
        int ratio_tmp = 100;
        int angle_tmp = 0;
    }m_repro;

    BOOL m_empty;
};

#endif // QIMAGEVIEWER_H
