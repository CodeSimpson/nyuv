#include <QApplication>
#include <string>

#include "cvtinterface.h"
#include "autointerface.h"
#include "mainwindow.h"

#undef __LOGTAG__
#define __LOGTAG__ "MAIN"
#include "log.h"

using namespace std;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);                                                   // 创建应用程序对象

    vector<string> version = autocomplete_load(CONFIG_TYPE::VERSION);
    QGuiApplication::setApplicationName("nyuv");
    QGuiApplication::setApplicationVersion(QString::fromStdString(version[0]));

    QCommandLineParser parser;
    parser.setApplicationDescription(QGuiApplication::translate("main", "nyuv"));   // 设置应用程序描述信息
    parser.addHelpOption();
    parser.addVersionOption();

    parser.process(app);

    char *path_cstr = argv[1];                                                      // 执行./nyuv，argv[1]为空
    QString path_str = QString::fromLocal8Bit(path_cstr);
    QFileInfo file_info(path_str);
    if (!file_info.exists() && !path_str.isEmpty())                                 // 判断path_str是否不为空且file_info文件信息存在 
    {
        LOGE("invalid path {}", path_cstr);
        exit(1);
    }

    cvt_schedul_init();
    MainWindow mainWindow;                                                          // 新建一个主窗口

    if (file_info.exists())
    {
        mainWindow.setHome(file_info.absolutePath());
    }
    if (file_info.isFile())
    {
        mainWindow.fileTreeTriggered(file_info.absoluteFilePath());
    }
    mainWindow.setWindowIcon(QIcon(":/statics/logo.png"));
    mainWindow.setWindowState(Qt::WindowMaximized);
    mainWindow.show();                                                               // 显示主窗口
    int ret = app.exec();
    cvt_schedul_uninit();

    return ret;
}
