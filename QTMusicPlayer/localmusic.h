#ifndef STACK_LOCALMUSIC_H
#define STACK_LOCALMUSIC_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSpacerItem>
#include <QTabWidget>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QScrollArea>
#include <QHeaderView>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QDebug>
#include <QApplication>
#include <QToolButton>
#include <QFileDialog>
#include <QVector>
#include <QScrollBar>
#include <QListWidget>
#include <QListWidgetItem>
#include <QBitmap>
#include <QPainter>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QMediaMetaData>
//#include "dbManager.h"
#include <QSqlRecord>
#include <QGraphicsDropShadowEffect>


enum
{
    LOCALMUSIC = 0,
    NETWORKMUSIC = 1,
};


struct MediaObjectInfo
{
    //用于保存歌曲名
    QString fileName;
    //用于保存歌曲文件路径
    QString filePath;
    //用于保存歌手名
    QString singer;
    //用于保存歌曲大小
    QString filesize;
    //用于保存歌曲播放时间
    QString playtime;
    //保存专辑信息
    QString AlbumName;
    //音乐来源
    int songSource;
    //音乐ID
    QString songId;
    //歌手照片
    QString SingerImage;
    //热度
    int hotnum;

    friend bool operator==(const MediaObjectInfo &obj1,const MediaObjectInfo &obj2)
    {
        return (obj1.fileName == obj2.fileName &&
                obj1.filePath == obj2.filePath &&
                obj1.singer == obj2.singer &&
                obj1.filesize == obj2.filesize &&
                obj1.playtime == obj2.playtime &&
                obj1.AlbumName == obj2.AlbumName &&
                obj1.songSource == obj2.songSource &&
                obj1.songId == obj2.songId &&
                obj1.SingerImage == obj2.SingerImage);
    }
    //成员运算符重载函数
    bool operator==(const MediaObjectInfo &obj)
    {
        return (fileName == obj.fileName &&
                filePath == obj.filePath &&
                singer == obj.singer &&
                filesize == obj.filesize &&
                playtime == obj.playtime &&
                AlbumName == obj.AlbumName &&
                songSource == obj.songSource &&
                songId == obj.songId &&
                SingerImage == obj.SingerImage);
    }
};

class selectDialog;

class localMusic : public QWidget
{
    Q_OBJECT
public:
    explicit localMusic(QWidget *parent = nullptr);

    ~localMusic(){};
    friend class Widget;
private:
    void localMusicLayout(void);
    void songInfoLayout(void);
    void ConnectInit(void);
    void LoadStyleSheet(void);
    void DataBaseInit(void);
    void GetplayPath(void);
    void SaveplayPath(void);

    void TabWidgetLoad(void);
    QUrl getUrlFromItem(QTableWidgetItem *item);

private:
    QLabel *displayLabel[2];
    //选择目录按钮
    QPushButton *selectDir;
    QPushButton *matchMusic;
    QVBoxLayout *local_VBoxLayout;
    //播放全部按钮
    QPushButton *playAll;

    //最上行标签以及按钮布局
    QHBoxLayout *display_HBoxLayout;


    //歌曲信息Tab页面
    QTabWidget *musicInfo_Widget;
    QWidget *song;//歌曲
    QWidget *singer;//歌手
    QWidget *album;//专辑
    QWidget *folder;//文件夹
    QTableWidget *music_Table;
    QScrollArea *music_ScrollArea;
    //存储对话框地址信息
    QList <QString> Dir_list;

    //媒体信息存储
    QList <MediaObjectInfo> mediaObjectInfo;
signals:
    void addall_signal( QList <MediaObjectInfo> *);
private slots:
    void table_customContextMenuRequest(const QPoint &pos);
    void btn_selectDirSlot(void);
    void btn_addlistclicked_slot(void);
    void btn_deleteclicked_slot(void);
};

class selectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit selectDialog(QWidget *parent = nullptr);
    selectDialog(QWidget *parent ,QList <QString> list);
    ~selectDialog() {};

    friend class localMusic;
private:
    QLabel *headLabel;
    QLabel *describLabel;
    //关闭对话框按钮
    QPushButton *closeButton;

    QScrollArea *Scroll_listArea;

    QPushButton *confirm_Button;
    QPushButton *addfolder_Button;

    QVBoxLayout *dialog_vLayout;

    //目录列表
    QList <QString> Dir_List;
    //目录列表List
    QListWidget *listPath;

private:
    void select_LayoutInit(void);
    void LoadStyleSheet(void);
    void ConnectInit(void);

signals:
    void confirm_clicked();
    void addfolder_clicked();
private slots:
    void confirmclicked_slot(void);
    void addfolder_slot(void);
    void btn_close_slot(void);
    void PathlistClicked_slot(QListWidgetItem *item);
protected:
    // //void showEvent(QShowEvent *);
    // void mousePressEvent(QMouseEvent *event);
    // void mouseMoveEvent(QMouseEvent *event);
    // void mouseReleaseEvent(QMouseEvent *event);
};












#endif // STACK_LOCALMUSIC_H
