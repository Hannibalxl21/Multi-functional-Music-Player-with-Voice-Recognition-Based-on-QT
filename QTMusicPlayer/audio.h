#ifndef AUDIO_H
#define AUDIO_H
#include <QMediaDevices>
#include <QObject>
#include <QAudioDevice> // Qt 6中用于查询音频设备
#include <QAudioInput> // Qt 6中用于音频输入
#include <QAudioSource>
#include <QFile>
#include <speech.h> // 假设这是您自己定义的语音识别类
#include <QMessageBox>
#include <QDebug>

class Audio : public QObject
{
    Q_OBJECT
public:
    explicit Audio(QObject *parent = nullptr); // 构造函数

public slots:
    void startAudio(QString fileName); // 开始录音
    void stopAudio(); // 停止录音

public:
    QString startSpeech(); // 开始语音识别

private:
    QString AudioFileName; // 用于存储音频文件名
    QFile *AudioFile; // 用于操作音频文件
    QAudioDevice *AudioDevice; // Qt 6中的音频设备对象
    QAudioSource *AudioSource; // Qt 6中的音频输入对象
    Speech *speech; // 语音识别对象
};

#endif // AUDIO_H
