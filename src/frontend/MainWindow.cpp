#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "localai_c_api.h"
#include <qmessagebox.h>

constexpr const char* llm_model_path = "D:\\ai-projects\\LocalAI-Assistant\\models\\Qwen3-0.6B-Q8_0.gguf";
constexpr const char* asr_model_path = "D:\\ai-projects\\LocalAI-Assistant\\models\\ggml-small.en.bin";

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LocalAI_Config default_config = {};
	auto result = LocalAI_Core_Initialize(default_config);
    if (result) {
		std::string msg = std::string(LocalAI_StatusGetMessage(result));
        LocalAI_StatusDestroy(result);
		throw std::runtime_error("Failed to initialize LocalAI core: " + msg);
    }

	Model_Params model_params = {0};
    model_params.n_gpu_layers = 99;
    auto status = LocalAI_Core_InitializeModel(LOCALAI_MODEL_TEXT_GEN,
        llm_model_path, model_params);
    if (status) {
        std::string msg = std::string(LocalAI_StatusGetMessage(status));
        LocalAI_StatusDestroy(status);
        throw std::runtime_error("Failed to initialize LocalAI core: " + msg);
    }

    auto r = LocalAI_Core_CreateChatSession(&session_);
    if (r) {
        LocalAI_StatusDestroy(r);
    }
}

MainWindow::~MainWindow()
{
    if (session_) {
        LocalAI_Core_ReleaseChatSession(session_);
        session_ = nullptr;
    }

    if (asr_model_) {
        LocalAI_ASR_Model_Release(asr_model_);
    }

    LocalAI_Core_Shutdown();

    delete ui;
}

size_t MainWindow::asrFillBuffer(float* buffer, int buffer_size, int ms)
{
    if (!audio_capture_)
        return 0;

    size_t sample = audio_capture_->getAudioData(buffer, buffer_size, ms);
    if (0 == sample) {
        fprintf(stderr, "No audio data retrieved.");
        return 0;
    }

    return sample;
}

void MainWindow::asrClearAudio()
{
    if (audio_capture_) {
        audio_capture_->clearBuffer();
    }
}

void MainWindow::asrOnTranscribe(const char* text, int lens)
{
    if (!text)
        return;

    QMetaObject::invokeMethod(this, &MainWindow::onSubtitle, Qt::QueuedConnection,
        QString(text).append("\n"));
}

void MainWindow::asrStatusChanged(ASRStatus status)
{
}

void MainWindow::asrError(LocalAI_ErrorCode code, const char* msg)
{
}

void MainWindow::onResponse(const QString& text)
{
    if (current_request_) {
        LocalAI_Request_Release(current_request_);
        current_request_ = nullptr;
    }
    ui->textBrowser->append(text);
}

void MainWindow::onSubtitle(const QString& text)
{
    ui->label_subtitle->setText(text);
}

void MainWindow::on_btn_send_txt_clicked()
{
    if (current_request_)
        return;

	auto text_input = ui->textEdit_input->toPlainText();

    auto callback = [](
        LocalAI_Request* request,
        const LocalAI_TextResult* result,
        LocalAI_Status* status,
        void* user_data) {

			auto window = static_cast<MainWindow*>(user_data);
            if (status) {
                std::string msg = std::string(LocalAI_StatusGetMessage(status));
				LocalAI_StatusDestroy(status);
                QMessageBox::critical(window, "Error", 
                    QString("Error occurred during generating\nError message: %1").arg(msg.c_str()));
            }
			else {
				qDebug() << "Generated text:" << result->text;
				QMetaObject::invokeMethod(window, &MainWindow::onResponse, Qt::QueuedConnection,
					QString(result->text).append("\n"));
			}

            LocalAI_TextResult_Free(result);
        };

	if (!text_input.isEmpty()) {
		auto result = LocalAI_GenerateAsync(session_,
			text_input.toStdString().c_str(),
			nullptr,
			callback, this, &current_request_);
		if (result) {
			LocalAI_StatusDestroy(result);
		}
	}
}

void MainWindow::on_btn_speech_clicked()
{
    if (!audio_capture_) {
        auto audio_capture = Capture::audio::createAudioCapture();
        int buffer_Size = 0;
        if (!audio_capture->initialize(16000, 0, buffer_Size, nullptr)) {
            assert(false);
            qDebug() << "Failed to initialize audio capture.";
            return;
        }

        LocalAI_AudioProviderInfo audio_provider{0};
        audio_provider.sample_rate = audio_capture->getAudioInfo().sample_rate;
        audio_provider.user_data = this;
        audio_provider.circle_buffer_size = buffer_Size;
        audio_provider.fill_buffer = [](float* buffer, int buffer_size, int ms, void* user_data) -> size_t {
            auto window = static_cast<MainWindow*>(user_data);
            if (window) {
                return window->asrFillBuffer(buffer, buffer_size, ms);
            }

            return 0;
        };
        audio_provider.clear_audio = [](void* user_data) {
            auto window = static_cast<MainWindow*>(user_data);
            if (window) {
                window->asrClearAudio();
            }
        };
        audio_provider.on_heard = [](const char* text, int len, void* user_data) {
            auto window = static_cast<MainWindow*>(user_data);
            if (window) {
                window->asrOnTranscribe(text, len);
            }
        };
        audio_provider.on_status_changed = [](ASRStatus status, void * user_data) {
            auto window = static_cast<MainWindow*>(user_data);
            if (window) {
                window->asrStatusChanged(status);
            }
        };
        audio_provider.on_error = [](LocalAI_ErrorCode code, const char* error, void* user_data) {
            auto window = static_cast<MainWindow*>(user_data);
            if (window) {
                window->asrError(code, error);
            }
        };

        Model_Params param{};
        param.n_threads = 1;

        auto status = LocalAI_ASR_Model_Create(asr_model_path, param, &asr_model_);
        if (status) {
            qDebug() << "Failed to create asr model.";
            assert(false);
            LocalAI_StatusDestroy(status);
            return;
        }

        auto result = LocalAI_ChatEnableASR(session_, &audio_provider, asr_model_);
        if (result) {
            qDebug() << "Failed to enable asr for session.";
            assert(false);
            LocalAI_StatusDestroy(status);
            LocalAI_ASR_Model_Release(asr_model_);
            return;
        }

        audio_capture_ = std::move(audio_capture);
    }

    audio_capture_->resume();
}
