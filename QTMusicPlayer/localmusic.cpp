#include "localMusic.h"
#include <QFile>
#include <QTableWidget>
#include"dbManager.h"


localMusic::localMusic(QWidget *parent):
    QWidget(parent)
{
    //this->setMinimumSize(800,530);
    //this->setObjectName("localmusic");

    local_VBoxLayout = new QVBoxLayout();
    local_VBoxLayout->setContentsMargins(0,0,0,0);
    this->setLayout(local_VBoxLayout);
    this->setAttribute(Qt::WA_StyledBackground);

    localMusicLayout();

    //LoadStyleSheet();
    //要先加载完样式再加载子控件，因为我这里子Widget容器，和父容器用的同一份QSS样式，
    //子控件会自动继承父控件的样式，但是得父控件先加载完样式才行！！！
    TabWidgetLoad();

    ConnectInit();
    DataBaseInit();


}



void localMusic::DataBaseInit(void)
{
    QString sql = "create table if not exists selectPath"
                  "(path STRING NOT NULL);";

    dbManager::excute(sql,"selectPath");

    QSqlQuery *query = nullptr;
    sql = QString("select * from selectPath;");
    query = dbManager::select(sql, "selectPath");

    if(query->first())
    {
        while (query->next())
        {
            //qDebug() << query->record().indexOf("path");
            QSqlRecord record = query->record();
            int FieldNo = record.indexOf("path");
            //qDebug() << record.value(FieldNo).toString();

            Dir_list.append(record.value(FieldNo).toString());
        }
    }
    else
    {
        Dir_list.append("我的音乐");
        QDir dir(QCoreApplication::applicationDirPath() + "/mp3");
        QDir dirbsolutePath(dir.absolutePath());
        Dir_list.append(dirbsolutePath.path());

        // ODBC风格批量插入步骤 1预处理插入占位符 2用可变列表给占位符赋值 3添加绑定  4执行预处理
        //预处理插入占位符
        sql = "insert into selectPath (path) values(?);";
        query = dbManager::select(sql, "selectPath");

        QStringList pathlist = this->Dir_list;

        query->addBindValue(pathlist);
        query->execBatch();

        //sql = "insert into selectPath (path) values(:path);";

        //query = dbManager::select(sql, "selectPath");

        //for (int i=0; i<Dir_list.count(); i++)
        //{
        //query->bindValue(":path",Dir_list.at(i));
        //query->bindValue(0,Dir_list.at(i));
        //query->exec();
        //}
    }

    delete query;
}

void localMusic::GetplayPath(void)
{
    Dir_list.clear();

    QString sql = "select * from selectPath;";

    QSqlQuery *query = nullptr;

    query = dbManager::select(sql, "selectPath");

    if (query->first())
    {
        do
        {
            Dir_list.append(query->value(0).toString());
        }
        while (query->next());
    }
    delete query;
}

void localMusic::SaveplayPath(void)
{
    QString sql;

    for (int i=0; i<Dir_list.count(); i++)
    {
        QSqlQuery *query = nullptr;
        sql = QString("select * from selectPath where path = '%1';").arg(Dir_list.at(i));
        query = dbManager::select(sql, "selectPath");
        if (!query->first())    //查询数据库中是否存在该路径，若不存在，则存入数据库中
        {
            sql = QString("insert into selectPath (path) values('%1');").arg(Dir_list.at(i));
            dbManager::excute(sql, "selectPath");
        }
        delete query;
    }
}

void localMusic::localMusicLayout(void)
{
    display_HBoxLayout = new QHBoxLayout();

    displayLabel[0] = new QLabel();
    displayLabel[0]->setText("本地音乐");
    displayLabel[0]->setObjectName("localLabel");
    displayLabel[1] = new QLabel();
    displayLabel[1]->setText("共0首");
    displayLabel[1]->setObjectName("numberlabel");

    selectDir = new QPushButton();
    selectDir->setText("选择目录");
    selectDir->setObjectName("selectButton");
    selectDir->setCursor(Qt::PointingHandCursor);

    QSpacerItem *display_hSpacer = new QSpacerItem(200,10,
                                                   QSizePolicy::Expanding,
                                                   QSizePolicy::Expanding);

    display_HBoxLayout->addWidget(displayLabel[0]);
    display_HBoxLayout->addWidget(displayLabel[1]);
    display_HBoxLayout->addSpacerItem(display_hSpacer);
    display_HBoxLayout->addWidget(selectDir);
    display_HBoxLayout->addSpacing(30);
}

void localMusic::TabWidgetLoad(void)
{


    //歌曲Tab界面初始化
    songInfoLayout();


    //歌曲信息Tab页
    musicInfo_Widget = new QTabWidget();
    musicInfo_Widget->setObjectName("musicIndo_TabWidget");
    musicInfo_Widget->setMinimumSize(800,400);
    musicInfo_Widget->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Expanding);
    musicInfo_Widget->tabBar()->setObjectName("musicInfo_TabBar");
    musicInfo_Widget->addTab(song,"歌曲");
    // musicInfo_Widget->addTab(singer,"歌手");
    // musicInfo_Widget->addTab(album,"专辑");
    // musicInfo_Widget->addTab(folder,"文件夹");

    local_VBoxLayout->addSpacing(20);
    local_VBoxLayout->addLayout(display_HBoxLayout);
    local_VBoxLayout->addWidget(musicInfo_Widget);
}

QUrl localMusic::getUrlFromItem(QTableWidgetItem *item)
{
    QVariant itemData= item->data(Qt::UserRole);
    QUrl sourse = itemData.value<QUrl>();
    return sourse;

}

void localMusic::songInfoLayout(void)
{
    //-----------------按钮初始化---------------------//
    song = new QWidget();
    QVBoxLayout *song_VLayout = new QVBoxLayout();

    playAll = new QPushButton();
    playAll->setText("播放全部");
    playAll->setMaximumSize(120,35);
    playAll->setMinimumSize(120,35);
    playAll->setObjectName("playallBtn");
    playAll->setIcon(QIcon(":/images/stackWidget/localMusic/btn_playall.png"));

    /*QAction *addAllAction = new QAction(playAll);
    addAllAction->setIcon(QIcon(":/images/stackWidget/localMusic/btn_playalladd.png"));
    playAll->addAction(addAllAction);*/

    matchMusic = new QPushButton();
    matchMusic->setText("清空列表");
    matchMusic->setIcon(QIcon(QPixmap(":/images/stackWidget/localMusic/btn_deleteAll.png")));
    matchMusic->setObjectName("matchMusicbtn");
    matchMusic->setMaximumSize(120,35);
    matchMusic->setMinimumSize(120,35);
    QLineEdit *searchEdit = new QLineEdit();
    searchEdit->setPlaceholderText("搜索本地音乐");
    searchEdit->setObjectName("localSearch");
    searchEdit->setMaximumSize(200,25);

    QAction *searchAction = new QAction(searchEdit);
    searchAction->setIcon(QIcon(":/picture/Search.png"));
    //设置ICON在搜索框右边
    searchEdit->addAction(searchAction,QLineEdit::TrailingPosition);

    QSpacerItem *Button_HSpacer = new QSpacerItem(200,20,
                                                  QSizePolicy::Expanding,
                                                  QSizePolicy::Expanding);


    QHBoxLayout *button_HBoxLayout = new QHBoxLayout();
    button_HBoxLayout->addWidget(playAll);
    button_HBoxLayout->addWidget(matchMusic);
    button_HBoxLayout->addSpacerItem(Button_HSpacer);
    button_HBoxLayout->addWidget(searchEdit);

    music_Table = new QTableWidget();
    music_Table->setMinimumSize(750,500);
    music_Table->setSizePolicy(QSizePolicy::Expanding,
                               QSizePolicy::Expanding);
    music_Table->setObjectName("table_music");
    QList <QString> tableList;
    tableList << "" << "音乐标题" << "歌手" << "专辑" << "时长" << "大小";
    music_Table->setColumnCount(6);
    music_Table->setHorizontalHeaderLabels(tableList);
    music_Table->setColumnWidth(0,10);
    music_Table->setColumnWidth(1,200);
    music_Table->setColumnWidth(2,100);
    music_Table->setColumnWidth(3,250);
    music_Table->setColumnWidth(4,70);
    music_Table->setColumnWidth(5,70);
    //打开右键菜单属性
    music_Table->setContextMenuPolicy(Qt::CustomContextMenu);
    //开启交替行背景色
    music_Table->setAlternatingRowColors(true);
    //设置表格内容不可编辑
    music_Table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置为行选择
    music_Table->setSelectionBehavior(QAbstractItemView::SelectRows);
    //设置最后一栏自适应长度
    music_Table->horizontalHeader()->setStretchLastSection(true);
    //删除网格表
    music_Table->setShowGrid(false);
    //去除边框线
    music_Table->setFrameShape(QFrame::NoFrame);
    //去除选中虚线框
    music_Table->setFocusPolicy(Qt::NoFocus);
    //设置点击表头进行排序
    music_Table->setSortingEnabled(true);
    //设置表头字体向左靠齐
    music_Table->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    //设置点击单元格时，表头项不高亮
    music_Table->horizontalHeader()->setHighlightSections(false);
    music_Table->verticalHeader()->setHighlightSections(false);
    //设置只能选中一个目标
    music_Table->setSelectionMode(QAbstractItemView::SingleSelection);
    //设置垂直滚动条最小宽度
    //music_Table->verticalScrollBar()->setMaximumWidth(7);

    music_Table->verticalHeader()->setAlternatingRowColors(true);
    //music_Table->verticalHeader()->setObjectName("music_verticalHeader");
    music_Table->verticalHeader()->setDefaultSectionSize(30);
    music_Table->verticalHeader()->setVisible(false);
    music_Table->setStyleSheet("QTableView::item:selected { color:white; background:rgb((85,170,255));}"
                                 "QTableCornerButton::section { background-color:#ecf5ff; }"
                                 "QHeaderView::section { color:black; background-color:#ecf5ff; }"
                                 "QTableWidget { border: none; }"
                                 );

    song_VLayout->addLayout(button_HBoxLayout);
    song_VLayout->addWidget(music_Table);

    song->setLayout(song_VLayout);
}

void localMusic::table_customContextMenuRequest(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);

    QAction *ViewComments = menu->addAction("查看评论");
    QAction *play = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_play.png"),"播放");
    QAction *nextPlay = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_playnext.png"),"下一首播放");
    menu->addSeparator();
    QAction *Collecttolist = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_collectlist.png"),"收藏到歌单");
    QAction *sharemusic = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_share.png"),"分享");
    QAction *coppy = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_coppy.png"),"复制链接");
    QAction *uploadcloud = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_cloudmusic.png"),"上传到我的音乐云盘");
    QAction *opendir = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_folder.png"),"打开文件所在目录");
    menu->addSeparator();
    QAction *deletefromlist = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_delete.png"),"从列表中删除(Delete)");
    QAction *deletefromlocal = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_delete.png"),"从本地磁盘删除");

    QMenu *collectChild = new QMenu(this);
    Collecttolist->setMenu(collectChild);

    QAction *creatnewSheet = collectChild->addAction(QIcon(":/images/stackWidget/localMusic/btn_add.png"),"创建新歌单");
    QAction *myfavor = collectChild->addAction(QIcon(":/images/stackWidget/localMusic/btn_add.png"),"我喜欢的音乐");


    //menu.addAction("",this,SLOT());

    menu->exec(QCursor::pos());

}

void localMusic::ConnectInit(void)
{
    //tablelist界面右键弹出菜单栏
    connect(music_Table,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(table_customContextMenuRequest(QPoint)));
    //选择目录
    connect(selectDir,SIGNAL(clicked()),
            this,SLOT(btn_selectDirSlot()));

    //播放全部按钮
    connect(playAll,SIGNAL(clicked()),
            this,SLOT(btn_addlistclicked_slot()));
    //清空列表按钮
    connect(matchMusic,SIGNAL(clicked()),
            this,SLOT(btn_deleteclicked_slot()));

    /*connect(playAll,SIGNAL(clicked()),
            playlist,SLOT(Rev_ListMsg_slot(QList <MediaObjectInfo1> *info)));*/

}

void localMusic::btn_deleteclicked_slot(void)
{
    mediaObjectInfo.clear();
    music_Table->clearContents();
    music_Table->setRowCount(0);

    displayLabel[1]->setText("共"+QString::number(mediaObjectInfo.count(),10)+"首");
}

void localMusic::btn_addlistclicked_slot(void)
{
    emit addall_signal(&this->mediaObjectInfo);
}

void localMusic::btn_selectDirSlot(void)
{
    //获取绝对位置x,y
    int x = 400;

    int y = 400;
    GetplayPath();

    //selectDialog *selectDialog = new class selectDialog(this);
    selectDialog *selectDialog = new class selectDialog(this,Dir_list);
    selectDialog->move(x,y);
    //设置关闭窗口时自动销毁
    //selectDialog->setAttribute(Qt::WA_DeleteOnClose);

    //以模态方式显示对话框
    int ret = selectDialog->exec();

    int count = 0;

    if(ret == QDialog::Accepted)
    {
        Dir_list = selectDialog->Dir_List;

        SaveplayPath();
        foreach (QString str, selectDialog->Dir_List)
        {
            //如果该目录被选择
            if(selectDialog->listPath->item(count)->checkState() == Qt::Checked)
            {
                QDir dir(str);
                if (dir.exists())
                {
                    //定义过滤器
                    QStringList filter;
                    //包含所有.mp3后缀的文件
                    filter << "*.mp3";
                    //获取该目录下的所有文件
                    QFileInfoList files = dir.entryInfoList(filter,QDir::Files);

                    for(int i=0; i< files.count(); i++)
                    {
                        QString songname = QString::fromUtf8(files.at(i).fileName().replace(".mp3","").toUtf8().data());

                        bool result = false;
                        foreach(MediaObjectInfo search,mediaObjectInfo)
                        {
                            if(search.filePath == files.at(i).filePath())
                            {
                                result = true;
                            }
                        }

                        if (result == false)
                        {
                            MediaObjectInfo info;

                            //qDebug() << files.at(i).fileName();

                            music_Table->insertRow(music_Table->rowCount());
                            int index = music_Table->rowCount()-1;
                            if(index < 0)
                                index =0;

                            QString number;
                            if (index+1<10)
                                number = "0" + QString::number(index+1,10);
                            else
                                number = QString::number(index+1,10);

                            QTableWidgetItem *item0 = new QTableWidgetItem(number);
                            music_Table->setItem(index,0,item0);

                            QStringList songinfo = songname.split("-");

                            info.fileName = songinfo.at(0);

                            if (songinfo.count()>1)
                                info.singer = songinfo.at(1);
                            else
                                info.singer = "未知歌手";

                            info.playtime = "00:00";

                            info.filePath = files.at(i).filePath();

                            //qDebug() << "path:" << files.at(i).filePath();

                            QTableWidgetItem *item1 = new QTableWidgetItem(info.fileName);
                            music_Table->setItem(index,1,item1);
                            QTableWidgetItem *item2 = new QTableWidgetItem(info.singer);
                            music_Table->setItem(index,2,item2);
                            QTableWidgetItem* item3 = new QTableWidgetItem("未知专辑");
                            music_Table->setItem(index,3,item3);
                            QTableWidgetItem* item4 = new QTableWidgetItem(info.playtime);
                            music_Table->setItem(index,4,item4);

                            float size = (float)files.at(i).size()/1000.0f/1000.0f;
                            QString string_size = QString::number(size,'f',1) + "M";

                            QTableWidgetItem* item5 = new QTableWidgetItem(string_size);
                            music_Table->setItem(index,5,item5);

                            info.filesize = string_size;
                            mediaObjectInfo.append(info);
                        }
                    }
                }
            }
            count++;
        }
        music_Table->setRowCount(mediaObjectInfo.count());
        displayLabel[1]->setText("共"+QString::number(mediaObjectInfo.count(),10)+"首");
        //QUrl localmusicSource = getUrlFromItem(music_Table->currentItem());

    }

    delete selectDialog;
}






//---------------------------Dialog--------------------------------------//
selectDialog::selectDialog(QWidget *parent) :
    QDialog(parent)
{
    // QScreen *screen = QGuiApplication::primaryScreen();
    // QRect availableGeometry = screen->availableGeometry();

    int x = 400;
    int y = 400;

    this->setGeometry(x,y,450,300);
    //this->setWindowFlag(Qt::FramelessWindowHint);

    dialog_vLayout = new QVBoxLayout();

    select_LayoutInit();

    LoadStyleSheet();

    ConnectInit();
}

selectDialog::selectDialog(QWidget *parent ,QList <QString> list):
    QDialog(parent)
{
    Dir_List = list;

    // QScreen *screen = QGuiApplication::primaryScreen();
    // QRect availableGeometry = screen->availableGeometry();

    int x = 400;
    int y = 400;


    this->setGeometry(x,y,450,300);
    //this->setWindowFlag(Qt::FramelessWindowHint);

    dialog_vLayout = new QVBoxLayout();

    select_LayoutInit();

    LoadStyleSheet();

    ConnectInit();
}

void selectDialog::LoadStyleSheet(void)
{
    QFile file("./localMusicDialog.qss");

    file.open(QIODevice::ReadOnly);

    if (file.isOpen())
    {
        QString style = this->styleSheet();
        style += QLatin1String(file.readAll());
        this->setStyleSheet(style);
        file.close();
    }
}

void selectDialog::select_LayoutInit(void)
{
    closeButton = new QPushButton();
    closeButton->setObjectName("selectDialog_closeButton");
    closeButton->setMaximumSize(20,20);
    closeButton->setCursor(Qt::PointingHandCursor);
    QHBoxLayout *Head_hLayout = new QHBoxLayout();
    Head_hLayout->addStretch();
    Head_hLayout->addWidget(closeButton);
    Head_hLayout->addSpacing(5);


    headLabel = new QLabel();
    headLabel->setMinimumSize(450,30);
    headLabel->setText("选择本地音乐文件夹");
    headLabel->setAlignment(Qt::AlignCenter);

    QFont font = headLabel->font();
    font.setBold(true);
    font.setPointSize(13);
    headLabel->setFont(font);

    describLabel = new QLabel();
    describLabel->setText("将自动扫描你勾选的目录，文件的增删实时同步。");

    QPalette pe;
    pe.setColor(QPalette::WindowText,QColor(115,115,115));
    describLabel->setPalette(pe);

    /*Dir_List.append("我的音乐");
    QDir dir(QCoreApplication::applicationDirPath() + "/mp3");
    QDir dirbsolutePath(dir.absolutePath());
    Dir_List.append(dirbsolutePath.path());*/

    listPath = new QListWidget();
    listPath->setViewMode(QListView::ListMode);
    listPath->setObjectName("selectDialog_listPath");
    listPath->setMinimumSize(350,100);
    listPath->setFocusPolicy(Qt::NoFocus);
    listPath->verticalScrollBar()->setFixedWidth(7);

    foreach(QString path, Dir_List)
    {
        QListWidgetItem *pathItem = new QListWidgetItem();
        //设置Item高度
        pathItem->setSizeHint(QSize(0,30));
        pathItem->setText(path);
        pathItem->setCheckState(Qt::Checked);
        listPath->addItem(pathItem);    //增加一个项目
    }

    confirm_Button = new QPushButton();
    confirm_Button->setText("确认");
    confirm_Button->setMaximumSize(90,36);
    confirm_Button->setMinimumSize(90,36);
    confirm_Button->setObjectName("selectDialog_confirbtn");
    addfolder_Button = new QPushButton();
    addfolder_Button->setText("添加文件夹");
    addfolder_Button->setMaximumSize(115,36);
    addfolder_Button->setMinimumSize(115,36);
    addfolder_Button->setObjectName("selectDialog_addbtn");

    QHBoxLayout *button_hLayout = new QHBoxLayout();

    button_hLayout->addStretch();
    button_hLayout->addWidget(confirm_Button);
    button_hLayout->addSpacing(5);
    button_hLayout->addWidget(addfolder_Button);
    button_hLayout->addStretch();

    dialog_vLayout->addLayout(Head_hLayout);
    dialog_vLayout->addWidget(headLabel);
    dialog_vLayout->addSpacing(5);
    dialog_vLayout->addWidget(describLabel);
    dialog_vLayout->addSpacing(5);
    dialog_vLayout->addWidget(listPath);
    dialog_vLayout->addSpacing(5);
    dialog_vLayout->addLayout(button_hLayout);
    dialog_vLayout->addSpacing(20);

    this->setLayout(dialog_vLayout);

    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    this->setAttribute(Qt::WA_StyledBackground);
    //设置无边框dialog可拖拽
    this->setSizeGripEnabled(true);
    //设置窗口不能改变大小
    this->setFixedSize(this->geometry().size());


    //给Dialog画个圆角边框
    QBitmap bmp(QSize(this->size()));
    bmp.fill();
    QPainter p(&bmp);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.drawRoundedRect(bmp.rect(),5,5);
    setMask(bmp);
}

void selectDialog::ConnectInit(void)
{
    //关闭按钮绑定
    connect(closeButton,SIGNAL(clicked()),this,SLOT(btn_close_slot()));

    //确定按钮绑定
    connect(confirm_Button,SIGNAL(clicked()),this,SLOT(confirmclicked_slot()));
    //添加文件夹按钮绑定
    connect(addfolder_Button,SIGNAL(clicked()),this,SLOT(addfolder_slot()));
    //路径List点击绑定
    connect(listPath,SIGNAL(itemClicked(QListWidgetItem *)),this,SLOT(PathlistClicked_slot(QListWidgetItem *)));

}

void selectDialog::PathlistClicked_slot(QListWidgetItem *item)
{
    if(item->checkState()==Qt::Checked)
    {
        item->setCheckState(Qt::Unchecked);
    }
    else
    {
        item->setCheckState(Qt::Checked);
    }

}

void selectDialog::btn_close_slot(void)
{
    reject();
}

void selectDialog::confirmclicked_slot(void)
{
    accept();
}
void selectDialog::addfolder_slot(void)
{
    QString DirPath = QFileDialog::getExistingDirectory(this);
    Dir_List.append(DirPath);

    QListWidgetItem *addItem = new QListWidgetItem();
    addItem->setText(DirPath);
    addItem->setCheckState(Qt::Checked);
    listPath->addItem(addItem);
    //qDebug() << "path:" << DirPath;
}

