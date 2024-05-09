#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include "changecolorform.h"
#include "lyricwidget.h"
#include <QPainter>
#include <QDebug>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QtMultimedia>
#include <QtMultimediaWidgets>
#include <QSettings>
#include "localmusic.h"
#include "audio.h"
#include <QTextToSpeech>



QT_BEGIN_NAMESPACE
class QAudioOutput;
class localMusic;
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void getToplistImg(QString coverImgUrl, QPushButton *pushButton);
    void getMvlistImg(QString coverImgUrl, QPushButton *pushButton);
    void getLyric(QString id);
    void createPlaylist(double id);
    void createWidgetToplist();
    void displayErrorMessage(QString message);
    void searchSinger(QString singerName);
    void setSinger();
    void GetplayPath();
    void DataBaseInit();
    void SaveplayPath();
    void setLocalmusiclist();
    void setMylovelist();
    QList<QMap<QString, QString>> loadMyloveSongs();
    void onMusicListSelected();
    void createMusiclistlayout();
    void loadPlaylistNames();
    void settableWidgetPlayListlayout();
    void removeSongFromDatabase(const QString &id, const QString &tableName);
    void speechconnect();

protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);



private slots:

    void on_horizontalSliderMVvolume_valueChanged(int value);
    void onSingerButtonClicked();
    //void on_pushButtonList_clicked();
    void on_pushButtonLastsong_clicked();
    void on_pushButtonNextsong_clicked();
    void on_pushButtonPlay_clicked();
    void on_pushButtonSound_clicked();
    void on_verticalSliderVolumn_valueChanged(int value);
    void on_pushButtonSearch_clicked();
    void on_pushButtoncloth_clicked();
    void on_pushButtonFindmusic_clicked();
    void on_pushButtonMV_clicked();
    void durationChange(qint64 d);
    void positionChange(qint64 p);
    void playSong(int row);
    void showHideLyric(bool on);
    void hideLyric();
    void dialogSet();
    void download(QString surl, QString filepath);
    void dialogDownload();
    void openDownloadPath();
    void chooseDownloadPath();
    void on_pushButtonLastpage_clicked();
    void on_pushButtonNextpage_clicked();
    void on_pushButtonLoop_clicked(bool checked);
    void onMediaStatusChangedSingleLoop(QMediaPlayer::MediaStatus status);
    void onMediaStatusChangedListLoop(QMediaPlayer::MediaStatus status);
    void on_pushButtonaddMylove_clicked(bool checked);
    void addMyloveSongs(QString songName, QString artist, QString album,int id, int duration);
    void addMyloveSongToPlayList(QTableWidgetItem* item);
    void addMycreateSongToPlayList(QTableWidgetItem* item);
    void tablecustomContextMenuRequest(const QPoint &pos);
    void on_pushButtonAddmusiclist_clicked();
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void listWidgettablecustomContextMenuRequest(const QPoint &pos);
    void MycreatecustomContextMenuRequest(const QPoint &pos);
    void MylovecustomContextMenuRequest(const QPoint &pos);
    void on_pushButtonPlayall_clicked();
    void on_pushButtonPlayallMycreate_clicked();
    void on_pushButtonMVplay_clicked();
private:
    Ui::Widget *ui;
    QList<QUrl>playList;//本地播放列表
    QList<QUrl>onlineplayList;
    /*换肤的界面*/
    ChangeColorForm * changeColor = nullptr; //换肤
    selectDialog *selectDialog;//目录选择
    localMusic *localmusic = nullptr;//本地音乐
    QAudioOutput *audioOutput = nullptr;
    QAudioOutput *MVaudioOutput = nullptr;
    LyricWidget *lyricWidget=nullptr;//歌词
    Audio *audio = nullptr;
    QTextToSpeech *myspeech = nullptr;
    int currentPageIndex = 0;
    struct Lyric
    {
        QTime time;
        QString sentence;
    };
    QList<Lyric> lyrics;
    QSettings settings;
    QMediaPlayer *mediaPlayer = nullptr;
    QMediaPlayer *VideoPlayer = nullptr;
    QVideoWidget *videoWidget;
    int curPlayIndex=0;
    QNetworkAccessManager* manager;
    int id,musicDuration,mvId;
    QString musicName,singerName,albumName;
    QByteArray searchInfo;
    QByteArray getReply(QString surl);
    QByteArray postReply(QString surl,QString spost);//由QString改为QByteArray
    QUrl constructMVUrl(int mvId);
    QUrl constructSongUrl(int songId);
    QLineEdit *lineEdit_downloadPath;
    QString downloadPath;
    QPushButton *pushButton_path,*pushButton_font, *pushButton_fontcolorleft, *pushButton_fontcolorright;
    QListWidget *musicListWidget; // 歌单列表的指针
    QTableWidget *createmusicTable;
    QPushButton *createmusicplayall,creatematchMusic;
    QTimer *clearTimer;
};


#endif // WIDGET_H
