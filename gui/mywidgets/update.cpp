#include <QWidget>
#include <QCoreApplication>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QDesktopServices>

#include "update.h"
#include "autointerface.h"

Update::Update(QWidget *parent) : QWidget(parent)
{
    vector<string> version = autocomplete_load(CONFIG_TYPE::VERSION);
    m_version = QString::fromStdString(version[0]);
    m_ignoreversion = QString::fromStdString(version[1]);
    mp_manager = new QNetworkAccessManager(this);
    QObject::connect(mp_manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)), Qt::DirectConnection);
}
Update::~Update() {}

bool Update::check(const bool &ignore)
{
    m_ignore = ignore;
    QNetworkRequest request;
    request.setUrl(QUrl("https://git.n.xiaomi.com/caibingcheng/nyuv/snippets/552/raw"));
    request.setHeader(QNetworkRequest::UserAgentHeader, "RT-Thread ART");
    QNetworkReply *reply = mp_manager->get(request);
}

void Update::replyFinished(QNetworkReply *reply)
{
    QString str = reply->readAll();
    parseUpdateJSON(str);
    reply->deleteLater();
}

int Update::parseUpdateJSON(QString str)
{
    QJsonParseError err_rpt;
    QJsonDocument root_Doc = QJsonDocument::fromJson(str.toUtf8(), &err_rpt); //字符串格式化为JSON
    if ((err_rpt.error != QJsonParseError::NoError) && (!m_ignore))
    {
        QMessageBox::critical(this, "检查失败", "服务器地址错误或JSON格式错误!");
        return -1;
    }
    if (root_Doc.isObject())
    {
        QJsonObject root_Obj = root_Doc.object();
        QJsonObject version_value = root_Obj.value("nyuv").toObject();
        QString verison = version_value.value("LatestVerison").toString();
        QString url = version_value.value("Url").toString();
        QString update_time = version_value.value("UpdateTime").toString();
        QString release_note = version_value.value("ReleaseNote").toString();
        if (verison > m_version)
        {
            if (m_ignore && !(verison > m_ignoreversion))
                return 0;
            QString warning = "当前版本：" + m_version + "\n检测到新版本!\n版本号：" + verison + "\n" + "更新时间：" + update_time + "\n" + "更新说明：" + release_note;
            int ret = QMessageBox::question(this, "检查更新", warning, "下载", "忽略");
            if (ret == 0)
            {
                ret = QMessageBox::question(this, "退出", "确认将会退出程序", "确认", "取消");
                if (ret == 0)
                {
                    QDesktopServices::openUrl(QUrl(url));
                    exit(0);
                }
            }
            else if (m_ignore)
            {
                m_ignoreversion = verison;
                vector<string> version_dump;
                version_dump.emplace_back(m_version.toStdString());
                version_dump.emplace_back(m_ignoreversion.toStdString());
                autocomplete_dump(version_dump, CONFIG_TYPE::VERSION);
            }
        }
        else if (!m_ignore)
        {
            QMessageBox::information(this, "检查更新", "当前版本：" + m_version + "\n已是最新版本!");
        }
    }
    return 0;
}
