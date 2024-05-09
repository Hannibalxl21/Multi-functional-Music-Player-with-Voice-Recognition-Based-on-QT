#include "widget.h"
#include "ui_widget.h"
#include "ui_playlistform.h"
#include <QDebug>
#include <QFileDialog>
#include <QDir>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QKeyEvent>
#include <QLineEdit>
#include <QColor>
#include <QGridLayout>
#include <QSqlError>
#include "dbManager.h"



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);


    audio = new Audio(this);
    //如何播放音乐 先加载模块multimedia
    //先new一个output对象
    // 在 Widget 类的构造函数中连接 lineEdit_2 的 textChanged 信号
    ui->widget->setFocusPolicy(Qt::NoFocus); // 禁用其他控件的焦点
    // 将widget设置为Widget类的布局
    move(QGuiApplication::primaryScreen()->geometry().center() - rect().center());

    audioOutput = new QAudioOutput(this);
    //再来一个媒体播放对象
    mediaPlayer = new QMediaPlayer(this);
    mediaPlayer->setAudioOutput(audioOutput);
    //是视频播放对象
    videoWidget = new QVideoWidget(this);
    //videoWidget->setStyleSheet("background-color:white;");
    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout;
    // 将视频小部件添加到布局中
    layout->addWidget(videoWidget);
    // 将布局设置给当前QWidget
    //播放MV
    MVaudioOutput = new QAudioOutput(this);
    VideoPlayer = new QMediaPlayer(this);
    VideoPlayer->setAudioOutput(MVaudioOutput);
    VideoPlayer->setVideoOutput(videoWidget);
    ui->MVwidget->setLayout(layout);
    this->setWindowTitle("MING");
    manager = new QNetworkAccessManager(this);

    //给播放器设置音乐
    //获取当前媒体的时长 通过信号关联来获取
    connect(mediaPlayer,&QMediaPlayer::durationChanged,this,[=](qint64 duration)
{
        ui->totallabel->setText(QString("%1:%2").arg(duration/1000/60,2,10,QChar('0')).arg(duration/1000%60,2,10,QChar('0')));
        ui->PlayCourseSlider->setRange(0,duration);
});
    //获取当前播放时长
    connect(mediaPlayer,&QMediaPlayer::positionChanged,this,[=](qint64 pos)
    {
        ui->Curlabel->setText(QString("%1:%2").arg(pos/1000/60,2,10,QChar('0')).arg(pos/1000%60,2,10,QChar('0')));
        ui->PlayCourseSlider->setValue(pos);

    });
    //让进度滑块动起来
    connect(ui->PlayCourseSlider,&QSlider::sliderMoved,mediaPlayer,&QMediaPlayer::setPosition);
    // 默认设置为列表循环
    disconnect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Widget::onMediaStatusChangedSingleLoop);
    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Widget::onMediaStatusChangedListLoop);
    QIcon icon(":/picture/ListLoop1.png");
    // 设置默认图标为列表循环
    ui->pushButtonLoop->setIcon(icon);
    ui->pushButtonLoop->setChecked(false); // 确保按钮状态与列表循环对应
    // 假设 muteButton 是你的静音按钮
    connect(ui->pushButtonSound, &QPushButton::clicked, this, [this]()
    {
        // 设置音量滑块值为0
        float previousVolume = mediaPlayer->audioOutput()->volume(); // 存储当前音量
        bool mute = mediaPlayer->audioOutput()->isMuted(); // 检查音频是否已静音
        if (mute)
        {
            mediaPlayer->audioOutput()->setVolume(previousVolume); // 恢复先前的音量
            ui->verticalSliderVolumn->setValue(previousVolume);
        }
        else if(!mute)
        {mediaPlayer->audioOutput()->setVolume(0);
            ui->verticalSliderVolumn->setValue(0);
        }
        mute = mediaPlayer->audioOutput()->isMuted();

        // 将音频输出的音量设置为0

    });

    //MV
    connect(VideoPlayer,&QMediaPlayer::durationChanged,this,[=](qint64 duration)
            {
                ui->totallabelMV->setText(QString("%1:%2").arg(duration/1000/60,2,10,QChar('0')).arg(duration/1000%60,2,10,QChar('0')));
                ui->horizontalSliderMV->setRange(0,duration);
            });
    //获取当前播放时长
    connect(VideoPlayer,&QMediaPlayer::positionChanged,this,[=](qint64 pos)
            {
                ui->CurlabelMV->setText(QString("%1:%2").arg(pos/1000/60,2,10,QChar('0')).arg(pos/1000%60,2,10,QChar('0')));
                ui->horizontalSliderMV->setValue(pos);
            });

    //让进度滑块动起来
    connect(ui->horizontalSliderMV,&QSlider::sliderMoved,VideoPlayer,&QMediaPlayer::setPosition);
    //跳转页面
    connect(ui->pushButtonFindmusic, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->pushButtonVideo, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(1);
    });
    connect(ui->pushButtonOnlineList, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(2);
    });
    connect(ui->pushButtonMylove, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(3);
    });
    connect(ui->pushButtonLocalDownload, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(4);
    });
    connect(ui->pushButtondetails, &QPushButton::clicked, this, [=]()
    {
        ui->stackedWidget->setCurrentIndex(5);
    });


    settableWidgetPlayListlayout();
    //QList<QUrl> playingList = playlistForm->playingList;
createWidgetToplist();
    setSinger();


    lyricWidget = new LyricWidget;
connect(lyricWidget->pushButton_set,SIGNAL(pressed()),this,SLOT(dialogSet()));
connect(lyricWidget->pushButton_close,SIGNAL(pressed()),this,SLOT(hideLyric()));

connect(ui->tableWidgetPlayList, &QTableWidget::cellDoubleClicked, this, &Widget::playSong);
// connect(ui->tableWidgetPlayList, SIGNAL(cellDoubleClicked(int,int)), this, SLOT(createPlayingList(int, int)));
ui->pushButtonLyric->setCheckable(true);
ui->pushButtonLyric->setChecked(true);
connect(ui->pushButtonLyric, &QPushButton::clicked, this, [this]()
    {
        showHideLyric(ui->pushButtonLyric->isChecked());
    });

connect(mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChange(qint64)));
connect(ui->pushButtondownload, SIGNAL(pressed()), this, SLOT(dialogDownload()));
QSettings settings("OrganizationName", "ApplicationName");
// settings.setValue("LyricX", 540);
// settings.setValue("LyricY", 780);
qDebug() << settings.allKeys();
QString slx =  settings.value("LyricX").toString();
QString sly =  settings.value("LyricY").toString();
//settings.setValue("LyricShow", true);
QString lyricShow = settings.value("LyricShow").toString();


qDebug() << "歌词坐标" << slx << sly;
downloadPath =  settings.value("DownloadPath").toString();
if (downloadPath == "") {
    settings.setValue("DownloadPath", QStandardPaths::standardLocations(QStandardPaths::MusicLocation).first());
}
qDebug() << settings.allKeys();
qDebug() << "lyricShow3" << lyricShow;
QString SColorLeft = settings.value("LyricFontColorLeft", "#FF0000").toString();
lyricWidget->color_left = QColor(SColorLeft);
QString SColorRight = settings.value("LyricFontColorRight", "#00FF00").toString();
lyricWidget->color_right = QColor(SColorRight);
QString sfont = settings.value("Font").toString();
if (sfont.isEmpty()) {
    QFont font = qApp->font();
    font.setPointSize(40);
    lyricWidget->font = font;
    sfont = font.family() + "," + QString::number(font.pointSize()) + "," + QString::number(font.weight()) + "," + QString::number(font.italic());
    settings.setValue("Font", sfont);
} else {
    QStringList SLFont = sfont.split(",");
    QFont font(SLFont.at(0), SLFont.at(1).toInt(), SLFont.at(2).toInt(), SLFont.at(3).toInt());
    lyricWidget->font = font;
    ui->textBrowser->setFont(font);

}
//我喜欢

setMylovelist();
//右键弹出菜单栏
// 设置tableWidgetPlayList的上下文菜单策略
ui->tableWidgetPlayList->setContextMenuPolicy(Qt::CustomContextMenu);
// 建立连接，右键点击tableWidgetPlayList时触发tablecustomContextMenuRequest槽函数
connect(ui->tableWidgetPlayList, &QTableWidget::customContextMenuRequested, this, &Widget::tablecustomContextMenuRequest);
musicListWidget = ui->listWidget; // 初始化 listWidget 是用于显示歌单的 QListWidget
connect(ui->listWidget, &QListWidget::customContextMenuRequested, this, &Widget::listWidgettablecustomContextMenuRequest);
ui->tableWidgetMycreate->setContextMenuPolicy(Qt::CustomContextMenu);
connect(ui->tableWidgetMycreate,&QTableWidget::customContextMenuRequested, this, &Widget::MycreatecustomContextMenuRequest);
ui->tableWidgetMylove->setContextMenuPolicy(Qt::CustomContextMenu);
connect(ui->tableWidgetMylove,&QTableWidget::customContextMenuRequested, this, &Widget::MylovecustomContextMenuRequest);




//本地音乐
localmusic = new localMusic;
ui->stackedWidget->insertWidget(4, localmusic);
onMusicListSelected();
loadPlaylistNames();
myspeech = new QTextToSpeech;
speechconnect();
clearTimer = new QTimer(this);
clearTimer->setSingleShot(true); // 设置为单次触发

// 连接信号和槽函数
connect(clearTimer, &QTimer::timeout, this, [this]() {
    ui->lineEditSpeech->clear(); // 清除文本内容
});
}



Widget::~Widget()
{
    delete ui;
}


void Widget::createWidgetToplist()
{

    ui->scrollAreaToplist->setWidget(ui->toplistWidget);
    ui->scrollAreaToplist->setWidgetResizable(true);   //关键语句
    QGridLayout *gridLayout = new QGridLayout(ui->toplistWidget);
    ui->toplistWidget->setLayout(gridLayout);


    gridLayout->setSpacing(40);
    QString surl = "http://music.163.com/api/toplist";
    //qWarning() << surl;
    QByteArray replyData = getReply(surl); // 只调用一次
    if (replyData.isEmpty()) {
        qDebug() << "Failed to get reply from" << surl;
        return; // 处理错误情况
    }
    QString replyString(replyData);
    //qWarning()  << "Reply from getReply(surl):" << replyString;
    QJsonDocument JD = QJsonDocument::fromJson(replyData);
    QJsonArray JA_list = JD.object().value("list").toArray();
    //qDebug() << list;
    for (int i=0; i< JA_list.size(); i++) {
        QString coverImgUrl = JA_list[i].toObject().value("coverImgUrl").toString();
        double id = JA_list[i].toObject().value("id").toDouble();
        QString name = JA_list[i].toObject().value("name").toString();
        QPushButton *pushButton = new QPushButton(this);
        pushButton->setFixedSize(120,120);
        pushButton->setIconSize(QSize(120,120));
        pushButton->setFlat(true);
        pushButton->setCursor(Qt::PointingHandCursor);
        gridLayout->addWidget(pushButton, i/5, i%5);
        getToplistImg(coverImgUrl, pushButton);
        connect(pushButton, &QPushButton::clicked,this, [=]{
            createPlaylist(id);
        });
    }
}



void Widget::getToplistImg(QString coverImgUrl, QPushButton *pushButton)
{
    QNetworkAccessManager *NAM = new QNetworkAccessManager;
    QNetworkRequest request;
    request.setUrl(QUrl(coverImgUrl));
    QNetworkReply *reply = NAM->get(request);

    connect(reply, &QNetworkReply::finished, this, [=](){
        if (pushButton)
        {
            QPixmap pixmap;
            pixmap.loadFromData(reply->readAll());
            pixmap = pixmap.scaled(pushButton->iconSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            pushButton->setIcon(QIcon(pixmap));
        }

        reply->deleteLater(); // 在处理完reply后记得释放资源
    });
}

void Widget::getLyric(QString id)
{
    QString surl = "http://music.163.com/api/song/lyric?os=pc&lv=-1&kv=-1&tv=-1&id=" + id;
    qDebug() << surl;
    QJsonDocument json = QJsonDocument::fromJson(getReply(surl));
    QString slyric = json.object().value("lrc").toObject().value("lyric").toString();
    ui->textBrowser->setText("");

    lyrics.clear();
    QStringList line = slyric.split("\n");
    for(int i=0; i<line.size(); i++){
        if(line.at(i).contains("]")){
            QStringList SL = line.at(i).split("]");
            //qDebug() << SL.at(0).mid(1);
            for(int j=0; j<SL.length()-1; j++){
                Lyric lyric;
                QString stime = SL.at(j).mid(1);
                //qDebug() << stime.length() << stime.indexOf(".");
                if((stime.length() - stime.indexOf(".")) == 3) stime += "0";
                lyric.time = QTime::fromString(stime, "mm:ss.zzz");
                lyric.sentence = SL.at(SL.length()-1);
                lyrics.append(lyric);
            }
        }
    }
    std::sort(lyrics.begin(), lyrics.end(), [](Lyric a, Lyric b){return a.time < b.time;});
    for(int i=0; i<lyrics.size(); i++){
        ui->textBrowser->insertPlainText(lyrics.at(i).sentence + "\n");
    }
    ui->textBrowser->selectAll();
    ui->textBrowser->setAlignment(Qt::AlignCenter);
    QTextCursor cursor = ui->textBrowser->textCursor();
    cursor.setPosition(0, QTextCursor::MoveAnchor);
    ui->textBrowser->setTextCursor(cursor);
}
void Widget::settableWidgetPlayListlayout()
{
    ui->tableWidgetPlayList->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetPlayList->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetPlayList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetPlayList->setAlternatingRowColors(true);
    ui->tableWidgetPlayList->setShowGrid(false);
    ui->tableWidgetPlayList->setFrameShape(QFrame::NoFrame);
    ui->tableWidgetPlayList->setFocusPolicy(Qt::NoFocus);
    ui->tableWidgetPlayList->setSortingEnabled(true);
    ui->tableWidgetPlayList->setColumnCount(8);
    ui->tableWidgetPlayList->setColumnHidden(4,true);
    ui->tableWidgetPlayList->setColumnHidden(5,true);
    ui->tableWidgetPlayList->setColumnHidden(6,true);
    ui->tableWidgetPlayList->horizontalHeader()->resizeSection(0,380);
    ui->tableWidgetPlayList->horizontalHeader()->resizeSection(1,150);
    ui->tableWidgetPlayList->horizontalHeader()->resizeSection(2,80);
    ui->tableWidgetPlayList->horizontalHeader()->resizeSection(3,80);
    ui->tableWidgetPlayList->horizontalHeader()->resizeSection(7,40);

    QStringList header;
    header << "歌名" << "歌手" << "专辑" << "时长" << "id" << "专辑封面" << "mvid" << "MV";
    ui->tableWidgetPlayList->setHorizontalHeaderLabels(header);
    ui->tableWidgetPlayList->setStyleSheet("QTableView::item:selected { color:white; background:rgb((85,170,255));}"
                                           "QTableCornerButton::section { background-color:#ecf5ff; }"
                                           "QHeaderView::section { color:black; background-color:#ecf5ff; }"
                                           "QTableWidget { border: none; }"
                                           );

}


void Widget::createPlaylist(double id)
{
    QString surl = QString("http://music.163.com/api/playlist/detail?id=%1").arg(id, 0, 'f', 0);
    qDebug() << surl;
    QByteArray responseData = getReply(surl);
    QJsonParseError jsonError;
    QJsonDocument JD = QJsonDocument::fromJson(responseData, &jsonError);

    if (jsonError.error != QJsonParseError::NoError) {
        qDebug() << "JSON parse error: " << jsonError.errorString();
        // 处理 JSON 解析错误
    } else {
        // 成功解析 JSON 数据，可以继续处理
    }

    int code = JD.object().value("code").toInt();
    qDebug() << code;
    if (code == 200)
    {
        int columnCount = ui->tableWidgetPlayList->columnCount();
        ui->tableWidgetPlayList->setRowCount(0);
        ui->tableWidgetPlayList->setColumnCount(columnCount); // 清空在线歌单列表
        ui->tableWidgetPlayList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableWidgetPlayList->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableWidgetPlayList->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidgetPlayList->setAlternatingRowColors(true);
        ui->tableWidgetPlayList->setShowGrid(false);
        ui->tableWidgetPlayList->setFrameShape(QFrame::NoFrame);
        ui->tableWidgetPlayList->setFocusPolicy(Qt::NoFocus);
        ui->tableWidgetPlayList->setSortingEnabled(true);
        ui->tableWidgetPlayList->setColumnCount(8);
        ui->tableWidgetPlayList->setColumnHidden(4,true);
        ui->tableWidgetPlayList->setColumnHidden(5,true);
        ui->tableWidgetPlayList->setColumnHidden(6,true);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(0,380);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(1,150);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(2,80);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(3,80);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(7,40);

        QStringList header;
        header << "歌名" << "歌手" << "专辑" << "时长" << "id" << "专辑封面" << "mvid" << "MV";
        ui->tableWidgetPlayList->setHorizontalHeaderLabels(header);
        ui->tableWidgetPlayList->setStyleSheet("QTableView::item:selected { color:white; background:rgb((85,170,255));}"
                                               "QTableCornerButton::section { background-color:#ecf5ff; }"
                                               "QHeaderView::section { color:black; background-color:#ecf5ff; }"
                                               "QTableWidget { border: none; }"
                                               );



        QJsonArray tracks = JD.object().value("result").toObject().value("tracks").toArray();
        for (int i=0; i<tracks.size(); i++)
        {
            ui->tableWidgetPlayList->insertRow(i);
            ui->tableWidgetPlayList->setItem(i, 0, new QTableWidgetItem(tracks[i].toObject().value("name").toString()));
            QJsonArray artists = tracks[i].toObject().value("artists").toArray();
            QString sartists = "";
            for(int a=0; a<artists.size(); a++)
            {
                sartists += artists[a].toObject().value("name").toString();
                if(a<artists.size()-1) sartists += ",";
            }
            ui->tableWidgetPlayList->setItem(i, 1, new QTableWidgetItem(sartists));
            ui->tableWidgetPlayList->setItem(i, 2, new QTableWidgetItem(tracks[i].toObject().value("album").toObject().value("name").toString()));
            int ds = tracks[i].toObject().value("duration").toInt()/1000;
            QTableWidgetItem *TWI = new QTableWidgetItem(QString("%1:%2").arg(ds/60,2,10,QLatin1Char(' ')).arg(ds%60,2,10,QLatin1Char('0')));
            TWI->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
            ui->tableWidgetPlayList->setItem(i, 3, TWI);
            ui->tableWidgetPlayList->setItem(i, 4, new QTableWidgetItem(QString::number(tracks[i].toObject().value("id").toInt())));
            ui->tableWidgetPlayList->setItem(i, 5, new QTableWidgetItem(tracks[i].toObject().value("album").toObject().value("picUrl").toString()));
            int mvid = tracks[i].toObject().value("mvid").toInt();
            ui->tableWidgetPlayList->setItem(i, 6, new QTableWidgetItem(QString::number(mvid)));
            if(mvid != 0)
            {
                QPushButton *pushButton_MV = new QPushButton;
                pushButton_MV->setFixedSize(24,24);
                pushButton_MV->setIcon(QIcon(":/picture/MV.png"));
                pushButton_MV->setIconSize(QSize(24, 24));
                pushButton_MV->setFocusPolicy(Qt::NoFocus);
                pushButton_MV->setFlat(true);
                pushButton_MV->setCursor(Qt::PointingHandCursor);
                connect(pushButton_MV, SIGNAL(clicked()), this, SLOT(on_pushButtonMV_clicked()));
                ui->tableWidgetPlayList->setCellWidget(i, 7, pushButton_MV);
            }
        }
        //ui->tableWidgetPlayList->resizeColumnsToContents();

        ui->stackedWidget->setCurrentIndex(2);
        //setCurrentPlaylist(name);
    }
    else
    {
        QString message = JD.object().value("message").toString();
        displayErrorMessage(message);
    }
}
void Widget::on_pushButtonMVplay_clicked()
{
    switch(VideoPlayer->playbackState())
    {

    case QMediaPlayer::PlaybackState::PlayingState://播放状态
        VideoPlayer->pause();
        break;
    case QMediaPlayer::PlaybackState::PausedState://暂停状态
        VideoPlayer->play();
        break;
    case QMediaPlayer::StoppedState:
        break;
    }
}
void Widget::hideLyric()
{
    lyricWidget->hide();
    ui->pushButtonLyric->setChecked(false);
    settings.setValue("LyricShow", "false");
}

void Widget::dialogSet()
{
    QDialog *dialog_set = new QDialog(this);
    dialog_set->setWindowTitle("设置");
    dialog_set->setFixedSize(300,200);
    dialog_set->setStyleSheet("QLineEdit { border:1px solid gray; } ");
    QGridLayout *gridLayout = new QGridLayout;

    QLabel *label = new QLabel("歌词字体");
    label->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(label,0,0,1,1);
    pushButton_font = new QPushButton;
    QString sfont = lyricWidget->font.family() + "," + QString::number(lyricWidget->font.pointSize()) + "," + QString::number(lyricWidget->font.weight()) + "," + (lyricWidget->font.italic() ? "true" : "false");
    pushButton_font->setText(sfont);
    pushButton_font->setFocusPolicy(Qt::NoFocus);
    connect(pushButton_font,SIGNAL(pressed()),this,SLOT(chooseFont()));
    gridLayout->addWidget(pushButton_font,0,1,1,2);

    //歌词颜色
    label = new QLabel("歌词颜色");
    label->setAlignment(Qt::AlignCenter);
    gridLayout->addWidget(label,1,0,1,1);

    pushButton_fontcolorleft = new QPushButton;
    pushButton_fontcolorleft->setObjectName("LyricFontColorLeft");
    pushButton_fontcolorleft->setText("■已播放");
    pushButton_fontcolorleft->setFocusPolicy(Qt::NoFocus);
    pushButton_fontcolorleft->setStyleSheet("color:" + lyricWidget->color_left.name());
    connect(pushButton_fontcolorleft,SIGNAL(pressed()),this,SLOT(chooseFontColor()));
    gridLayout->addWidget(pushButton_fontcolorleft,1,1,1,1);

    pushButton_fontcolorright = new QPushButton;
    pushButton_fontcolorright->setObjectName("LyricFontColorRight");
    pushButton_fontcolorright->setText("■未播放");
    pushButton_fontcolorright->setFocusPolicy(Qt::NoFocus);
    pushButton_fontcolorright->setStyleSheet("color:" + lyricWidget->color_right.name());
    connect(pushButton_fontcolorright,SIGNAL(pressed()),this,SLOT(chooseFontColor()));
    gridLayout->addWidget(pushButton_fontcolorright,1,2,1,1);

    // 保存路径
    QPushButton *pushButton_downloadPath = new QPushButton("保存路径");
    pushButton_downloadPath->setFocusPolicy(Qt::NoFocus);
    pushButton_downloadPath->setFlat(true);
    connect(pushButton_downloadPath,SIGNAL(pressed()),this,SLOT(openDownloadPath()));
    gridLayout->addWidget(pushButton_downloadPath,2,0,1,1);
    lineEdit_downloadPath = new QLineEdit;
    downloadPath =  settings.value("DownloadPath").toString();
    lineEdit_downloadPath->setText(downloadPath);
    QAction *action_browse = new QAction(this);
    action_browse->setObjectName("SettingDialogChooseDownloadPath");
    action_browse->setIcon(style()->standardIcon(QStyle::SP_DirIcon));
    connect(action_browse,SIGNAL(triggered(bool)),this,SLOT(chooseDownloadPath()));
    lineEdit_downloadPath->addAction(action_browse,QLineEdit::TrailingPosition);
    gridLayout->addWidget(lineEdit_downloadPath,2,1,1,2);

    dialog_set->setLayout(gridLayout);
    dialog_set->show();
}

void Widget::download(QString surl, QString filepath)
{
    ui->pushButtondownload->setEnabled(false);
    qDebug() <<  "download -> " << surl << "->" << filepath;
    QUrl url = QString(surl);
    QNetworkAccessManager *NAM = new QNetworkAccessManager;
    QNetworkRequest request(url);
    QNetworkReply *reply = NAM->get(request);
    QEventLoop loop;
    if(!surl.contains("http://music.163.com/song/media/outer/url?id="))
        connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    //跳转URL处理  https://blog.csdn.net/mingzznet/article/details/9724371
    if(surl.contains("http://music.163.com/song/media/outer/url?id=")){
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << "HttpStatusCode" << statusCode;
        surl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        qDebug() << "redirect" << surl;
        url.setUrl(surl);
        request.setUrl(url);
        reply = NAM->get(request);
        connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateProgress(qint64,qint64)));
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        loop.exec();
    }
    QFile file(filepath);
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    //qDebug() << reply->readAll();
    file.close();
    ui->pushButtondownload->setEnabled(true);
}

void Widget::dialogDownload()
{
    QDialog *dialog = new QDialog(this);
    dialog->setFixedWidth(300);
    dialog->setWindowTitle("下载");
    dialog->setStyleSheet("QLineEdit { border:1px solid gray; }"
                          "QToolTip { border:1px solid black; background-color: black; }");
    QGridLayout *gridLayout = new QGridLayout;
    QLabel *label = new QLabel("歌名");
    gridLayout->addWidget(label,0,0,1,1);
    QLineEdit *lineEdit_songname = new QLineEdit;
    gridLayout->addWidget(lineEdit_songname,0,1,1,1);
    label = new QLabel("下载地址");
    gridLayout->addWidget(label,1,0,1,1);
    QLineEdit *lineEdit_url = new QLineEdit;

    lineEdit_url->setText(mediaPlayer->source().toString());

    gridLayout->addWidget(lineEdit_url,1,1,1,1);
    QString suffix = QFileInfo(lineEdit_url->text()).suffix();
    suffix = suffix.left(suffix.indexOf("?"));
    lineEdit_songname->setText(ui->labelSongname->text().replace("\n"," - ") + "." + suffix);
    QPushButton *pushButton_open = new QPushButton("保存路径");
    pushButton_open->setFlat(true);
    gridLayout->addWidget(pushButton_open,2,0,1,1);
    pushButton_path = new QPushButton;
    pushButton_path->setObjectName("DownloadDialogPath");
    pushButton_path->setFocusPolicy(Qt::NoFocus);
    downloadPath =  settings.value("DownloadPath").toString();
    pushButton_path->setText(downloadPath);
    pushButton_path->setToolTip(downloadPath);
    connect(pushButton_path,SIGNAL(pressed()),this,SLOT(chooseDownloadPath()));
    gridLayout->addWidget(pushButton_path,2,1,1,1);
    connect(pushButton_open, &QPushButton::pressed, [=](){
        QDesktopServices::openUrl(QUrl(downloadPath));
    });
    dialog->setLayout(gridLayout);
    QHBoxLayout *hbox = new QHBoxLayout;
    hbox->addStretch();
    QPushButton *pushButton_confirm = new QPushButton("确定");
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    hbox->addWidget(pushButton_confirm);
    QPushButton *pushButton_cancel = new QPushButton("取消");
    connect(pushButton_cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
    hbox->addWidget(pushButton_cancel);
    hbox->addStretch();
    gridLayout->addLayout(hbox,3,0,1,2);
    int result = dialog->exec();
    if (result == QDialog::Accepted) {
        download(lineEdit_url->text(), pushButton_path->text() + "/" + lineEdit_songname->text());
    } else if (result == QDialog::Rejected) {
        dialog->close();
    }

}

// void Widget::createPlayingList(int row, int column)
// {

//     QList<QUrl> playingList = playlistForm->playingList;
//     for (int i = 0; i < ui->tableWidgetPlayList->rowCount(); ++i)
//     {
//         QTableWidgetItem *songItem = ui->tableWidgetPlayList->item(i, 0); // 获取歌曲名称
//         QTableWidgetItem *artistItem = ui->tableWidgetPlayList->item(i, 1); // 获取艺术家名称
//         QTableWidgetItem *ds = ui->tableWidgetPlayList->item(i, 3);//获取时长
//         int newRow = playlistForm->getUI()->PlayList->rowCount();
//         playlistForm->getUI()->PlayList->insertRow(newRow);

//         playlistForm->getUI()->PlayList->setItem(newRow, 0, new QTableWidgetItem(songItem->text()));
//         playlistForm->getUI()->PlayList->setItem(newRow, 1, new QTableWidgetItem(artistItem->text()));
//         playlistForm->getUI()->PlayList->setItem(newRow, 3, new QTableWidgetItem(ds->text()));

//         // 添加其他列的信息，如来源、时长等
//     }
//     QString id = ui->tableWidgetPlayList->item(row,4)->text();
//     QString surl = "http://music.163.com/song/media/outer/url?id=" + id + ".mp3";

//     QStringList headers;
//     headers<<"name"<<"singer"<<"from"<<"time";

// }

void Widget::openDownloadPath()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(lineEdit_downloadPath->text()));
}

void Widget::chooseDownloadPath()
{
     downloadPath = QFileDialog::getExistingDirectory(this, "保存路径", downloadPath, QFileDialog::ShowDirsOnly |QFileDialog::DontResolveSymlinks);
     if (downloadPath != "") {
        QObject *object = sender();
         qDebug() << object->objectName() << downloadPath;
         if (object->objectName() == "SettingDialogChooseDownloadPath") {
             lineEdit_downloadPath->setText(downloadPath);
         }
         if (object->objectName() == "DownloadDialogPath") {
             pushButton_path->setText(downloadPath);
             pushButton_path->setToolTip(downloadPath);
         }
         settings.setValue("DownloadPath", downloadPath);
     }
}


void Widget::showHideLyric(bool checked)
{
    QSettings settings("OrganizationName", "ApplicationName");

    qDebug() << settings.allKeys();
    if(checked){

        lyricWidget->show();
        ui->pushButtonLyric->setChecked(true);
        settings.setValue("LyricShow", true);
        settings.sync(); // 立即将设置的值写入到存储中
        QString lyricShow = settings.value("LyricShow").toString();
        qDebug() << "lyricShow" << lyricShow;
    }else
    {
        lyricWidget->hide();
        ui->pushButtonLyric->setChecked(false);
        settings.setValue("LyricShow", false);
        settings.sync(); // 立即将设置的值写入到存储中
        QString lyricShow = settings.value("LyricShow").toString();
        qDebug() << "lyricShow" << lyricShow;
    }


}



// 构建歌曲URL的函数
QUrl Widget::constructSongUrl(int songId)
{
    // 根据歌曲ID构建URL
    QString songUrlStr = QString("http://music.163.com/song/%1").arg(songId);
    return QUrl(songUrlStr);
}

// 构建MV URL的函数
QUrl Widget::constructMVUrl(int mvId)
{
    // 根据MV ID构建URL
    QString mvUrlStr = QString("http://music.163.com/mv/%1").arg(mvId);
    return QUrl(mvUrlStr);
}

// 设置当前播放歌单的函数


void Widget::searchSinger(QString singerName)
{
    int limit = 50;
    QString surl = "http://music.163.com/api/search/get";
    //QString spost = "type=1&s=" + ui->lineEdit->text() + "&limit=" + QString::number(limit) + "&offset=" + QString::number((ui->lineEditPage->text().toInt()-1)*limit);
    QString spost = "type=1&s=" + singerName + "&limit=" + QString::number(limit) + "&offset=0";
    qDebug() << surl + "?" + spost;
    //qDebug() << postReply(surl,spost);
    ui->stackedWidget->setCurrentIndex(2);
    ui->tableWidgetPlayList->setRowCount(0);
    QJsonDocument JD = QJsonDocument::fromJson(postReply(surl, spost));
    int code = JD.object().value("code").toInt();
    qDebug() << code;
    if (code == 200) {
        ui->tableWidgetPlayList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableWidgetPlayList->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableWidgetPlayList->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidgetPlayList->setAlternatingRowColors(true);
        ui->tableWidgetPlayList->setShowGrid(false);
        ui->tableWidgetPlayList->setFrameShape(QFrame::NoFrame);
        ui->tableWidgetPlayList->setFocusPolicy(Qt::NoFocus);
        ui->tableWidgetPlayList->setSortingEnabled(true);
        ui->tableWidgetPlayList->setColumnCount(8);
        ui->tableWidgetPlayList->setColumnHidden(4,true);
        ui->tableWidgetPlayList->setColumnHidden(5,true);
        ui->tableWidgetPlayList->setColumnHidden(6,true);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(0,380);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(1,150);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(2,135);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(3,80);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(7,40);

        QStringList header;
        header << "歌名" << "歌手" << "专辑" << "时长" << "id" << "专辑封面" << "mvid" << "MV";
        ui->tableWidgetPlayList->setHorizontalHeaderLabels(header);
        ui->tableWidgetPlayList->setStyleSheet("QTableView::item:selected { color:white; background:rgb((85,170,255));}"
                                               "QTableCornerButton::section { background-color:#ecf5ff; }"
                                               "QHeaderView::section { color:black; background-color:#ecf5ff; }"
                                               "QTableWidget { border: none; }"
                                               );

        QJsonArray songs = JD.object().value("result").toObject().value("songs").toArray();
        //qDebug() << songs;
        for(int i=0; i<songs.size(); i++){
            ui->tableWidgetPlayList->insertRow(i);
            ui->tableWidgetPlayList->setItem(i,0,new QTableWidgetItem(songs[i].toObject().value("name").toString()));
            ui->tableWidgetPlayList->setItem(i,1,new QTableWidgetItem(songs[i].toObject().value("artists").toArray()[0].toObject().value("name").toString()));
            ui->tableWidgetPlayList->setItem(i,2,new QTableWidgetItem(songs[i].toObject().value("album").toObject().value("name").toString()));
            int ds = songs[i].toObject().value("duration").toInt()/1000;
            QTableWidgetItem *TWI = new QTableWidgetItem(QString("%1:%2").arg(ds/60,2,10,QLatin1Char(' ')).arg(ds%60,2,10,QLatin1Char('0')));
            TWI->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            ui->tableWidgetPlayList->setItem(i,3,TWI);
            ui->tableWidgetPlayList->setItem(i,4,new QTableWidgetItem(QString::number(songs[i].toObject().value("id").toInt())));
            ui->tableWidgetPlayList->setItem(i,5,new QTableWidgetItem(songs[i].toObject().value("album").toObject().value("picUrl").toString()));
            int mvid = songs[i].toObject().value("mvid").toInt();
            ui->tableWidgetPlayList->setItem(i,6,new QTableWidgetItem(QString::number(mvid)));


            if(mvid != 0){
                QPushButton *pushButton_MV = new QPushButton;
                pushButton_MV->setFixedSize(24,24);
                pushButton_MV->setIcon(QIcon(":/picture/MV.png"));
                pushButton_MV->setIconSize(QSize(24,24));
                pushButton_MV->setFocusPolicy(Qt::NoFocus);
                pushButton_MV->setFlat(true);
                pushButton_MV->setCursor(Qt::PointingHandCursor);
                connect(pushButton_MV, SIGNAL(clicked()), this, SLOT(on_pushButtonMV_clicked()));
                ui->tableWidgetPlayList->setCellWidget(i,7,pushButton_MV);
            }
        }

        ui->tableWidgetPlayList->setColumnWidth(0, 345);
    }
}

void Widget::setSinger()
{
    QGridLayout *gridLayout1 = new QGridLayout(ui->tab_5); // 创建华语选项卡的布局管理器
    QGridLayout *gridLayout2 = new QGridLayout(ui->tab_8); // 创建欧美选项卡的布局管理器
    QGridLayout *gridLayout3 = new QGridLayout(ui->tab_9); // 创建日本选项卡的布局管理器
    QGridLayout *gridLayout4 = new QGridLayout(ui->tab_11); // 创建港台选项卡的布局管理器
    QGridLayout *gridLayout5 = new QGridLayout(ui->tab_12); // 创建韩国选项卡的布局管理器
    QStringList singerNames =
    {
    "薛之谦", "毛不易", "汪苏泷", "张杰", "单依纯", "周深", "颜人中", "赵雷", "郭顶", "马思唯", "张碧晨", "沈以诚", "胡彦斌", "梁博", "华晨宇",
    "Justin Bieber","The Weeknd","Charlie Puth","Imagine Dragons","Maroon 5","Ariana Grande","Alan Walker","Chester Young","Drake","The Chainsmokers","Ed Sheeran",
    "Rihanna","Bruno Mars","Eminem","Coldplay",
    "米津玄师","YOASOBI","RADWIMPS","宇多田光","泽野弘之","鏡音リン","Aimer","藤井风","久石譲","坂本龍一","milet","中島美嘉","椎名林檎","玉置浩二","爱缪",
    "陈奕迅","陶喆","方大同","薛凯琪","王菲","陈慧娴","邓丽君","邓紫棋","陈小春","李克勤","张敬轩","容祖儿","丁世光","古巨基","张学友",
    "(G)I-DLE","BIGBANG","BLACKPINK","Ashley Alisha","aespa","IU","T-ara","ROSÉ","BTS (防弹少年团)","J.Fla","朴宰范","IVE","少女时代","NewJeans","TWICE",
    };
    for (int i = 0; i < 75; ++i) {
        QPushButton *button = new QPushButton(singerNames[i]);
        button->setText(""); // 将文本内容设置为空字符串
        button->setProperty("singerName", singerNames[i]); // 将歌手名称存储为自定义属性
        button->setFixedSize(90, 115);
        button->setIconSize(QSize(93, 119));
        button->setFlat(true);
        button->setCursor(Qt::PointingHandCursor);
        QPixmap pixmap(":/singerpic/"+QString::number(i) + ".png");
        QIcon icon(pixmap);
        button->setIcon(icon);


        // button->setLayout(layout);
        connect(button, &QPushButton::clicked, this, &Widget::onSingerButtonClicked);

        if (i < 15) {
            int row = i / 5; // 计算行号
            int col = i % 5; // 计算列号
            gridLayout1->addWidget(button, row, col);
        }
        else if (i >= 15 && i < 30) {
            int row = (i - 15) / 5; // 计算行号
            int col = (i - 15) % 5; // 计算列号
            gridLayout2->addWidget(button, row, col);
        }
        else if (i >= 30 && i < 45) {
            int row = (i - 30) / 5; // 计算行号
            int col = (i - 30) % 5; // 计算列号
            gridLayout3->addWidget(button, row, col);
        }
        else  if(i >= 45 && i < 60){
            int row = (i - 45) / 5; // 计算行号
            int col = (i - 45) % 5; // 计算列号
            gridLayout4->addWidget(button, row, col);
        }
        else{
            int row = (i - 60) / 5; // 计算行号
            int col = (i - 60) % 5; // 计算列号
            gridLayout5->addWidget(button, row, col);
        }
    }
}

void Widget::onSingerButtonClicked()
{
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if (button) {
        QString singerName = button->property("singerName").toString(); // 获取按钮的自定义属性，即为歌手名称
        searchSinger(singerName);
    }
}





// 显示错误信息的函数
void Widget::displayErrorMessage(QString message)
{
    // 在界面上显示错误信息等操作
}
QByteArray Widget::getReply(QString surl)
{
    QNetworkAccessManager *NAM = new QNetworkAccessManager;
    QNetworkRequest request;
    request.setUrl(QUrl(surl));
    request.setRawHeader("Referer", "http://music.163.com/");
    qDebug() << "Request URL:" << surl; // 输出请求的URL
    QNetworkReply *reply = NAM->get(request);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QByteArray responseData = reply->readAll();
    reply->deleteLater();
    NAM->deleteLater();
    return responseData;
}

QByteArray Widget::postReply(QString surl, QString spost)
{
    QNetworkAccessManager *NAM = new QNetworkAccessManager;
    QNetworkRequest request;
    request.setUrl(QUrl(surl));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    QByteArray postData = spost.toUtf8();  // 将 QString 转换为 QByteArray
    QNetworkReply *reply = NAM->post(request, postData);
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();
    QByteArray responseData = reply->readAll();
    reply->deleteLater();
    NAM->deleteLater();
    return responseData;
}

void Widget::on_pushButtonPlay_clicked()
{
   switch(mediaPlayer->playbackState())
    {
    case QMediaPlayer::PlaybackState::StoppedState://停止状态
        {
            //如果没播放，那就播放当前选中音乐
            //播放当前选中的音乐
            //1.获取选中的行号
        curPlayIndex = ui->tableWidgetPlayList ->currentRow();
            //2.播放对应下标的音乐
            //mediaPlayer->setSource(playList[curPlayIndex]);
            mediaPlayer->play();
            break;
        }
    case QMediaPlayer::PlaybackState::PlayingState://播放状态
        mediaPlayer->pause();
        break;
    case QMediaPlayer::PlaybackState::PausedState://暂停状态
        mediaPlayer->play();
        break;
    }
}


void Widget::on_pushButtonLastsong_clicked()//上一曲
{


    int totalRows = ui->tableWidgetPlayList->rowCount();

    int row = ui->tableWidgetPlayList->currentRow();

    //qDebug() << row;

    if (row == 0)
    {
        row = totalRows;

        row--;
        playSong(row);
        ui->tableWidgetPlayList->setCurrentCell(row,0);

    }
    else
    {

        row--;
        playSong(row);
        ui->tableWidgetPlayList->setCurrentCell(row,0);

    }

}


 void Widget::on_pushButtonNextsong_clicked()//下一曲
 {

     int totalRows = ui->tableWidgetPlayList->rowCount();

     int row = ui->tableWidgetPlayList->currentRow();

     //qDebug() << row;

     if (row == totalRows-1)
     {
         row = -1;

        row++;
        playSong(row);
        ui->tableWidgetPlayList->setCurrentCell(row,0);

     }
     else
     {

        row++;
        playSong(row);
        ui->tableWidgetPlayList->setCurrentCell(row,0);

     }
 }







void Widget::on_pushButtonSound_clicked()
{
    float previousVolume = mediaPlayer->audioOutput()->volume(); // 存储当前音量
    bool mute = mediaPlayer->audioOutput()->isMuted();
    if (!mute)
    {
        mediaPlayer->audioOutput()->setVolume(0);

    }
    else if(mute)
    {
        mediaPlayer->audioOutput()->setVolume(previousVolume); // 恢复先前的音量
    }
    // 更新静音状态
    mute = mediaPlayer->audioOutput()->isMuted();
}




void Widget::on_verticalSliderVolumn_valueChanged(int value)
{
    mediaPlayer->audioOutput()->setVolume(value / 100.0);

}

void Widget::on_horizontalSliderMVvolume_valueChanged(int value)
{
    VideoPlayer->audioOutput()->setVolume(value / 100.0);
}

void Widget::on_pushButtonSearch_clicked()
{
    int limit = 50;
    QString surl = "http://music.163.com/api/search/get";
    //QString spost = "type=1&s=" + ui->lineEdit->text() + "&limit=" + QString::number(limit) + "&offset=" + QString::number((ui->lineEditPage->text().toInt()-1)*limit);
    QString spost = "type=1&s=" + ui->lineEdit->text() + "&limit=" + QString::number(limit) + "&offset=0";
    qDebug() << surl + "?" + spost;
    //qDebug() << postReply(surl,spost);
    ui->stackedWidget->setCurrentIndex(2);
    ui->tableWidgetPlayList->setRowCount(0);
    QJsonDocument JD = QJsonDocument::fromJson(postReply(surl, spost));
    int code = JD.object().value("code").toInt();
    qDebug() << code;
    if (code == 200) {
        ui->tableWidgetPlayList->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableWidgetPlayList->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tableWidgetPlayList->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tableWidgetPlayList->setAlternatingRowColors(true);
        ui->tableWidgetPlayList->setShowGrid(false);
        ui->tableWidgetPlayList->setFrameShape(QFrame::NoFrame);
        ui->tableWidgetPlayList->setFocusPolicy(Qt::NoFocus);
        ui->tableWidgetPlayList->setSortingEnabled(true);
        ui->tableWidgetPlayList->setColumnCount(8);
        ui->tableWidgetPlayList->setColumnHidden(4,true);
        ui->tableWidgetPlayList->setColumnHidden(5,true);
        ui->tableWidgetPlayList->setColumnHidden(6,true);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(0,380);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(1,150);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(2,135);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(3,80);
        ui->tableWidgetPlayList->horizontalHeader()->resizeSection(7,40);

        QStringList header;
        header << "歌名" << "歌手" << "专辑" << "时长" << "id" << "专辑封面" << "mvid" << "MV";
        ui->tableWidgetPlayList->setHorizontalHeaderLabels(header);
        ui->tableWidgetPlayList->setStyleSheet("QTableView::item:selected { color:white; background:rgb((85,170,255));}"
                                               "QTableCornerButton::section { background-color:#ecf5ff; }"
                                               "QHeaderView::section { color:black; background-color:#ecf5ff; }"
                                               "QTableWidget { border: none; }"
                                               );

        QJsonArray songs = JD.object().value("result").toObject().value("songs").toArray();
        //qDebug() << songs;
        for(int i=0; i<songs.size(); i++){
            ui->tableWidgetPlayList->insertRow(i);
            ui->tableWidgetPlayList->setItem(i,0,new QTableWidgetItem(songs[i].toObject().value("name").toString()));
            ui->tableWidgetPlayList->setItem(i,1,new QTableWidgetItem(songs[i].toObject().value("artists").toArray()[0].toObject().value("name").toString()));
            ui->tableWidgetPlayList->setItem(i,2,new QTableWidgetItem(songs[i].toObject().value("album").toObject().value("name").toString()));
            int ds = songs[i].toObject().value("duration").toInt()/1000;
            QTableWidgetItem *TWI = new QTableWidgetItem(QString("%1:%2").arg(ds/60,2,10,QLatin1Char(' ')).arg(ds%60,2,10,QLatin1Char('0')));
            TWI->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);
            ui->tableWidgetPlayList->setItem(i,3,TWI);
            ui->tableWidgetPlayList->setItem(i,4,new QTableWidgetItem(QString::number(songs[i].toObject().value("id").toInt())));
            ui->tableWidgetPlayList->setItem(i,5,new QTableWidgetItem(songs[i].toObject().value("album").toObject().value("picUrl").toString()));
            int mvid = songs[i].toObject().value("mvid").toInt();
           ui->tableWidgetPlayList->setItem(i,6,new QTableWidgetItem(QString::number(mvid)));


            if(mvid != 0){
                QPushButton *pushButton_MV = new QPushButton;
                pushButton_MV->setFixedSize(24,24);
                pushButton_MV->setIcon(QIcon(":/picture/MV.png"));
                pushButton_MV->setIconSize(QSize(24,24));
                pushButton_MV->setFocusPolicy(Qt::NoFocus);
                pushButton_MV->setFlat(true);
                pushButton_MV->setCursor(Qt::PointingHandCursor);
                connect(pushButton_MV, SIGNAL(clicked()), this, SLOT(on_pushButtonMV_clicked()));
                ui->tableWidgetPlayList->setCellWidget(i,7,pushButton_MV);
            }
        }

        ui->tableWidgetPlayList->setColumnWidth(0, 345);
}
}
void Widget::on_pushButtonMV_clicked()
{
    videoWidget = new QVideoWidget;
    ui->stackedWidget->setCurrentIndex(1);

    QPushButton *senderObj = qobject_cast<QPushButton*>(sender());
    if (senderObj == nullptr) {
        return;
    }

    QModelIndex index =  ui->tableWidgetPlayList->indexAt(QPoint(senderObj->frameGeometry().x(),senderObj->frameGeometry().y()));
    int row = index.row();
    ui->tableWidgetPlayList->setCurrentCell(row,0);
    QString songname =  ui->tableWidgetPlayList->item(row,0)->text() + "\n" +  ui->tableWidgetPlayList->item(row,1)->text();
    //navWidget->label_songname->setText(songname);
    QString mvid =  ui->tableWidgetPlayList->item(row,6)->text();
    QString surl = "http://music.163.com/api/mv/detail/?type=mp4&id=" + mvid;


    qDebug() << "getMV -> " <<  surl;
    QJsonDocument JD = QJsonDocument::fromJson(getReply(surl));
    qDebug() << JD;
    QJsonObject brs = JD.object().value("data").toObject().value("brs").toObject();
    QString mvurl = brs.value("1080").toString();
    qDebug() << 1080 << mvurl;
    if (mvurl == "") {
        mvurl = brs.value("720").toString();
        qDebug() << 720 << mvurl;
    }
    if (mvurl == "") {
        mvurl = brs.value("480").toString();
        qDebug() << 480 << mvurl;
    }
    if (mvurl == "") {
        mvurl = brs.value("240").toString();
        qDebug() << 240 << mvurl;
    }
    //lyricWidget->hide();

    ui->stackedWidget->setCurrentWidget(videoWidget);
    VideoPlayer->setSource(QUrl(mvurl));
    VideoPlayer->play();

    QPixmap pixmap;
    pixmap.loadFromData(getReply( ui->tableWidgetPlayList->item(row,5)->text()));
    ui->pushButtondetails->setIcon(QIcon(pixmap));
    //pixmap.save(QDir::currentPath() + "/cover.jpg");
}
void Widget::on_pushButtoncloth_clicked()
{
    if(changeColor)
    {
        if(changeColor->isHidden())
        {
            changeColor->show();
        }
        else {
            changeColor->hide();
        }

    }
    else {
        changeColor = new ChangeColorForm(this);
        changeColor->setGeometry(780,30,409,317); //这里要调整好
        changeColor->raise();  //提示显示层数
        changeColor->show();
}
}

void Widget::on_pushButtonFindmusic_clicked()
{

}

void Widget::durationChange(qint64 d)
{
    ui->PlayCourseSlider->setMaximum(static_cast<int>(d));
    QTime t(0,0,0);
    t = t.addMSecs(static_cast<int>(d));
    ui->totallabel->setText(t.toString("mm:ss"));
}

void Widget::positionChange(qint64 p)
{
    //qDebug() << "position =" << p;
    if(!ui->PlayCourseSlider->isSliderDown())ui->PlayCourseSlider->setValue(static_cast<int>(p));
    QTime t(0,0,0);
    t = t.addMSecs(static_cast<int>(p));
    ui->Curlabel->setText(t.toString("mm:ss"));

    // 歌词选行
    int hl=0;
    // 非最后一句
    for (int i=0; i<lyrics.size()-1; i++)
    {
        if (t>lyrics.at(i).time && t<lyrics.at(i+1).time)
        {
            lyricWidget->text = lyrics.at(i).sentence;
            lyricWidget->lp = static_cast<float>(lyrics.at(i).time.msecsTo(t)) / lyrics.at(i).time.msecsTo(lyrics.at(i+1).time);
            lyricWidget->update();
            hl=i;
            break;
        }
    }
    //最后一句
    if (lyrics.size()>0) {
        int j = lyrics.size() - 1;
        if (t>lyrics.at(j).time) {
            lyricWidget->text = lyrics.at(j).sentence;
            lyricWidget->update();
            hl=j;
        }

}
    // 歌词文本着色
    for(int a=0; a<lyrics.size(); a++){
        QTextCursor cursor(ui->textBrowser->document()->findBlockByLineNumber(a));
        QTextBlockFormat TBF = cursor.blockFormat();
        TBF.setForeground(QBrush(Qt::white));
        //TBF.setForeground(QBrush(QColor("#409eff")));
        //TBF.setBackground(QBrush(Qt::transparent));
        TBF.clearBackground();
        cursor.setBlockFormat(TBF);
    }
    if(lyrics.size()>0){
        QTextCursor cursor1(ui->textBrowser->document()->findBlockByLineNumber(hl));
        QTextBlockFormat TBF1 = cursor1.blockFormat();
        TBF1.setForeground(QBrush(Qt::blue));
        //TBF1.setBackground(QBrush(QColor(255,255,255,80)));
        TBF1.setBackground(QBrush(QColor(85,170,255,80)));
        cursor1.setBlockFormat(TBF1);
        //textBrowser->setTextCursor(cursor1);
        QScrollBar *scrollBar = ui->textBrowser->verticalScrollBar();
        scrollBar->setSliderPosition(scrollBar->maximum()*hl/(lyrics.size()));
    }
}

void Widget::playSong(int row)
{




    //记得改回来
    QString id = ui->tableWidgetPlayList->item(row,4)->text();
    QString surl = "http://music.163.com/song/media/outer/url?id=" + id + ".mp3";
    mediaPlayer->setSource(QUrl(surl));
    mediaPlayer->play();

    ui->labelSongname->setText(ui->tableWidgetPlayList->item(row, 0)->text() + "\n" + ui->tableWidgetPlayList->item(row, 1)->text());
    ui->labeldetail->setText(ui->tableWidgetPlayList->item(row, 0)->text() + "\n" + ui->tableWidgetPlayList->item(row, 1)->text());
    lyricWidget->text = ui->tableWidgetPlayList->item(row,0)->text() + " - " + ui->tableWidgetPlayList->item(row,1)->text();
    lyricWidget->update();
    getLyric(id);
    ui->textBrowser->setStyleSheet("QTextBrowser { color: black; font-size: 25px; background-color: transparent; border: none;line-height: 500%; }");
    if (ui->tableWidgetPlayList->columnCount() > 5) {
        if (!ui->tableWidgetPlayList->item(row, 5) || ui->tableWidgetPlayList->item(row, 5)->text().isEmpty()) {
            // 如果第5列的内容为空，则不执行后续操作
            return;
        }

        QPixmap pixmap;
        pixmap.loadFromData(getReply(ui->tableWidgetPlayList->item(row, 5)->text()));
        ui->pushButtondetails->setIcon(QIcon(pixmap));
        ui->pushButtonalbum->setIcon(QIcon(pixmap));
        //pixmap.save(QDir::currentPath() + "/cover.jpg");
        qDebug() << QDir::currentPath() + "/cover.jpg";
    }


}


void Widget::on_pushButtonLoop_clicked(bool checked)
{
    if (checked) {
        // 按钮被选中，表示要开启单曲循环
        disconnect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Widget::onMediaStatusChangedListLoop);
        connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Widget::onMediaStatusChangedSingleLoop);
        QIcon icon(":/picture/SingleLoop1.png");
        // 将图标设置给 pushButtonLoop
        ui->pushButtonLoop->setIcon(icon);
        qDebug() << "Single loop enabled";
    } else {
        // 按钮未被选中，表示要开启列表循环
        disconnect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Widget::onMediaStatusChangedSingleLoop);
        connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, &Widget::onMediaStatusChangedListLoop);
        QIcon icon(":/picture/ListLoop1.png");
        // 将图标设置给 pushButtonLoop
        ui->pushButtonLoop->setIcon(icon);
        qDebug() << "List loop enabled";
    }
}

void Widget::onMediaStatusChangedSingleLoop(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        mediaPlayer->setPosition(0); // 将播放位置设置为起始位置
        mediaPlayer->play(); // 重新播放
        qDebug()<<"loop";
    }
}

void Widget::onMediaStatusChangedListLoop(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::EndOfMedia)
    {
        int totalRows = ui->tableWidgetPlayList->rowCount();
        int row = ui->tableWidgetPlayList->currentRow();

        int nextRow = (row + 1) % totalRows; // 计算下一首歌曲的行号

        playSong(nextRow); // 播放下一首歌曲
        ui->tableWidgetPlayList->setCurrentCell(nextRow, 0); // 设置表格当前选中行
        qDebug()<<"listloop";
    }
}


void Widget::addMyloveSongToPlayList(QTableWidgetItem* item)
{
    settableWidgetPlayListlayout();
    if (item) {
        int row = item->row();
        QTableWidgetItem *item_songName = ui->tableWidgetMylove->item(row, 0);
        QTableWidgetItem *item_artist = ui->tableWidgetMylove->item(row, 1);
        QTableWidgetItem *item_album = ui->tableWidgetMylove->item(row, 2);
        QTableWidgetItem *item_duration = ui->tableWidgetMylove->item(row, 3);
        QTableWidgetItem *item_id = ui->tableWidgetMylove->item(row, 4);

        // 将歌曲信息添加到 ui->tableWidgetPlayList 表格中
        int rowCount = ui->tableWidgetPlayList->rowCount();
        ui->tableWidgetPlayList->insertRow(rowCount);
        ui->tableWidgetPlayList->setItem(rowCount, 0, new QTableWidgetItem(item_songName->text()));
        ui->tableWidgetPlayList->setItem(rowCount, 1, new QTableWidgetItem(item_artist->text()));
        ui->tableWidgetPlayList->setItem(rowCount, 2, new QTableWidgetItem(item_album->text()));
        ui->tableWidgetPlayList->setItem(rowCount, 3, new QTableWidgetItem(item_duration->text()));
        ui->tableWidgetPlayList->setItem(rowCount, 4, new QTableWidgetItem(item_id->text()));

    }
}
void Widget::addMycreateSongToPlayList(QTableWidgetItem* item)
{
    settableWidgetPlayListlayout();
    if (item) {
        int row = item->row();
        QTableWidgetItem *item_songName = ui->tableWidgetMycreate->item(row, 0);
        QTableWidgetItem *item_artist = ui->tableWidgetMycreate->item(row, 1);
        QTableWidgetItem *item_album = ui->tableWidgetMycreate->item(row, 2);
        QTableWidgetItem *item_duration = ui->tableWidgetMycreate->item(row, 3);
        QTableWidgetItem *item_id = ui->tableWidgetMycreate->item(row, 4);

        // 将歌曲信息添加到 ui->tableWidgetPlayList 表格中
        int rowCount = ui->tableWidgetPlayList->rowCount();
        ui->tableWidgetPlayList->insertRow(rowCount);
        ui->tableWidgetPlayList->setItem(rowCount, 0, new QTableWidgetItem(item_songName->text()));
        ui->tableWidgetPlayList->setItem(rowCount, 1, new QTableWidgetItem(item_artist->text()));
        ui->tableWidgetPlayList->setItem(rowCount, 2, new QTableWidgetItem(item_album->text()));
        ui->tableWidgetPlayList->setItem(rowCount, 3, new QTableWidgetItem(item_duration->text()));
        ui->tableWidgetPlayList->setItem(rowCount, 4, new QTableWidgetItem(item_id->text()));

    }
}

void Widget::on_pushButtonLastpage_clicked()
{

    int previousPageIndex = currentPageIndex;
    if (currentPageIndex > 0) {
        currentPageIndex--;
    }
    ui->stackedWidget->setCurrentIndex(currentPageIndex);
    qDebug() << "Previous page index: " << previousPageIndex << ", Current page index: " << currentPageIndex;
}

void Widget::on_pushButtonNextpage_clicked()
{
    int previousPageIndex = currentPageIndex;
    if (currentPageIndex < ui->stackedWidget->count() - 1) {
        currentPageIndex++;
    }
    ui->stackedWidget->setCurrentIndex(currentPageIndex);
    qDebug() << "Previous page index: " << previousPageIndex << ", Current page index: " << currentPageIndex;
}

void Widget::setMylovelist()
{

    ui->tableWidgetMylove->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetMylove->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetMylove->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetMylove->setAlternatingRowColors(true);
    ui->tableWidgetMylove->setShowGrid(false);
    ui->tableWidgetMylove->setFrameShape(QFrame::NoFrame);
    ui->tableWidgetMylove->setFocusPolicy(Qt::NoFocus);
    ui->tableWidgetMylove->setSortingEnabled(true);
    ui->tableWidgetMylove->setColumnCount(5);
    ui->tableWidgetMylove->setColumnHidden(4,true);
    ui->tableWidgetMylove->horizontalHeader()->resizeSection(0,380);
    ui->tableWidgetMylove->horizontalHeader()->resizeSection(1,155);
    ui->tableWidgetMylove->horizontalHeader()->resizeSection(2,135);
    ui->tableWidgetMylove->horizontalHeader()->resizeSection(3,70);
    ui->tableWidgetMylove->horizontalHeader()->resizeSection(4,30);

    QStringList header;
    header << "歌名" << "歌手" << "专辑" << "时长" << "id";
    ui->tableWidgetMylove->setHorizontalHeaderLabels(header);
    ui->tableWidgetMylove->setStyleSheet("QTableView::item:selected { color:white; background:rgb((85,170,255));}"
                                           "QTableCornerButton::section { background-color:#ecf5ff; }"
                                           "QHeaderView::section { color:black; background-color:#ecf5ff; }"
                                           "QTableWidget { border: none; }"
                                           );
    // 加载数据到表格
    QList<QMap<QString, QString>> MyloveSongs = loadMyloveSongs();
    ui->tableWidgetMylove->setRowCount(MyloveSongs.size());
    for (int i = 0; i < MyloveSongs.size(); ++i) {
        QMap<QString, QString> song = MyloveSongs.at(i);
        QTableWidgetItem *item_songName = new QTableWidgetItem(song["songName"]);
        QTableWidgetItem *item_artist = new QTableWidgetItem(song["artist"]);
        QTableWidgetItem *item_album = new QTableWidgetItem(song["album"]);
        QTableWidgetItem *item_duration = new QTableWidgetItem(song["duration"]);
        QTableWidgetItem *item_id = new QTableWidgetItem(song["id"]);

        ui->tableWidgetMylove->setItem(i, 0, item_songName);
        ui->tableWidgetMylove->setItem(i, 1, item_artist);
        ui->tableWidgetMylove->setItem(i, 2, item_album);
        ui->tableWidgetMylove->setItem(i, 3, item_duration);
        ui->tableWidgetMylove->setItem(i, 4, item_id);
    }
    connect(ui->tableWidgetMylove, &QTableWidget::cellDoubleClicked, this, [this](int row, int column){
        addMyloveSongToPlayList(ui->tableWidgetMylove->item(row, 0));
    });

    ui->labelTotal->setText("共"+QString::number(ui->tableWidgetMylove->rowCount())+"首");
}

void Widget::on_pushButtonaddMylove_clicked(bool checked)
{
    //QSqlDatabase db = dbManager::getSqlDataBase("QTMusicPlayer", "QTMusicPlayer");
    QSqlDatabase db = dbManager::getSqlDataBase("Mylove", "QTMusicPlayer.db");

    if (!db.isOpen())
    {
        if (!db.open())
        {
            qDebug() << "Failed to open database.";
            return;
        }
    }


    int currentRow = ui->tableWidgetPlayList->currentRow();
    QString id = ui->tableWidgetPlayList->item(currentRow, 4)->text();

    // 检查歌曲是否已经在我喜欢的歌单中
    bool isSongInMyLoveList = false;
    for (int i = 0; i < ui->tableWidgetMylove->rowCount(); ++i) {
        if (ui->tableWidgetMylove->item(i, 4)->text() == id) {
            // 如果歌曲已经在我喜欢的歌单中，则移除它
            ui->tableWidgetMylove->removeRow(i);
            isSongInMyLoveList = true;
            // 从数据库中删除该歌曲
            QString deleteQuery = "DELETE FROM Mylove WHERE id = '" + id + "'";
            QSqlQuery query(db);
            qDebug()<<"已成功移出我喜欢";

            QIcon icon(":/picture/MyLove.png");
            ui->pushButtonaddMylove->setIcon(icon);
            if (!query.exec(deleteQuery)) {
                qDebug() << "Delete query failed: " << query.lastError().text();
            }
            break;
        }
    }

    // 如果歌曲不在我喜欢的歌单中，则添加它
    if (!isSongInMyLoveList) {
        QIcon icon(":/picture/Mylove1.png");
        ui->pushButtonaddMylove->setIcon(icon);
        QString songName = ui->tableWidgetPlayList->item(currentRow, 0)->text();
        QString singer = ui->tableWidgetPlayList->item(currentRow, 1)->text();
        QString album = ui->tableWidgetPlayList->item(currentRow, 2)->text();
        QString duration = ui->tableWidgetPlayList->item(currentRow, 3)->text();

        int row = ui->tableWidgetMylove->rowCount();
        ui->tableWidgetMylove->insertRow(row);
        ui->tableWidgetMylove->setItem(row, 0, new QTableWidgetItem(songName));
        ui->tableWidgetMylove->setItem(row, 1, new QTableWidgetItem(singer));
        ui->tableWidgetMylove->setItem(row, 2, new QTableWidgetItem(album));
        ui->tableWidgetMylove->setItem(row, 3, new QTableWidgetItem(duration));
        ui->tableWidgetMylove->setItem(row, 4, new QTableWidgetItem(id));

        // 将歌曲插入到数据库
        QString insertQuery = "INSERT INTO Mylove (songName, artist, album, id, duration) VALUES ('" + songName + "', '" + singer + "', '" + album + "', '" + id + "', '" + duration + "')";
        QSqlQuery query(db);
        qDebug()<<"已成功添加到我喜欢";
        if (!query.exec(insertQuery)) {
            qDebug() << "Insert query failed: " << query.lastError().text();
        }
    }

    setMylovelist();
}

void Widget::addMyloveSongs(QString songName, QString artist, QString album, int id, int duration)
{
    QSqlQuery query;
    query.prepare("INSERT INTO Mylove (songName, artist, album,id,duration) VALUES (:songName, :artist, :album,:id,:duration)");
    query.bindValue(":songName", songName);
    query.bindValue(":artist", artist);
    query.bindValue(":album", album);
    query.bindValue(":id",id);
    query.bindValue(":duration",duration);
    query.exec();
}
// 加载喜欢的歌曲列表
QList<QMap<QString, QString>> Widget::loadMyloveSongs()
{
    QSqlDatabase db = dbManager::getSqlDataBase("Mylove", "QTMusicPlayer.db");


    QList<QMap<QString, QString>> MyloveSongs;
    QSqlQuery query(db);
    query.exec("SELECT * FROM Mylove");
    while (query.next()) {
        QMap<QString, QString> song;
        song["songName"] = query.value("songName").toString();
        song["artist"] = query.value("artist").toString();
        song["album"] = query.value("album").toString();
        song["id"] = query.value("id").toString();
        song["duration"] = query.value("duration").toString();
        MyloveSongs.append(song);
    }
    return MyloveSongs;
}
void Widget::tablecustomContextMenuRequest(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);

    QAction *play = menu->addAction(QIcon(":/picture/play1 .png"), "播放");
    connect(play, &QAction::triggered, this, &Widget::playSong);

    QAction *deletesong = menu->addAction(QIcon(":/picture/deletelist.png"), "从歌单移除歌曲");
    connect(deletesong, &QAction::triggered, this, [=](){
        // 获取当前选中的行
        int currentRow = ui->tableWidgetPlayList->currentRow();
        // 检查是否有选中的行
        if (currentRow >= 0) {
            // 移除选中的行
            ui->tableWidgetPlayList->removeRow(currentRow);
        }
    });
    menu->addSeparator();

    QAction *Collecttolist = menu->addAction(QIcon(":/picture/addsongtolist.png"), "收藏到歌单");
    QMenu *collectChild = new QMenu(this);
    Collecttolist->setMenu(collectChild);

    QAction *creatnewSheet = collectChild->addAction(QIcon(":/picture/createlist.png"), "创建新歌单");
    connect(creatnewSheet, &QAction::triggered, this, &Widget::on_pushButtonAddmusiclist_clicked);

    // 添加已有歌单选项
    for (int i = 0; i < musicListWidget->count(); ++i)
    {
        QString musicListName = musicListWidget->item(i)->text();
        QAction *existingListAction = collectChild->addAction(musicListName);
        connect(existingListAction, &QAction::triggered, [=](){
            // 在这里实现将歌曲添加到选定的歌单中的逻辑
            QString selectedListName = existingListAction->text();
            // 添加歌曲到选定的歌单中
            // 将当前选中的歌曲添加到选定的歌单中
            // 假设 db 是您的 QSqlDatabase 对象
            QSqlDatabase db = dbManager::getSqlDataBase("Myceate", "QTMusicPlayer.db");
            QString tableName = "playlist_" + musicListName; // 为该歌单创建对应的表名
            QSqlQuery query(db);
            query.exec("CREATE TABLE " + tableName + " (songName TEXT, singer TEXT, album TEXT, duration INTEGER, id INTEGER)");



            // 获取当前选中的歌曲信息
            int currentRow = ui->tableWidgetPlayList->currentRow();
            QString id = ui->tableWidgetPlayList->item(currentRow, 4)->text();
            QString songName = ui->tableWidgetPlayList->item(currentRow, 0)->text();
            QString singer = ui->tableWidgetPlayList->item(currentRow, 1)->text();
            QString album = ui->tableWidgetPlayList->item(currentRow, 2)->text();
            QString duration = ui->tableWidgetPlayList->item(currentRow, 3)->text();
            // 检查是否同一首歌已经存在于选定的歌单中

            QSqlQuery checkQuery(db);
            checkQuery.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id = :id");
            checkQuery.bindValue(":id", id);
            if (checkQuery.exec() && checkQuery.next()) {
                int count = checkQuery.value(0).toInt();
                if (count > 0) {
                    QMessageBox::warning(this, "错误", "同一首歌已经存在于该歌单中");
                    return;
                }
            } else {
                qDebug() << "Check query failed:" << checkQuery.lastError();
                return;
            }

            // 将歌曲信息插入到新创建的表中
            query.prepare("INSERT INTO " + tableName + " (songName, singer, album, duration, id) VALUES (:songName, :singer, :album, :duration, :id)");
            query.bindValue(":songName", songName);
            query.bindValue(":singer", singer);
            query.bindValue(":album", album);
            query.bindValue(":duration", duration);
            query.bindValue(":id", id);
            query.exec();



        });
    }

    QAction *mylove = collectChild->addAction(QIcon(":/picture/Mylove1.png"), "我喜欢的音乐");
    connect(mylove, &QAction::triggered, this, &Widget::on_pushButtonaddMylove_clicked);

    qDebug() << "Right-clicked at position:" << pos;
    menu->exec(QCursor::pos());

}
void Widget::on_pushButtonAddmusiclist_clicked()
{
    QString musicListName = QInputDialog::getText(this, "新建歌单", "请输入新歌单标题:");

    // 检查歌单名称是否为空
    if (musicListName.isEmpty()) {
        QMessageBox::warning(this, "错误", "歌单名称不能为空");
        return;
    }
    // 将歌单名称添加到数据库中
    for (int i = 0; i < ui->listWidget->count(); ++i) {
        if (ui->listWidget->item(i)->text() == musicListName) {
            // 如果在listWidget中找到播放列表名称，不要创建它
            QMessageBox::warning(this, "错误", "歌单名称不能相同");
            return; // 退出函数
        }
    }
    // 获取数据库实例
    QSqlDatabase db = dbManager::getSqlDataBase("Myceate", "QTMusicPlayer.db");
    // 仅在播放列表名称唯一时创建表
    QString tableName = "playlist_" + musicListName; // 根据播放列表名称创建表名称
    QSqlQuery query(db);

    // 准备SQL语句以创建表
    if (!query.exec("CREATE TABLE IF NOT EXISTS " + tableName + " ("
                                                                "songName TEXT, "
                                                                "singer TEXT, "
                                                                "album TEXT, "
                                                                "duration INTEGER, "
                                                                "id INTEGER PRIMARY KEY)")) {
        // 如果创建表失败，输出错误信息
        qDebug() << "创建表失败：" << query.lastError();
        return;
    }
    // 将歌单名称添加到ui->listWidget中
 ui->listWidget->addItem(musicListName);


}


// 当在ui->listWidget中选择某个歌单时的槽函数
void Widget:: onMusicListSelected()
{
    createMusiclistlayout();
    connect(ui->listWidget, &QListWidget::itemClicked, this, [=]()
            {
                ui->stackedWidget->setCurrentIndex(6);
        //获取用户选择的歌单名称
        QString selectedMusicList = ui->listWidget->currentItem()->text();
        ui->labelMycreate->setText(selectedMusicList);
            });
}

void Widget::createMusiclistlayout()
{
    ui->tableWidgetMycreate->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidgetMycreate->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetMycreate->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetMycreate->setAlternatingRowColors(true);
    ui->tableWidgetMycreate->setShowGrid(false);
    ui->tableWidgetMycreate->setFrameShape(QFrame::NoFrame);
    ui->tableWidgetMycreate->setFocusPolicy(Qt::NoFocus);
    ui->tableWidgetMycreate->setSortingEnabled(true);
    ui->tableWidgetMycreate->setColumnCount(5);
    ui->tableWidgetMycreate->setColumnHidden(4,true);
    ui->tableWidgetMycreate->horizontalHeader()->resizeSection(0,380);
    ui->tableWidgetMycreate->horizontalHeader()->resizeSection(1,155);
    ui->tableWidgetMycreate->horizontalHeader()->resizeSection(2,135);
    ui->tableWidgetMycreate->horizontalHeader()->resizeSection(3,70);
    ui->tableWidgetMycreate->horizontalHeader()->resizeSection(4,30);

    QStringList header;
    header << "歌名" << "歌手" << "专辑" << "时长" << "id";
    ui->tableWidgetMycreate->setHorizontalHeaderLabels(header);
    ui->tableWidgetMycreate->setStyleSheet("QTableView::item:selected { color:white; background:rgb((85,170,255));}"
                                         "QTableCornerButton::section { background-color:#ecf5ff; }"
                                         "QHeaderView::section { color:black; background-color:#ecf5ff; }"
                                         "QTableWidget { border: none; }"
                                         );

    connect(ui->tableWidgetMycreate, &QTableWidget::cellDoubleClicked, this, [this](int row, int column){
        addMycreateSongToPlayList(ui->tableWidgetMycreate->item(row, 0));
    });

}



void Widget::on_listWidget_itemClicked(QListWidgetItem *item)
{
    QSqlDatabase db = dbManager::getSqlDataBase("Myceate", "QTMusicPlayer.db");
    // 获取歌单名
    QString playlistName = item->text();
    QString tableName = "playlist_" + playlistName;
    // 准备SQL查询
    QString sql = "SELECT songName, singer, album ,duration , id FROM " + tableName;
    // 执行查询
    QSqlQuery query(db);
    if (query.exec(sql))
    {
        // 清空表格
        ui->tableWidgetMycreate->clearContents();
        ui->tableWidgetMycreate->setRowCount(0);

        // 遍历查询结果，并添加到表格中
        while (query.next())
        {
            int row = ui->tableWidgetMycreate->rowCount();
            ui->tableWidgetMycreate->insertRow(row);
            ui->tableWidgetMycreate->setItem(row, 0, new QTableWidgetItem(query.value("songName").toString()));
            ui->tableWidgetMycreate->setItem(row, 1, new QTableWidgetItem(query.value("singer").toString()));
            ui->tableWidgetMycreate->setItem(row, 2, new QTableWidgetItem(query.value("album").toString()));
            ui->tableWidgetMycreate->setItem(row, 3, new QTableWidgetItem(query.value("duration").toString()));
            ui->tableWidgetMycreate->setItem(row, 4, new QTableWidgetItem(query.value("id").toString()));

        }
    }
    else
    {
        // 处理错误
        qDebug() << "Query failed:" << query.lastError();
    }
}


void Widget::loadPlaylistNames()
{
    // 清空现有的列表项
    ui->listWidget->clear();
    QSqlDatabase db = dbManager::getSqlDataBase("Myceate", "QTMusicPlayer.db");
    // 查询数据库中所有的表名
    QSqlQuery query(db);
    if (query.exec("SELECT name FROM sqlite_master WHERE type='table' AND name LIKE 'playlist_%'"))
    {
        while (query.next())
        {
            QString tableName = query.value(0).toString();
            // 从表名中提取歌单名
            QString playlistName = tableName.mid(9); // 假设表名格式为 "playlist_歌单名"
            // 创建一个 QListWidgetItem 对象
            QListWidgetItem *item = new QListWidgetItem(playlistName, ui->listWidget);
            // 设置图标
            QIcon icon(":/picture/PlayList.png");
            item->setIcon(icon);
        }
    }
    else
    {
        // 处理错误
        qDebug() << "Query failed:" << query.lastError();
    }
}




void Widget::listWidgettablecustomContextMenuRequest(const QPoint &pos)
{  QMenu *menu = new QMenu(this);

    QAction *playAllcreate = menu->addAction(QIcon(":/picture/play1 .png"), "播放全部");
    connect(playAllcreate, &QAction::triggered, this, &Widget::on_pushButtonPlayallMycreate_clicked);

    QAction *deletelist = menu->addAction(QIcon(":/picture/deletelist.png"), "删除该歌单");
    connect(deletelist, &QAction::triggered, [=](){
        // 删除歌单的逻辑
        QListWidgetItem *selectedItem = ui->listWidget->itemAt(pos);
        if (selectedItem) {
            QString playlistNameToDelete = selectedItem->text();
            // 在这里实现删除歌单的逻辑
            // 确保删除数据库中的对应表等操作
            QSqlDatabase db = dbManager::getSqlDataBase("Myceate", "QTMusicPlayer.db");
            QString tableNameToDelete = "playlist_" + playlistNameToDelete;
            QSqlQuery query(db);
            query.exec("DROP TABLE " + tableNameToDelete);
            // 删除成功后，还需要在界面上刷新歌单列表等操作
        }
    });

    menu->exec(ui->listWidget->mapToGlobal(pos));
    loadPlaylistNames();
}

void Widget::on_pushButtonPlayall_clicked()
{
    // 清空播放列表
    ui->tableWidgetPlayList->setRowCount(0);

    // 获取歌曲数量
    int rowCount = ui->tableWidgetMylove->rowCount();

    // 将歌曲逐一添加到播放列表中
    for (int i = 0; i < rowCount; ++i) {
        // 获取歌曲信息
        QString songName = ui->tableWidgetMylove->item(i, 0)->text();
        QString artist = ui->tableWidgetMylove->item(i, 1)->text();
        QString duration = ui->tableWidgetMylove->item(i, 2)->text();
        QString album  = ui->tableWidgetMylove->item(i, 3)->text();
        QString id  = ui->tableWidgetMylove->item(i, 4)->text();


        // 在播放列表中添加歌曲
        int row = ui->tableWidgetPlayList->rowCount();
        ui->tableWidgetPlayList->insertRow(row);
        ui->tableWidgetPlayList->setItem(row, 0, new QTableWidgetItem(songName));
        ui->tableWidgetPlayList->setItem(row, 1, new QTableWidgetItem(artist));
        ui->tableWidgetPlayList->setItem(row, 2, new QTableWidgetItem(duration));
        ui->tableWidgetPlayList->setItem(row, 3, new QTableWidgetItem(album));
        ui->tableWidgetPlayList->setItem(row, 4, new QTableWidgetItem(id));
    }
}

void Widget::on_pushButtonPlayallMycreate_clicked()
{
    // 清空播放列表
    ui->tableWidgetPlayList->setRowCount(0);

    // 获取歌曲数量
    int rowCount = ui->tableWidgetMycreate->rowCount();

    // 将歌曲逐一添加到播放列表中
    for (int i = 0; i < rowCount; ++i) {
        // 获取歌曲信息
        QString songName = ui->tableWidgetMycreate->item(i, 0)->text();
        QString artist = ui->tableWidgetMycreate->item(i, 1)->text();
        QString duration = ui->tableWidgetMycreate->item(i, 2)->text();
        QString album  = ui->tableWidgetMycreate->item(i, 3)->text();
        QString id  = ui->tableWidgetMycreate->item(i, 4)->text();


        // 在播放列表中添加歌曲
        int row = ui->tableWidgetPlayList->rowCount();
        ui->tableWidgetPlayList->insertRow(row);
        ui->tableWidgetPlayList->setItem(row, 0, new QTableWidgetItem(songName));
        ui->tableWidgetPlayList->setItem(row, 1, new QTableWidgetItem(artist));
        ui->tableWidgetPlayList->setItem(row, 2, new QTableWidgetItem(duration));
        ui->tableWidgetPlayList->setItem(row, 3, new QTableWidgetItem(album));
        ui->tableWidgetPlayList->setItem(row, 4, new QTableWidgetItem(id));
    }
}
void Widget::removeSongFromDatabase(const QString &id, const QString &tableName)
{
    QSqlDatabase db = dbManager::getSqlDataBase("Myceate", "QTMusicPlayer.db");
    QSqlQuery query(db);

    // 使用动态生成的表名来构建删除查询
    query.prepare("DELETE FROM " + tableName + " WHERE id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qDebug() << "Error deleting song from database:" << query.lastError();
    }
}


void Widget::MycreatecustomContextMenuRequest(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);

    // QAction *nextplay = menu->addAction(QIcon(":/picture/play1.png"), "下一首播放");
    //connect(nextplay, &QAction::triggered, this, &Widget::addMycreateSongToPlayList);
    QAction *deletesong = menu->addAction(QIcon(":/picture/deletelist.png"), "从歌单移除歌曲");
    connect(deletesong, &QAction::triggered, this, [=](){
        int currentRow = ui->tableWidgetMycreate->currentRow();
        QString id = ui->tableWidgetMycreate->item(currentRow,4)->text();
        QString tableName = "playlist_" + ui->listWidget->currentItem()->text(); //
        removeSongFromDatabase(id, tableName);
        ui->tableWidgetMycreate->removeRow(currentRow);
    });
    menu->addSeparator();

    QAction *Collecttolist = menu->addAction(QIcon(":/picture/addsongtolist.png"), "收藏到歌单");
    QMenu *collectChild = new QMenu(this);
    Collecttolist->setMenu(collectChild);

    QAction *creatnewSheet = collectChild->addAction(QIcon(":/picture/createlist.png"), "创建新歌单");
    connect(creatnewSheet, &QAction::triggered, this, &Widget::on_pushButtonAddmusiclist_clicked);

    // 添加已有歌单选项
    for (int i = 0; i < musicListWidget->count(); ++i)
    {
        QString musicListName = musicListWidget->item(i)->text();
        QAction *existingListAction = collectChild->addAction(musicListName);
        connect(existingListAction, &QAction::triggered, [=](){
            // 在这里实现将歌曲添加到选定的歌单中的逻辑
            QString selectedListName = existingListAction->text();
            // 添加歌曲到选定的歌单中
            // 将当前选中的歌曲添加到选定的歌单中
            QSqlDatabase db = dbManager::getSqlDataBase("Myceate", "QTMusicPlayer.db");
            QString tableName = "playlist_" + musicListName; // 为该歌单创建对应的表名
            QSqlQuery query(db);
            query.exec("CREATE TABLE " + tableName + " (songName TEXT, singer TEXT, album TEXT, duration INTEGER, id INTEGER)");



            // 获取当前选中的歌曲信息
            int currentRow = ui->tableWidgetMycreate->currentRow();
            QString songName = ui->tableWidgetMycreate->item(currentRow, 0)->text();
            QString singer = ui->tableWidgetMycreate->item(currentRow, 1)->text();
            QString album = ui->tableWidgetMycreate->item(currentRow, 2)->text();
            QString duration = ui->tableWidgetMycreate->item(currentRow, 3)->text();
            QString id = ui->tableWidgetMycreate->item(currentRow, 4)->text();
            // 检查是否同一首歌已经存在于选定的歌单中

            QSqlQuery checkQuery(db);
            checkQuery.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id = :id");
            checkQuery.bindValue(":id", id);
            if (checkQuery.exec() && checkQuery.next()) {
                int count = checkQuery.value(0).toInt();
                if (count > 0) {
                    QMessageBox::warning(this, "错误", "同一首歌已经存在于该歌单中");
                    return;
                }
            } else {
                qDebug() << "Check query failed:" << checkQuery.lastError();
                return;
            }
            // 将歌曲信息插入到新创建的表中
            query.prepare("INSERT INTO " + tableName + " (songName, singer, album, duration, id) VALUES (:songName, :singer, :album, :duration, :id)");
            query.bindValue(":songName", songName);
            query.bindValue(":singer", singer);
            query.bindValue(":album", album);
            query.bindValue(":duration", duration);
            query.bindValue(":id", id);
            query.exec();
        });
    }

    QAction *mylove = collectChild->addAction(QIcon(":/picture/Mylove1.png"), "我喜欢的音乐");
    connect(mylove, &QAction::triggered, this, &Widget::on_pushButtonaddMylove_clicked);

    qDebug() << "Right-clicked at position:" << pos;
    menu->exec(QCursor::pos());

}

void Widget::MylovecustomContextMenuRequest(const QPoint &pos)
{
    QMenu *menu = new QMenu(this);

    // QAction *nextplay = menu->addAction(QIcon(":/images/stackWidget/localMusic/btn_play.png"), "下一首播放");
    // connect(nextplay, &QAction::triggered, this, &Widget::addMyloveSongToPlayList);

    QAction *deletesong = menu->addAction(QIcon(":/picture/deletelist.png"), "从歌单移除歌曲");
    connect(deletesong, &QAction::triggered, this, [=](){
        int currentRow = ui->tableWidgetMylove->currentRow();
        QString id = ui->tableWidgetMylove->item(currentRow,4)->text();
        QString tableName = "Mylove";
        removeSongFromDatabase(id, tableName);
        ui->tableWidgetMylove->removeRow(currentRow);
    });
    menu->addSeparator();

    QAction *Collecttolist = menu->addAction(QIcon(":/picture/addsongtolist.png"), "收藏到歌单");
    QMenu *collectChild = new QMenu(this);
    Collecttolist->setMenu(collectChild);

    QAction *creatnewSheet = collectChild->addAction(QIcon(":/picture/createlist.png"), "创建新歌单");
    connect(creatnewSheet, &QAction::triggered, this, &Widget::on_pushButtonAddmusiclist_clicked);

    // 添加已有歌单选项
    for (int i = 0; i < musicListWidget->count(); ++i)
    {
        QString musicListName = musicListWidget->item(i)->text();
        QAction *existingListAction = collectChild->addAction(musicListName);
        connect(existingListAction, &QAction::triggered, [=](){
            // 在这里实现将歌曲添加到选定的歌单中的逻辑
            QString selectedListName = existingListAction->text();
            // 添加歌曲到选定的歌单中
            // 将当前选中的歌曲添加到选定的歌单中

            QSqlDatabase db = dbManager::getSqlDataBase("Myceate", "QTMusicPlayer.db");
            QString tableName = "playlist_" + musicListName; // 为该歌单创建对应的表名
            QSqlQuery query(db);
            query.exec("CREATE TABLE " + tableName + " (songName TEXT, singer TEXT, album TEXT, duration INTEGER, id INTEGER)");



            // 获取当前选中的歌曲信息
            int currentRow = ui->tableWidgetMylove->currentRow();
            QString songName = ui->tableWidgetMylove->item(currentRow, 0)->text();
            QString singer = ui->tableWidgetMylove->item(currentRow, 1)->text();
            QString album = ui->tableWidgetMylove->item(currentRow, 2)->text();
            QString duration = ui->tableWidgetMylove->item(currentRow, 3)->text();
            QString id = ui->tableWidgetMylove->item(currentRow, 4)->text();
            // 检查是否同一首歌已经存在于选定的歌单中

            QSqlQuery checkQuery(db);
            checkQuery.prepare("SELECT COUNT(*) FROM " + tableName + " WHERE id = :id");
            checkQuery.bindValue(":id", id);
            if (checkQuery.exec() && checkQuery.next()) {
                int count = checkQuery.value(0).toInt();
                if (count > 0) {
                    QMessageBox::warning(this, "错误", "同一首歌已经存在于该歌单中");
                    return;
                }
            } else {
                qDebug() << "Check query failed:" << checkQuery.lastError();
                return;
            }

            // 将歌曲信息插入到新创建的表中
            query.prepare("INSERT INTO " + tableName + " (songName, singer, album, duration, id) VALUES (:songName, :singer, :album, :duration, :id)");
            query.bindValue(":songName", songName);
            query.bindValue(":singer", singer);
            query.bindValue(":album", album);
            query.bindValue(":duration", duration);
            query.bindValue(":id", id);
            query.exec();
        });
    }
    qDebug() << "Right-clicked at position:" << pos;
    menu->exec(QCursor::pos());
}


//语音识别

void Widget::speechconnect()
{
    connect(ui->lineEditSpeech, &QLineEdit::textChanged, this, [this]() {
        if(ui->lineEditSpeech->text() == "下一曲。") {
            qDebug()<<"识别到下一曲";
            ui->pushButtonNextsong->click(); // 触发 pushButtonNextsong 的点击事件
        }
        else if(ui->lineEditSpeech->text() == "上一曲。") {
            qDebug()<<"识别到上一曲。";
            ui->pushButtonLastsong->click(); // 触发 pushButtonLastsong 的点击事件
        }
        else if(ui->lineEditSpeech->text() == "播放。")
        {
            int row = ui->tableWidgetPlayList->currentRow();
            playSong(row);
        }        else if(ui->lineEditSpeech->text() == "播放歌曲。")
        {
            playSong(0);
        }
        else if(ui->lineEditSpeech->text() == "暂停。")
        {
            mediaPlayer->pause();
        }
        else if(ui->lineEditSpeech->text() == "单曲循环。")
        {
            on_pushButtonLoop_clicked(true);
        }
        else if(ui->lineEditSpeech->text() == "列表循环。")
        {
            on_pushButtonLoop_clicked(false);
        }
        else if(ui->lineEditSpeech->text() == "歌词。")
        {
            showHideLyric(ui->pushButtonLyric->isChecked());
        }
        else if(ui->lineEditSpeech->text() == "添加我喜欢。")
        {
            ui->pushButtonaddMylove->click();
        }
        else if(ui->lineEditSpeech->text() == "上一页。")
        {
            ui->pushButtonLastpage->click();
        }
        else if(ui->lineEditSpeech->text() == "下一页。")
        {
            ui->pushButtonNextpage->click();
        }


    });
    connect(ui->lineEditSpeech, &QLineEdit::textChanged, this, [this]() {
        QString searchText = ui->lineEditSpeech->text();
        if(searchText.startsWith("搜索")) {
            QString keyword = searchText.mid(2); // 获取"搜索"后面的关键词
            ui->lineEdit->setText(keyword); // 在 ui->lineEdit 中显示关键词
            on_pushButtonSearch_clicked(); // 触发搜索按钮的点击事件
        }
    });

}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {
        ui->pushButtonStart->setText("松开识别");
        if(event->isAutoRepeat())
            return;
        audio->startAudio("audiofile");
    }

}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Space)
    {
        ui->pushButtonStart->setText("按下识别");
        if(event->isAutoRepeat())
            return;
        audio->stopAudio();
        //启动识别
        //QString retStr = audio->startSpeech();
        QString retStr = audio->startSpeech().trimmed();
        myspeech->say(retStr); ui->lineEditSpeech->setText(retStr);
        qDebug() << "retStr: " << retStr;
        // 启动定时器，在一秒后清除文本内容
        clearTimer->start(1000);
    }

}



