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
    QString filename;
    QString path;
    QDir dir;
    QFileInfo fileInfo;
    QFileInfoList fileInfoList;

    QPixmap pixmap;
    QImage image;
    QImage origin_image;

    REQUESTINFO rawRequest;
    std::string rawType;
    char cfilename[1024];

    IMAGEINFO rawinfo;
    STATS_INFO m_stats;

    void setStats(const STATS_INFO& stats) { m_stats = stats; }
    STATS_INFO getStats() { return m_stats; }

    RESULT setRaw(const IMAGEINFO &imgInfo, const bool& process=true);
    RESULT justSetRaw(const IMAGEINFO &imgInfo);
    RESULT justSetAngle(const int &angle);
    BOOL isSameFormat(QImageViewer *viewer);

    /* open a file */
    RESULT openImageFile(const QString &caption,const QString &dir,const QString &filer);
    RESULT openImageFile(const QString &filename);
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

    /* open a image */
    RESULT loadImageResource(void);
    RESULT loadImageResource(const QString &caption,const QString &dir,const QString &filer);
    RESULT loadImageResource(const QString &ifilename);

    /* get file info list from current path */
    int getFileInfoList(void);
    int getFileCurIndex(void);
    RESULT upgradeFileInfo(QString &filename,int angle,int sizeScale);
    RESULT loadNormalImg(QString &filename,int angle,int sizeScale);
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
