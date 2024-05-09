#include "audio.h"

Audio::Audio(QObject *parent) : QObject(parent)
{
    speech = new Speech(this); // 初始化语音识别对象
}

void Audio::startAudio(QString fileName)
{
    if (fileName.isEmpty()) {
        QMessageBox::warning(nullptr, "警告(Audio)", "未提供音频文件名");
        return;
    }

    // 获取默认的音频输入设备
    QAudioDevice device = QMediaDevices::defaultAudioInput();
    if (device.isNull()) {
        QMessageBox::warning(nullptr, "警告(Audio)", "没有可用的音频设备");
        return;
    }

    AudioFileName = fileName; // 设置音频文件名

    // 设置音频格式
    QAudioFormat format;
    format.setSampleRate(16000); // 设置采样频率
    format.setChannelCount(1); // 设置通道数
    format.setSampleFormat(QAudioFormat::Int16); // 设置采样点格式,很关键这一步
    // format.setSampleSize(16); // 设置位深度
    //format.setCodec("audio/pcm"); // 设置编码格式

    //检查设备是否支持该格式
    // if (!device.isFormatSupported(format)) {
    //     format = device.nearestFormat(format); // 寻找最接近的格式
    //     qDebug()<<"不支持";
    // }

    // 打开文件
    AudioFile = new QFile;
    AudioFile->setFileName(fileName);
    AudioFile->open(QIODevice::WriteOnly);

    // 创建音频输入对象
    AudioSource = new QAudioSource(device, format, this);
    connect(AudioSource, &QAudioSource::stateChanged, this, [this](QAudio::State state) {
        if (state == QAudio::StoppedState) {
            AudioFile->close(); // 当录音停止时关闭文件
        }
    });


    // 开始录音
    AudioSource->start(AudioFile);
}

QString Audio::startSpeech()
{
    if (AudioFileName.isEmpty()) {
        QMessageBox::warning(nullptr, "警告", "文件不存在");
        return QString();
    }
    return speech->speechIdentify(AudioFileName); // 调用语音识别并返回结果
}

void Audio::stopAudio()
{
    if (AudioSource) {
        AudioSource->stop(); // 停止录音
        AudioFile->close(); // 关闭文件

        delete AudioFile; // 删除文件操作对象
        AudioFile = nullptr; // 将指针置空

    }
}
