#ifndef __MYINFOBAR_H__
#define __MYINFOBAR_H__

#include "stats.h"
#include "defines.h"
#include "autointerface.h"

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

class ImageInformationBar : public QFrame
{
    Q_OBJECT
public:
    explicit ImageInformationBar(const QStringList &supportedFormats, QWidget *parent = nullptr);

    IMAGEINFO value();
    void parse(const QString &filename);
    void setZoom(const int &);
    void setInfo(const IMAGEINFO &);
    void resetFit(const Qt::CheckState &);
    void resetAL(const Qt::CheckState &);
    void setStats(const STATS_INFO &);
    void updateFit();

private slots:
    /*
     * 功能：槽函数，发送信号processBtnClicked()
    */
    void OnProcessBtnClicked();
    void OnApplyBtnClicked();
    void updateHeight();
    void updateWidth();
    void updateStride();
    void exchangeHW();
    void updateflock();
    void set(const QString &type, const int &height, const int &width, const int &stride);
    IMAGEINFO genIMAGEINFO();
    void setZoomValue(const int &);
    void setBayerShow(const int &);
    void setLightValue(const int &);
    void setFit(const int &);
    void setHF(const int &);
    void setAL(const int &);
    void setStats(const int &);
    void calStats();
    STR justSetHF();

signals:
    void processBtnClicked(const IMAGEINFO &imageInfo);

private:
    QGridLayout *m_layout;

    QComboBox *m_type;
    QSpinBox *m_height;
    QSpinBox *m_width;
    QSpinBox *m_stride;
    QSpinBox *m_align;
    QSpinBox *m_gain;
    QLineEdit *m_lscfile;
    QCheckBox *m_bayer;
    QCheckBox *m_fit;
    QCheckBox *m_hard_format;
    QCheckBox *m_auto_light;
    QCheckBox *m_stats_info;

    QSpinBox *mp_spin_zoom;
    QSlider* mp_slider_zoom;
    QSpinBox *mp_spin_light;
    QSlider* mp_slider_light;

    int m_last_height;
    int m_last_width;
    int m_last_stride;
    float m_exchange_stride;
    bool m_flock;

    QLabel* m_stats_ave_y;

    QPushButton *m_process_btn;
    QPushButton *m_exchange_btn;
    QPushButton *m_flock_btn;
    QPushButton *m_apply_btn;

    QWidget *mp_parent;
    // AutoCompleteFormat m_format_auto;
    QStringList m_supportedFormats;
    QString m_filename;

    enum class PART{
        FORMAT,
        CHECK,
        SLIDER,
        STATS,
    };

    STATS_INFO m_stats;

private:
    /*
     * 功能：初始化图像格式信息栏Format窗口布局，返回Format窗口布局管理器
    */
    QGridLayout *initFormatBox();
    QGridLayout *initViewBox();
    /*
     * 功能：初始化check栏窗口布局
    */
    QGridLayout *initCheckBox();
    /*
     * 功能：初始化slider栏窗口布局，确定图像的缩放大小
    */
    QGridLayout *initSliderBox();
    /*
     * 功能：初始化数据统计stats栏布局
    */
    QGridLayout *initStatsBox();
    QComboBox *createTypeComboBox(const QStringList &supportedFormats);

    BOOL checkBPTip();

    /*
     * 功能：初始化图像格式信息栏布局
    */
    void initBox();
    /*
     * 功能：初始化并自动推导图片格式
    */
    void initFormat();
};

#endif