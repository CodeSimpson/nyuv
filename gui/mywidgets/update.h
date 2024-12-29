#ifndef __UPDATE_H__
#define __UPDATE_H__

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

class Update : public QWidget
{    
    Q_OBJECT
public:
    explicit Update(QWidget *parent = 0);
    ~Update();

    bool check(const bool &ignore = false);

private slots:
    void replyFinished(QNetworkReply *);

private:
    int parseUpdateJSON(QString str);

private:
    QNetworkAccessManager *mp_manager;
    QString m_version;
    QString m_ignoreversion;
    bool m_ignore;
};

#endif