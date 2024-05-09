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
class MyLove : public QWidget
{
    Q_OBJECT
public:
    explicit MyLove(QWidget *parent = nullptr);

    void addSong(const QString &song);
    void removeSong(const QString &song);
    QVector<QString> getAllSongs();
    QLabel *displayLabel;
    //选择目录按钮


    //播放全部按钮
    QPushButton *playAllmylove;

    //最上行标签以及按钮布局
    QHBoxLayout *display_HBoxLayout;
    QVBoxLayout *mylove_VBoxLayout;


    //歌曲信息Tab页面

    QTabWidget *musicInfo_Widget;
    QWidget *song;//歌曲

    QTableWidget *Mylovemusic_Table;
    QScrollArea *Mylovemusic_ScrollArea;


    ;
private:
    QVector<QString> mylove_songs;
    void setMylovelayout();
    void TabMylovWidget();
};

