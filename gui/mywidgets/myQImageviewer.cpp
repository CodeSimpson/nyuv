#include "myQImageviewer.h"
#include "cvtinterface.h"
#include "autointerface.h"
#include "threadpool.h"
#include "defines.h"
#include "table.h"
#include "alight.h"

#undef __LOGTAG__
#define __LOGTAG__ "VIEWER"
#include "log.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QImageReader>
#include <iostream>
#include <functional>
#include <future>
#include <cmath>

QImageViewer::QImageViewer(QWidget *parent) : QWidget(parent), m_empty(true)
{
    this->parent = parent;
    initImageResource();
}

QImageViewer::QImageViewer(QWidget *parent,
                           QString &caption,
                           QString &dir,
                           QString &filer) : QWidget(parent), m_empty(true)
{
    this->parent = parent;
    initImageResource();
    loadImageResource(caption, dir, filer);
}

QImageViewer::~QImageViewer(void)
{
    this->parent = NULL;
}

BOOL QImageViewer::isSameFormat(QImageViewer *viewer)
{
    if (!viewer || filename.isEmpty() || (viewer->filename).isEmpty())
    {
        return false;
    }

    QFileInfo src_info = QFileInfo(filename);
    QFileInfo dest_info = QFileInfo(viewer->filename);

    BOOL is_same = (src_info.size() == dest_info.size()) && (src_info.suffix() == dest_info.suffix());

    return is_same;
}

RESULT QImageViewer::setRaw(const IMAGEINFO &imgInfo, const bool &process)
{
    rawinfo = imgInfo;
    memcpy(cfilename, filename.toStdString().c_str(), filename.size() + 1);
    rawRequest.ifilename = cfilename;
    rawRequest.iformat = imgInfo.format;
    rawRequest.ofilename = "-";
    rawRequest.oformat = imgInfo.format;
    rawRequest.bayer = imgInfo.bayer;
    rawType = imgInfo.type;

    if (process)
    {
        return upgradeFileInfo(filename, angle, 0);
    }

    return RESULT::SUCCESS;
}

RESULT QImageViewer::justSetRaw(const IMAGEINFO &imgInfo)
{
    rawinfo = imgInfo;
}

RESULT QImageViewer::justSetAngle(const int &ag)
{
    angle = ag;
}

RESULT QImageViewer::openImageFile(const QString &caption,
                                   const QString &dir,
                                   const QString &filer)
{
    return loadImageResource(caption, dir, filer);
}

RESULT QImageViewer::openImageFile(const QString &filename)
{
    return loadImageResource(filename);
}

RESULT QImageViewer::openImageFile()
{
    return loadImageResource();
}

RESULT QImageViewer::saveAs()
{
    if (filename.isEmpty())
    {
        QMessageBox::warning(this,
                             tr("Error"),
                             tr("Nothing to save!"));
        return RESULT::FAILED;
    }
    QString placeholder = QFileInfo(filename).baseName() + ".jpg";
    QString save_filename = QFileDialog::getSaveFileName(this, tr("另存为...jpg"), tr(placeholder.toStdString().c_str()), tr("Curve TagName Files (*.jpeg, *.jpg)"));
    if (!save_filename.isEmpty())
    {
        auto info = QFileInfo(save_filename);
        QString suffix = info.suffix();
        if (suffix == "")
        {
            save_filename += ".jpg";
        }
        image.save(save_filename, "JPEG");
    }
    return RESULT::SUCCESS;
}

RESULT QImageViewer::closeImageFile(void)
{
    int angle_tmp = angle;
    initImageResource();
    angle = angle_tmp;
    pixmap = QPixmap();
    image = QImage();
    return RESULT::SUCCESS;
}

RESULT QImageViewer::delImageFile(void)
{
    if (filename.isEmpty())
    {
        return RESULT::FAILED;
    }

    if (!QFile::remove(filename))
    {
        LOGE("remove {} failed", filename.toStdString());
        return RESULT::FAILED;
    }

    /* delete from list */
    fileInfoList.removeAt(index);

    return RESULT::SUCCESS;
}

RESULT QImageViewer::indexLoop(const std::function<void(int &)> &func)
{
    int count = fileInfoList.count();
    if (count <= 0)
        return RESULT::FAILED;
    while (1)
    {
        func(index);
        if (index < 0)
        {
            index = count - 1;
        }
        else if (index >= count)
        {
            index = 0;
        }

        filename.clear();
        filename.append(path);
        filename += "/";
        filename += fileInfoList.at(index).fileName();

        if (!QFile(filename).exists())
        {
            fileInfoList.removeAt(index);
            continue;
        }
        else
        {
            break;
        }
    }

    return RESULT::SUCCESS;
}

RESULT QImageViewer::last(void)
{
    return indexLoop([=](int &index) {
        index -= 1;
    });
}

RESULT QImageViewer::next(void)
{
    return indexLoop([=](int &index) {
        index += 1;
    });
}

int QImageViewer::getZoom(void)
{
    return size_ratio;
}

RESULT QImageViewer::setZoom(const int &zoom)
{
    size_ratio = zoom;
    if (!image.isNull())
    {
        LOGI("zoom triggered {}.", zoom);
        return setAngleAndScale(angle, 0);
    }
    else
    {
        return RESULT::FAILED;
    }
}

// RGB<> QImageViewer::getRGB()
// {
//     return
// }

RESULT QImageViewer::setLight(const int &l)
{
    if (!image.isNull())
    {
        if (l >= 0)
        {
            LOGI("auto light triggered {}.", l);
            image = adjustBrightness(origin_image, l);
        }
        else
        {
            image = origin_image;
        }
        //没有改变size 和 angle, 但是认为图像内容改变了, 所以认为是初始化状态
        m_repro.is_init = true;
        return setAngleAndScale(angle, 0);
    }
    return RESULT::SUCCESS;
}

RESULT QImageViewer::setBP(const bool &bp)
{
    if (!image.isNull())
    {
        LOGI("bayer partern triggered {}.", bp);
        rawinfo.bayer = bp;
        return setRaw(rawinfo);
    }
    else
    {
        return RESULT::FAILED;
    }
}

RESULT QImageViewer::zoomIn(void)
{
    if (!image.isNull())
    {
        LOGI("zoom in triggered.");
        return setAngleAndScale(angle, size_ratio_step);
    }
    else
    {
        return RESULT::FAILED;
    }
}

RESULT QImageViewer::zoomOut(void)
{
    if (!image.isNull())
    {
        LOGI("zoom out triggered.");
        return setAngleAndScale(angle, -size_ratio_step);
    }
    else
    {
        return RESULT::FAILED;
    }
}

RESULT QImageViewer::spinToRight(void)
{
    angle += 1;
    angle = angle % 4;
    LOGI("to right triggered.");
    /* load file info */
    return setAngleAndScale(angle, 0);
}

RESULT QImageViewer::spinToLeft(void)
{
    angle += 3;
    angle = angle % 4;
    LOGI("to left triggered.");
    /* load file info */
    return setAngleAndScale(angle, 0);
}

RESULT QImageViewer::flip()
{
    if (!image.isNull())
    {
        LOGI("flip triggered.");
        image = image.mirrored(true, false);
        m_repro.is_init = true;
        return setAngleAndScale(angle, 0);
    }
    return RESULT::SUCCESS;
}

void QImageViewer::initImageResource(void)
{
    m_empty = true;
    index = -1;
    angle = 0;
    size = QSize(0, 0);

    filename.clear();
    path.clear();
    fileInfoList.clear();
}

RESULT QImageViewer::resetALL(void)
{
    LOGI("reset triggered.");
    angle = 0;
    size_ratio = 100;
    return RESULT::SUCCESS;
}

RESULT QImageViewer::loadImageResource(void)
{
    filename = QFileDialog::getOpenFileName(this, tr("Select image:"),
                                            "/home/", tr("Images (*)"));
    if (filename.isEmpty())
    {
        return RESULT::FAILED;
    }

    /* get file list */
    getFileInfoList();

    /* load file info */
    // return upgradeFileInfo(filename, angle, 0);
    return RESULT::SUCCESS;
}

RESULT QImageViewer::loadImageResource(const QString &caption,
                                       const QString &directory,
                                       const QString &filer)
{
    filename = QFileDialog::getOpenFileName(this, caption, directory, filer);
    if (filename.isEmpty())
    {
        return RESULT::INVALID_FILE;
    }

    /* get file list */
    getFileInfoList();

    /* load file info */
    // return upgradeFileInfo(filename, angle, 0);
    return RESULT::SUCCESS;
}

RESULT QImageViewer::loadImageResource(const QString &ifilename)
{
    filename = ifilename;
    if (filename.isEmpty())
    {
        return RESULT::INVALID_FILE;
    }

    /* get file list */
    getFileInfoList();
    /* load file info */
    // return upgradeFileInfo(filename, angle, 0);
    return RESULT::SUCCESS;
}

RESULT QImageViewer::upgradeFileInfo(QString &filename, int angle, int sizeScale)
{
    RESULT ret = RESULT::SUCCESS;
    if (filename.isEmpty())
    {
        LOGE("empty file name");
        return RESULT::INVALID_FILE;
    }

    //图片格式则直接显示
    fileInfo = QFileInfo(filename);
    QString suffix = fileInfo.suffix();
    if (suffix == "jpg" || suffix == "bmp" || suffix == "png" || suffix == "gif" || suffix == "jpeg")
    {
        ret = loadNormalImg(filename, angle, sizeScale);
    }
    else //raw 图则转换
    {
        ret = loadRawImg(filename, angle, sizeScale);
    }

    if (ret != RESULT::SUCCESS)
    {
        m_empty = true;
        size = QSize(0, 0);
    }
    else
    {
        origin_image = image.copy();
        m_empty = false;
    }

    return ret;
}
RESULT QImageViewer::loadRawImg(QString &filename, int angle, int sizeScale)
{
    //check request
    if (filename.isEmpty())
    {
        return RESULT::INVALID_FILE;
    }
    if (rawType.empty())
    {
        return RESULT::INVALID_TYPE;
    }
    if (!rawRequest.iformat.isValid())
    {
        return RESULT::INVALID_FORMAT;
    }

    // memcpy(cfilename, filename.toStdString().c_str(), filename.size() + 1);
    strcpy(cfilename, filename.toStdString().c_str());
    rawRequest.ifilename = cfilename;

    RESULT ret = cvt_process(rawType, rawRequest);
    if (ret != RESULT::SUCCESS)
    {
        return ret;
    }

    image = QImage(rawRequest.obuffer.begin(),
                   rawRequest.oformat.width,
                   rawRequest.oformat.height,
                   QImage::Format_RGB888);

    size = image.size();
    origin_size = image.size();
    origin_real_size = image.size();

    m_repro.is_init = true;
    setAngleAndScale(angle, sizeScale);
    m_repro.is_init = false;

    /* upgrade index */
    index = getFileCurIndex();

    return RESULT::SUCCESS;
}
RESULT QImageViewer::loadNormalImg(QString &filename, int angle, int sizeScale)
{
    if (filename.isEmpty())
    {
        return RESULT::FAILED;
    }

    if (!image.load(filename))
    {
        return RESULT::FAILED;
    }

    size = image.size();
    origin_size = image.size();
    origin_real_size = image.size();

    m_repro.is_init = true;
    setAngleAndScale(angle, sizeScale);
    m_repro.is_init = false;

    /* upgrade index */
    index = getFileCurIndex();

    return RESULT::SUCCESS;
}

static const int MAXZOOM = 500;
static const int MINZOOM = 1;
int QImageViewer::setAGetZoom(const int &sizeScale)
{
    //缩放比例函数
    if (sizeScale > 0)
    {
        size_ratio += (size_ratio >= 100 ? 20 : ([=]() -> int {
            if (size_ratio >= 68)
                return 16;
            if (size_ratio >= 36)
                return 8;
            if (size_ratio >= 20)
                return 4;
            return 2;
        }()));
    }
    else if (sizeScale < 0)
    {
        size_ratio += (size_ratio > 100 ? -20 : ([=]() -> int {
            if (size_ratio > 68)
                return -16;
            if (size_ratio > 36)
                return -8;
            if (size_ratio > 20)
                return -4;
            return -2;
        }()));
    }

    //极大极小抑制
    if (size_ratio > MAXZOOM)
        size_ratio = MAXZOOM;
    else if (size_ratio < MINZOOM)
        size_ratio = MINZOOM;

    return size_ratio;
}

RESULT QImageViewer::setAngleAndScale(const int &angle, const int &sizeScale)
{
    QImage imgRotate;
    QMatrix matrix;
    QImage imgScaled;

    //旋转90/270
    if ((angle & 1))
    {
        origin_real_size = origin_size.transposed();
    }
    else
    {
        origin_real_size = origin_size;
    }

    size_ratio = setAGetZoom(sizeScale);

    //如果scale和angle没有改变，就不用再往下做了，但是第一次除外
    if (!(m_repro.is_init) && ((m_repro.ratio_tmp == size_ratio) && (m_repro.angle_tmp == angle)))
    {
        return RESULT::SUCCESS;
    }

    double ratio = sqrt((double)size_ratio / 100.0);
    int new_w = origin_size.width() * ratio;
    int new_h = origin_size.height() * ratio;

    auto scale_mode = [=]() -> Qt::TransformationMode {
        if (ratio < 1)
        {
            return Qt::SmoothTransformation;
        }
        else
        {
            return Qt::FastTransformation;
        }
    }();

    imgScaled = image.scaled((new_w < 1) ? 1 : new_w,
                             (new_h < 1) ? 1 : new_h,
                             Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);
    size = imgScaled.size();

    /* modify angle */
    matrix.rotate(angle * 90);
    imgRotate = imgScaled.transformed(matrix);

    if (angle % 2 == 1)
    {
        size = QSize(size.height(), size.width());
    }

    pixmap = QPixmap::fromImage(imgRotate);
    m_repro.ratio_tmp = size_ratio;
    m_repro.angle_tmp = angle;

    LOGI("new image triggered.");

    return RESULT::SUCCESS;
}

int QImageViewer::getFileInfoList(void)
{
    QFileInfo info;
    QFileInfoList infoList;

    path = QFileInfo(filename).absolutePath();
    dir = QFileInfo(filename).absoluteDir();

    /* clear list */
    fileInfoList.clear();

    QStringList file_filter;
    vector<string> filter = autocomplete_load(CONFIG_TYPE::FILTER);
    for (auto f : filter)
    {
        file_filter.append(QString::fromStdString(f));
    }
    // qDebug() << file_filter;
    infoList = dir.entryInfoList(file_filter, QDir::Files);

    for (int i = 0; i < infoList.count(); i++)
    {
        info = infoList.at(i);
        fileInfoList.append(info);
    }

    return 0;
}

int QImageViewer::getFileCurIndex(void)
{
    QFileInfo info;
    int j;

    if (fileInfoList.count() <= 0)
    {
        LOGE("empty file list");
        return -1;
    }

    for (j = 0; j < fileInfoList.count(); j++)
    {
        info = fileInfoList.at(j);
        if (info.fileName() == fileInfo.fileName())
        {
            break;
        }
    }

    if (j >= fileInfoList.count())
    {
        LOGE("cannot find current file");
        return -1;
    }

    index = j;
    //qDebug() << "Current fileInfo index: " << index;

    return index;
}
