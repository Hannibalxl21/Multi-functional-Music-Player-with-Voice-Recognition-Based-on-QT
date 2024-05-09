#ifndef SPEECH_H
#define SPEECH_H
#include <http.h>
#include <QObject>
#include <QByteArray>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QHostInfo>
#include <QDebug>

const QString BaiduSpeechUrl = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id=%1&client_secret=%2"; //获取token请求的Url
const QString BaiduSpeechClientID = "sF4aKsLBFbNUgBGcqBToSMyg"; //AK
const QString BaiduSpeechClientSecret = "XzdSnzsjmWYysmGuNw98vJY0LYLRY5kl";//SK

//语音识别的Url
const QString BaiduSpeechSatrtUrl = "http://vop.baidu.com/server_api?dev_pid=1537&cuid=%1&token=%2";//80001识别普通话  %1:本机标识 %2:获取的token
/*
 * 1.发送获取token值的Url，相当于向百度对接，获取通行证(token值)
 * 2.发送语音识别的Url，进行语音识别
 */


class Speech : public QObject
{
    Q_OBJECT
public:
    explicit Speech(QObject *parent = nullptr);

signals:

public slots:

public:
    QString speechIdentify(QString audioFile);//语音识别请求函数

private:
    QString getJsonValue(QByteArray &data, QString &key);//json解析函数

private:
    QString accessToken;//存放获取的token值
};

#endif // SPEECH_H
