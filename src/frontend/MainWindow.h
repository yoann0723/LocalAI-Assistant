#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "localai_c_api.h"
#include "IAudioCapture.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    bool asrFillBuffer(float* buffer, int buffer_size, int ms);
    void asrOnTranscribe(const char *text, int lens);
    void asrStatusChanged(ASRStatus status);
    void asrError(LocalAI_ErrorCode code, const char *msg);

public slots:
    void onResponse(const QString& text);
    void onSubtitle(const QString& text);
	void on_btn_send_txt_clicked();
	void on_btn_speech_clicked();

private:
    Ui::MainWindow *ui;
    LocalAI_ChatSession* session_ = nullptr;
    LocalAI_Request* current_request_ = nullptr;
    LocalAI_ASR_Model* asr_model_ = nullptr;
    std::unique_ptr<Capture::audio::IAudioCapture> audio_capture_;
};

#endif // MAINWINDOW_H
