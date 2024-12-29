#include "myInfoBar.h"
#include "defines.h"
#include "autointerface.h"
#include "cvtinterface.h"
#include "mainwindow.h"
#include "myInfoBarQFrame.h"
#include "myQSpinBox.h"
#include "myQSlider.h"
#include "myQLabel.h"

#undef __LOGTAG__
#define __LOGTAG__ "INFOBAR"
#include "log.h"

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
#include <QSlider>
#include <QSpinBox>
#include <QFrame>
#include <QCheckBox>
#include <QDebug>

#include <tuple>
#include <utility>

using std::cout;
using std::endl;

static MainWindow *mainwindow = nullptr;
ImageInformationBar::ImageInformationBar(const QStringList &supportedFormats, QWidget *parent)
    : QFrame(parent), mp_parent(parent), m_supportedFormats(supportedFormats),
      m_last_height(0), m_last_width(0), m_last_stride(0)
{
    mainwindow = static_cast<MainWindow *>(mp_parent);
    setFrameShape(QFrame::Box);
    setFrameShadow(QFrame::Sunken);
    initBox();
    initFormat();
}

void ImageInformationBar::initFormat()
{
    FORMAT format;
    string type = m_type->currentText().toStdString();
    IMAGEINFO predict(type, format);
    RESULT ret = autocomplete_init(predict);
    if (RESULT::SUCCESS == ret)
    {
        m_height->setValue(predict.format.height);
        m_width->setValue(predict.format.width);
        m_stride->setValue(predict.format.stride);
    }
    m_flock = false;
}

void ImageInformationBar::initBox()
{
    m_layout = new QGridLayout(this);
    m_layout->setSpacing(1);
    m_layout->setContentsMargins(0, 0, 0, 0);

    QGridLayout *pLayout_format = initFormatBox();
    myInfoBarQFrame *frame_format = new myInfoBarQFrame(pLayout_format, this);
    m_layout->addWidget(frame_format);

    // QGridLayout *pLayout_apply = initViewBox();
    // auto frame_apply = new myInfoBarQFrame(pLayout_apply, this);
    // m_layout->addWidget(frame_apply);

    QGridLayout *pLayout_check = initCheckBox();
    myInfoBarQFrame *frame_check = new myInfoBarQFrame(pLayout_check, this);
    m_layout->addWidget(frame_check);

    QGridLayout *pLayout_zoom = initSliderBox();
    myInfoBarQFrame *frame_zoom = new myInfoBarQFrame(pLayout_zoom, this);
    m_layout->addWidget(frame_zoom);

    QGridLayout *pLayout_stats = initStatsBox();
    myInfoBarQFrame *frame_stats = new myInfoBarQFrame(pLayout_stats, this);
    m_layout->addWidget(frame_stats);
    frame_stats->setVisible(false);

    setLayout(m_layout);
}

QGridLayout *ImageInformationBar::initStatsBox()
{
    QFont font("黑体", 8, QFont::Normal);
    auto lable_stats = new myQLabel(this, "<u>STATS</u>", Qt::AlignCenter, 10);
    auto lable_average = new myQLabel(this, "AVERAGE:", Qt::AlignLeft, 10);

    auto stats_ave_y = new myQLabel(this, "y:", Qt::AlignLeft, 8);
    m_stats_ave_y = new myQLabel(this, "na", Qt::AlignLeft, 8);
    auto process_btn = new QPushButton(tr("计算"), this);
    connect(process_btn, SIGNAL(clicked()), this, SLOT(calStats()));
    process_btn->setFont(font);

    QGridLayout *pLayout_stats = new QGridLayout();
    pLayout_stats->addWidget(lable_stats, 1, 0, 1, 3);
    pLayout_stats->addWidget(lable_average, 2, 0, 1, 3);
    pLayout_stats->addWidget(stats_ave_y, 3, 0, 1, 1);
    pLayout_stats->addWidget(m_stats_ave_y, 3, 1, 1, 2);
    pLayout_stats->addWidget(process_btn, 4, 0, 1, 3);

    return pLayout_stats;
}

QGridLayout *ImageInformationBar::initSliderBox()
{
    auto lable_slider = new myQLabel(this, "<u>SLIDER</u>", Qt::AlignCenter, 10);
    auto lable_zoom = new myQLabel(this, "<u>ZOOM</u>", Qt::AlignLeft, 8);
    mp_spin_zoom = new mySpinBox(this, 1, 1, 500, 100, "%");
    mp_slider_zoom = new myQSlider(this, 1, 1, 500, 100);
    // auto lable_light = new myQLabel(this, "<u>LIGHT</u>", Qt::AlignLeft, 8);
    // mp_spin_light = new mySpinBox(this, 1, 1, 255, 125);
    // mp_slider_light = new myQSlider(this, 1, 1, 255, 125);

    connect(mp_spin_zoom, SIGNAL(valueChanged(int)), this, SLOT(setZoomValue(int)));
    connect(mp_slider_zoom, SIGNAL(valueChanged(int)), this, SLOT(setZoomValue(int)));
    // connect(mp_spin_light, SIGNAL(valueChanged(int)), this, SLOT(setLightValue(int)));
    // connect(mp_slider_light, SIGNAL(valueChanged(int)), this, SLOT(setLightValue(int)));

    QGridLayout *pLayout_slider = new QGridLayout();
    pLayout_slider->addWidget(lable_slider, 1, 0, 1, 3);
    pLayout_slider->addWidget(lable_zoom, 2, 0, 1, 2);
    pLayout_slider->addWidget(mp_slider_zoom, 3, 0, 1, 2);
    pLayout_slider->addWidget(mp_spin_zoom, 3, 2, 1, 1);
    // pLayout_slider->addWidget(lable_light, 4, 0, 1, 2);
    // pLayout_slider->addWidget(mp_slider_light, 5, 0, 1, 2);
    // pLayout_slider->addWidget(mp_spin_light, 5, 2, 1, 1);
    pLayout_slider->setRowMinimumHeight(1, 32);

    return pLayout_slider;
}

QGridLayout *ImageInformationBar::initCheckBox()
{
    auto lable_check = new myQLabel(this, "<u>CHECK</u>", Qt::AlignCenter, 10);
    m_bayer = new QCheckBox("BP", this);
    m_bayer->setCheckState(Qt::Checked);
    checkBPTip();
    m_fit = new QCheckBox("FIT", this);
    m_fit->setCheckState(Qt::Unchecked);
    m_fit->setToolTip("勾选FIT将开启自适应大小");
    m_hard_format = new QCheckBox("HF", this);
    m_hard_format->setCheckState(Qt::Unchecked);
    m_hard_format->setToolTip("勾选HF将开启更加准确的格式检测, 但是将花费更多时间");
    m_auto_light = new QCheckBox("AL", this);
    m_auto_light->setCheckState(Qt::Unchecked);
    m_auto_light->setToolTip("勾选AL将使用自动亮度, 避免过暗/过亮导致看不清");
    m_stats_info = new QCheckBox("STAT", this);
    m_stats_info->setCheckState(Qt::Unchecked);
    m_stats_info->setToolTip("计算图像stats信息");

    connect(m_bayer, SIGNAL(stateChanged(int)), this, SLOT(setBayerShow(int)));
    connect(m_fit, SIGNAL(stateChanged(int)), this, SLOT(setFit(int)));
    connect(m_hard_format, SIGNAL(stateChanged(int)), this, SLOT(setHF(int)));
    connect(m_auto_light, SIGNAL(stateChanged(int)), this, SLOT(setAL(int)));
    connect(m_stats_info, SIGNAL(stateChanged(int)), this, SLOT(setStats(int)));

    QGridLayout *pLayout_check = new QGridLayout();
    pLayout_check->addWidget(lable_check, 1, 0, 1, 3);
    pLayout_check->addWidget(m_bayer, 2, 0, 1, 1);
    pLayout_check->addWidget(m_fit, 2, 1, 1, 1);
    pLayout_check->addWidget(m_hard_format, 2, 2, 1, 1);
    pLayout_check->addWidget(m_auto_light, 3, 0, 1, 1);
    pLayout_check->addWidget(m_stats_info, 3, 1, 1, 1);
    pLayout_check->setRowMinimumHeight(1, 32);

    return pLayout_check;
}

QGridLayout *ImageInformationBar::initViewBox()
{
    QFont font("黑体", 8, QFont::Normal);

    m_gain = new mySpinBox(this, 1, 1, 99999, 0);
    m_lscfile = new QLineEdit(this);
    m_apply_btn = new QPushButton(tr("应用"), this);
    connect(m_apply_btn, SIGNAL(clicked()), this, SLOT(OnApplyBtnClicked()));
    m_apply_btn->setFont(font);
    m_gain->setFont(font);
    m_lscfile->setFont(font);
    m_gain->setFixedHeight(22);
    m_lscfile->setFixedHeight(22);

    auto *lable_apply = new myQLabel(this, "<u>VIEW</u>", Qt::AlignCenter, 10);
    auto *lable_gain = new myQLabel(this, "gain", Qt::AlignLeft);
    auto *lable_lsc = new myQLabel(this, "lsc", Qt::AlignLeft);

    QGridLayout *pLayout_apply = new QGridLayout();
    pLayout_apply->addWidget(lable_apply, 0, 0, 1, 3);
    pLayout_apply->addWidget(lable_gain, 1, 0, 1, 1);
    pLayout_apply->addWidget(m_gain, 1, 1, 1, 2);
    pLayout_apply->addWidget(lable_lsc, 2, 0, 1, 1);
    pLayout_apply->addWidget(m_lscfile, 2, 1, 1, 2);
    pLayout_apply->addWidget(m_apply_btn, 3, 0, 1, 3);
    pLayout_apply->setRowMinimumHeight(0, 32);
    pLayout_apply->setRowMinimumHeight(3, 32);

    return pLayout_apply;
}

QGridLayout *ImageInformationBar::initFormatBox()
{
    QFont font("黑体", 8, QFont::Normal);

    m_type = createTypeComboBox(m_supportedFormats);
    m_width = new mySpinBox(this, 1, 1, 99999, 0);
    m_height = new mySpinBox(this, 1, 1, 99999, 0);
    m_stride = new mySpinBox(this, 1, 0, 99999, 0);
    m_align = new mySpinBox(this, 1, 0, 99999, 0); //只用来显示，=stride-width

    m_process_btn = new QPushButton(tr("提交"), this);
    connect(m_process_btn, SIGNAL(clicked()), this, SLOT(OnProcessBtnClicked()));
    // connect(m_height, SIGNAL(valueChanged(const int &)), this, SLOT(updateHeight()));
    connect(m_width, SIGNAL(valueChanged(const int &)), this, SLOT(updateWidth()));
    connect(m_stride, SIGNAL(valueChanged(const int &)), this, SLOT(updateStride()));

    m_type->setFont(font);
    m_align->setDisabled(true);
    m_process_btn->setFont(font);
    auto *lable_title = new myQLabel(this, "<u>FORMAT<u>", Qt::AlignCenter, 10);
    auto *lable_s = new myQLabel(this, "stride", Qt::AlignLeft);
    auto *lable_align = new myQLabel(this, "align", Qt::AlignLeft);
    auto *lable_h = new myQLabel(this, "height", Qt::AlignLeft);
    auto *lable_w = new myQLabel(this, "width", Qt::AlignLeft);

    m_exchange_btn = new QPushButton(this);
    m_exchange_btn->setFont(font);
    m_exchange_btn->setMaximumSize(20, 20);
    m_exchange_btn->setIcon(QIcon(":/statics/hwchange.png"));
    connect(m_exchange_btn, SIGNAL(clicked()), this, SLOT(exchangeHW()));

    m_flock_btn = new QPushButton(this);
    m_flock_btn->setFont(font);
    m_flock_btn->setMaximumSize(20, 20);
    m_flock_btn->setIcon(QIcon(":/statics/unlock.png"));
    m_flock_btn->setToolTip(tr("允许自动推导格式"));
    connect(m_flock_btn, SIGNAL(clicked()), this, SLOT(updateflock()));

    QGridLayout *pLayout_format = new QGridLayout();
    pLayout_format->setRowMinimumHeight(0, 36);
    pLayout_format->addWidget(lable_title, 0, 0, 1, 3);
    pLayout_format->addWidget(m_type, 1, 0, 1, 3);
    pLayout_format->addWidget(lable_h, 2, 0, 1, 1);
    pLayout_format->addWidget(lable_w, 2, 2, 1, 1);
    pLayout_format->addWidget(m_height, 3, 0, 1, 1);
    pLayout_format->addWidget(m_exchange_btn, 3, 1, 1, 1);
    pLayout_format->addWidget(m_width, 3, 2, 1, 1);
    pLayout_format->addWidget(lable_align, 4, 0, 1, 1);
    pLayout_format->addWidget(lable_s, 4, 2, 1, 1);
    pLayout_format->addWidget(m_align, 5, 0, 1, 1);
    pLayout_format->addWidget(m_stride, 5, 2, 1, 1);

    QGridLayout *pLayout_format_bnt = new QGridLayout();
    pLayout_format_bnt->addWidget(m_flock_btn, 0, 0, 1, 1);
    pLayout_format_bnt->addWidget(m_process_btn, 0, 1, 1, 4);
    pLayout_format->setRowMinimumHeight(6, 36);
    pLayout_format->addLayout(pLayout_format_bnt, 6, 0, 1, 3);

    return pLayout_format;
}

BOOL ImageInformationBar::checkBPTip()
{
    BOOL ret = false;
    QString tip;
    if (m_bayer->checkState() == Qt::Checked)
    {
        tip = "显示Bayer格式";
    }
    else
    {
        tip = "显示RGB格式";
    }

    auto type = m_type->currentText();
    if (type.contains("BAYER", Qt::CaseInsensitive) ||
        type.contains("MIPI", Qt::CaseInsensitive))
    {
        ret = true;
    }
    else
    {
        if (m_bayer->checkState() == Qt::Checked)
        {
            tip = "显示Bayer格式, 对" + type + "无效";
        }
        ret = false;
    }
    m_bayer->setToolTip(tip);
    return ret;
}

void ImageInformationBar::updateFit()
{
    setFit(m_fit->checkState());
}

void ImageInformationBar::resetFit(const Qt::CheckState &fit)
{
    m_fit->setCheckState(fit);
}

void ImageInformationBar::setZoom(const int &zoom)
{
    mp_spin_zoom->setValue(zoom);
    mp_slider_zoom->setValue(zoom);
}

void ImageInformationBar::setZoomValue(const int &zoom)
{
    mp_spin_zoom->setValue(zoom);
    mp_slider_zoom->setValue(zoom);
    if (zoom != mainwindow->getZoom())
        mainwindow->setZoomAction(zoom);
}

void ImageInformationBar::setLightValue(const int &light)
{
    mp_spin_light->setValue(light);
    mp_slider_light->setValue(light);

    mainwindow->setLight(light);
    // if (zoom != mainwindow->getZoom())
    //     mainwindow->setZoomAction(zoom);
}

void ImageInformationBar::setBayerShow(const int &bp)
{
    if (checkBPTip())
    {
        mainwindow->setBP((bool)bp);
    }
}

void ImageInformationBar::setFit(const int &fit)
{
    if (fit != Qt::Checked)
        return;

    QSize visiable_size = mainwindow->getImgViewSize() * 0.95;
    QSize img_size = mainwindow->getImgLabelSize();

    float ratio = min((float)visiable_size.width() / (float)img_size.width(), (float)visiable_size.height() / (float)img_size.height());
    int zoom = ratio * ratio * 100;
    setZoom(zoom);
    resetFit(Qt::Checked);
}

void ImageInformationBar::resetAL(const Qt::CheckState &fit)
{
    m_auto_light->setCheckState(fit);
}

void ImageInformationBar::setAL(const int &al)
{
    if (al == Qt::Checked)
    {
        auto auto_light = mainwindow->getAL();
        mainwindow->setLight(auto_light);
    }
    else if (al == Qt::Unchecked)
    {
        mainwindow->setLight(-1);
    }
}

void ImageInformationBar::calStats()
{
    getStats(m_stats, mainwindow->getImage());
    setStats(m_stats);
    mainwindow->getImageViewer()->setStats(m_stats);
}

void ImageInformationBar::setStats(const STATS_INFO &stats)
{
    m_stats = stats;
    m_stats_ave_y->setText(QString(std::to_string(m_stats.ave_y).c_str()));
}

void ImageInformationBar::setStats(const int &st)
{
    auto stats = m_layout->itemAt(static_cast<int>(PART::STATS));
    if (st == Qt::Checked)
    {
        stats->widget()->setVisible(true);
    }
    else if (st == Qt::Unchecked)
    {
        stats->widget()->setVisible(false);
    }
}

STR ImageInformationBar::justSetHF()
{
    REQUESTINFO raw_req;
    std::string raw_type;
    FORMAT format;
    format.width = m_width->value();
    format.height = m_height->value();
    format.stride = m_stride->value();

    char cfilename[1024];
    memcpy(cfilename, m_filename.toStdString().c_str(), m_filename.size() + 1);
    raw_req.ifilename = cfilename;
    raw_req.iformat = format;
    raw_req.ofilename = "-";
    raw_req.oformat = format;
    raw_req.bayer = false;
    raw_type = m_type->currentText().toStdString();

    STR parsed_format = cvt_parseFormat(raw_type, raw_req);

    if (parsed_format.empty())
    {
        return "";
    }
    m_type->setCurrentText(QString::fromStdString(parsed_format));

    return parsed_format;
}

void ImageInformationBar::setHF(const int &fit)
{
    if (fit != Qt::Checked)
        return;

    STR type = justSetHF();
    if (type.empty())
        return;

    mainwindow->loadImageResource();
}

QComboBox *ImageInformationBar::createTypeComboBox(const QStringList &supportedFormats)
{
    QComboBox *format_combo_box = new QComboBox(this);
    format_combo_box->addItems(supportedFormats);
    format_combo_box->setMaximumSize(200, 40);
    // format_combo_box->setMaxVisibleItems(5);

    return format_combo_box;
}

IMAGEINFO ImageInformationBar::value()
{
    FORMAT format;
    format.width = m_width->value();
    format.height = m_height->value();
    format.stride = m_stride->value();

    IMAGEINFO ret(m_type->currentText().toStdString(), format);
    autocomplete_add(ret);

    checkBPTip();
    ret.bayer = (bool)m_bayer->checkState();

    return ret;
}

IMAGEINFO ImageInformationBar::genIMAGEINFO()
{
    FORMAT format;
    format.width = m_width->value();
    format.height = m_height->value();
    format.stride = m_stride->value();
    string type = m_type->currentText().toStdString();
    IMAGEINFO gen(type, format);

    return gen;
}

static vector<string> types_str;
void ImageInformationBar::parse(const QString &filename)
{
    if (m_flock)
        return;

    m_exchange_stride = -1.0f;
    m_filename = filename;
    QFileInfo info = QFileInfo(filename);
    QString suffix = info.suffix();
    if (suffix == "jpg" || suffix == "bmp" || suffix == "png" || suffix == "gif" || suffix == "jpeg")
    {
        LOGI("using picture type: {}", suffix.toStdString());
        return;
    }

    IMAGEINFO predict = genIMAGEINFO();

    FILEINFO fileinfo = {
        info.size(),
        info.suffix().toStdString(),
        info.fileName().toStdString()};

    if (types_str.empty())
    {
        for (auto type : m_supportedFormats)
        {
            types_str.emplace_back(type.toStdString());
        }
    }
    RESULT ret = autocomplete_fileparse(predict, fileinfo, types_str);

    if (RESULT::SUCCESS == ret)
    {
        set(QString::fromStdString(predict.type), predict.format.height, predict.format.width, predict.format.stride);
    }

    //如果开启硬自动格式
    if (m_hard_format->checkState() == Qt::Checked)
    {
        justSetHF();
    }

    LOGI("auto parser run.");
}

void ImageInformationBar::set(const QString &type, const int &height, const int &width, const int &stride)
{
    m_height->setValue(height);
    m_width->setValue(width);
    m_stride->setValue(stride);

    if (!type.isEmpty())
    {
        m_type->setCurrentText(type);
        LOGI("using raw type: {}", type.toStdString());
    }
}

void ImageInformationBar::setInfo(const IMAGEINFO &info)
{
    m_height->setValue(info.format.height);
    m_width->setValue(info.format.width);
    m_stride->setValue(info.format.stride);

    if (!info.type.empty())
        m_type->setCurrentText(QString::fromStdString(info.type));
}

void ImageInformationBar::OnProcessBtnClicked()
{
    emit processBtnClicked(value());
}
void ImageInformationBar::OnApplyBtnClicked()
{
    emit processBtnClicked(value());
}

void ImageInformationBar::updateHeight()
{
    int test = m_height->value() / 10;
    if ((test == m_last_height) && (test / 10 > 0))
    {
        IMAGEINFO predict = genIMAGEINFO(), src = genIMAGEINFO();
        RESULT ret = autocomplete_format(predict, src, SOURCETYPE::HEIGHT);

        if (RESULT::SUCCESS == ret)
        {
            set(QString::fromStdString(predict.type), predict.format.height, predict.format.width, predict.format.stride);
        }
    }

    m_last_width = m_width->value();
    m_last_height = m_height->value();
    m_last_stride = m_stride->value();
}
void ImageInformationBar::updateWidth()
{
    // if (((m_width->value()) / 10 == m_stride->value()) || m_width->text().isEmpty() || m_stride->text().isEmpty())
    // {
    //     m_stride->setValue(m_width->value());
    // }

    // int test = m_width->value() / 10;
    // if ((test == m_last_width) && (test / 10 > 0))
    // {
    //     IMAGEINFO predict = genIMAGEINFO(), src = genIMAGEINFO();
    //     RESULT ret = autocomplete_format(predict, src, SOURCETYPE::WIDTH);

    //     if (RESULT::SUCCESS == ret)
    //     {
    //         set(QString::fromStdString(predict.type), predict.format.height, predict.format.width, predict.format.stride);
    //     }
    // }

    // m_last_width = m_width->value();
    // m_last_height = m_height->value();
    // m_last_stride = m_stride->value();
    m_align->setValue(m_stride->value() - m_width->value());
}
void ImageInformationBar::updateStride()
{
    // if (((m_stride->value()) / 10 == m_width->value()) || m_width->text().isEmpty() || m_stride->text().isEmpty())
    // {
    //     m_width->setValue(m_stride->value());
    // }

    // int test = m_stride->value() / 10;
    // if ((test == m_last_stride) && (test / 10 > 0))
    // {
    //     IMAGEINFO predict = genIMAGEINFO(), src = genIMAGEINFO();
    //     RESULT ret = autocomplete_format(predict, src, SOURCETYPE::STRIDE);

    //     if (RESULT::SUCCESS == ret)
    //     {
    //         set(QString::fromStdString(predict.type), predict.format.height, predict.format.width, predict.format.stride);
    //     }
    // }

    // m_last_width = m_width->value();
    // m_last_height = m_height->value();
    // m_last_stride = m_stride->value();
    m_align->setValue(m_stride->value() - m_width->value());
}

void ImageInformationBar::exchangeHW()
{
    int width = m_width->value();
    int height = m_height->value();
    float stride = m_exchange_stride > 0 ? m_exchange_stride : m_stride->value();
    float size = stride * height;
    m_exchange_stride = size / width;

    m_width->setValue(height);
    m_height->setValue(width);
    m_stride->setValue(m_exchange_stride + 0.5);
    updateStride();

    LOGI("exchange width and height {} {} {}", height, width, m_exchange_stride);
}

void ImageInformationBar::updateflock()
{
    m_flock = !m_flock;

    if (m_flock)
    {
        m_flock_btn->setIcon(QIcon(":/statics/lock.png"));
        m_flock_btn->setToolTip(tr("不允许自动推导格式"));
    }
    else
    {
        m_flock_btn->setIcon(QIcon(":/statics/unlock.png"));
        m_flock_btn->setToolTip(tr("允许自动推导格式"));
    }
}