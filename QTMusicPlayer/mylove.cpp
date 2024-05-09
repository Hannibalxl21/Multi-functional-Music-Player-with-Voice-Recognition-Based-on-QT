#include "mylove.h"

MyLove::MyLove(QWidget *parent) : QWidget(parent)
{
    setMylovelayout();
    mylove_VBoxLayout = new QVBoxLayout();
    mylove_VBoxLayout->setContentsMargins(0,0,0,0);

    this->setLayout(mylove_VBoxLayout);
    this->setAttribute(Qt::WA_StyledBackground);



    //LoadStyleSheet();
    //要先加载完样式再加载子控件，因为我这里子Widget容器，和父容器用的同一份QSS样式，
    //子控件会自动继承父控件的样式，但是得父控件先加载完样式才行！！！
    TabMylovWidget();



}

void MyLove::addSong(const QString &song)
{
    if (!mylove_songs.contains(song)) {
        mylove_songs.append(song);
    }
}

void MyLove::removeSong(const QString &song)
{
    mylove_songs.removeOne(song);
}

QVector<QString> MyLove::getAllSongs()
{
    return mylove_songs;
}

void MyLove::setMylovelayout()
{
    song = new QWidget();
    QVBoxLayout *song_VLayout = new QVBoxLayout();

    playAllmylove = new QPushButton();
    playAllmylove->setText("播放全部");
    playAllmylove->setMaximumSize(120,35);
    playAllmylove->setMinimumSize(120,35);

    playAllmylove->setIcon(QIcon(":/images/stackWidget/localMusic/btn_playall.png"));

    /*QAction *addAllAction = new QAction(playAll);
    addAllAction->setIcon(QIcon(":/images/stackWidget/localMusic/btn_playalladd.png"));
    playAll->addAction(addAllAction);*/




    QSpacerItem *Button_HSpacer = new QSpacerItem(200,20,
                                                  QSizePolicy::Expanding,
                                                  QSizePolicy::Expanding);


    QHBoxLayout *button_HBoxLayout = new QHBoxLayout();
    button_HBoxLayout->addWidget(playAllmylove);
    button_HBoxLayout->addSpacerItem(Button_HSpacer);


    Mylovemusic_Table = new QTableWidget();
    Mylovemusic_Table->setMinimumSize(750,500);
    Mylovemusic_Table->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Expanding);
    Mylovemusic_Table->setObjectName("table_music");
    QList <QString> tableList;
    tableList << "" << "音乐标题" << "歌手" << "专辑" << "时长" << "大小";
    Mylovemusic_Table->setColumnCount(6);
    Mylovemusic_Table->setHorizontalHeaderLabels(tableList);
    Mylovemusic_Table->setColumnWidth(0,10);
    Mylovemusic_Table->setColumnWidth(1,200);
    Mylovemusic_Table->setColumnWidth(2,100);
    Mylovemusic_Table->setColumnWidth(3,250);
    Mylovemusic_Table->setColumnWidth(4,70);
    Mylovemusic_Table->setColumnWidth(5,70);
    //打开右键菜单属性
    Mylovemusic_Table->setContextMenuPolicy(Qt::CustomContextMenu);
    //开启交替行背景色
    Mylovemusic_Table->setAlternatingRowColors(true);
    //设置表格内容不可编辑
    Mylovemusic_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置为行选择
    Mylovemusic_Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    //设置最后一栏自适应长度
    Mylovemusic_Table->horizontalHeader()->setStretchLastSection(true);
    //删除网格表
    Mylovemusic_Table->setShowGrid(false);
    //去除边框线
    Mylovemusic_Table->setFrameShape(QFrame::NoFrame);
    //去除选中虚线框
    Mylovemusic_Table->setFocusPolicy(Qt::NoFocus);
    //设置点击表头进行排序
    Mylovemusic_Table->setSortingEnabled(true);
    //设置表头字体向左靠齐
    Mylovemusic_Table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    //设置点击单元格时，表头项不高亮
    Mylovemusic_Table->horizontalHeader()->setHighlightSections(false);
    Mylovemusic_Table->verticalHeader()->setHighlightSections(false);
    //设置只能选中一个目标
    Mylovemusic_Table->setSelectionMode(QAbstractItemView::SingleSelection);
    //设置垂直滚动条最小宽度
    //music_Table->verticalScrollBar()->setMaximumWidth(7);

    Mylovemusic_Table->verticalHeader()->setAlternatingRowColors(true);
    //music_Table->verticalHeader()->setObjectName("music_verticalHeader");
    Mylovemusic_Table->verticalHeader()->setDefaultSectionSize(30);
    Mylovemusic_Table->verticalHeader()->setVisible(false);
    Mylovemusic_Table->setStyleSheet("QTableView::item:selected { color:white; background:rgb((85,170,255));}"
                               "QTableCornerButton::section { background-color:#ecf5ff; }"
                               "QHeaderView::section { color:black; background-color:#ecf5ff; }"
                               "QTableWidget { border: none; }"
                               );

    song_VLayout->addLayout(button_HBoxLayout);
    song_VLayout->addWidget(Mylovemusic_Table);

    song->setLayout(song_VLayout);
}

void MyLove::TabMylovWidget()
{
    QWidget *mylovesong = new QWidget();
    QWidget *mylovemv = new QWidget();


    //歌曲Tab界面初始化
    setMylovelayout();


    //歌曲信息Tab页
    musicInfo_Widget = new QTabWidget();
    musicInfo_Widget->setObjectName("musicIndo_TabWidget");
    musicInfo_Widget->setMinimumSize(800,400);
    musicInfo_Widget->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
    musicInfo_Widget->addTab(mylovesong,"歌曲");
    musicInfo_Widget->addTab(mylovemv,"视频");
    mylove_VBoxLayout->addSpacing(20);
    mylove_VBoxLayout->addLayout(display_HBoxLayout);
    mylove_VBoxLayout->addWidget(musicInfo_Widget);
}
