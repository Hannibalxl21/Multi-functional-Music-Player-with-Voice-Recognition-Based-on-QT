#include "http.h"

Http::Http(QObject *parent) : QObject(parent)
{

}

bool Http::http_postRequst(QString Url, QMap<QString, QString> header, QByteArray &requestData, QByteArray &replyData)
{
    QNetworkAccessManager manager; //请求者
    QNetworkRequest request;                //请求内容

    request.setUrl(Url);                            //获取token值时，参数只需要url和接收token值即可

    QMapIterator<QString, QString> it(header);
    while (it.hasNext()) {//判断header中是否有内容，并读取
        it.next();
        request.setRawHeader(it.key().toLatin1(),it.value().toLatin1());
    }

    /* 发送请求等待响应 */
    QNetworkReply *Reply = manager.post(request,requestData);//发送请求
    QEventLoop l;
    connect(Reply,&QNetworkReply::finished,&l,&QEventLoop::quit); //等待响应
    l.exec();//阻塞等待响应

    if(Reply != nullptr && Reply->error() == QNetworkReply::NoError)
    {
        replyData = Reply->readAll();//读取获取的语音识别结果
        return true;
    }
    return false;
}
