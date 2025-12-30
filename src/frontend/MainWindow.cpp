#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "localai_c_api.h"
#include <qmessagebox.h>

constexpr const char* llm_model_path = "D:\\ai-projects\\LocalAI-Assistant\\models\\Qwen3-0.6B-Q8_0.gguf";

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMetaObject::connectSlotsByName(this);

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

    auto r = LocalAI_Core_CreateSession(&session_);
    if (r) {
        LocalAI_StatusDestroy(r);
    }
}

MainWindow::~MainWindow()
{
    if (session_) {
        LocalAI_Core_ReleaseSession(session_);
        session_ = nullptr;
    }

    LocalAI_Core_Shutdown();

    delete ui;
}

void MainWindow::onResponse(const QString& text)
{
    if (current_request_) {
        LocalAI_Request_Release(current_request_);
        current_request_ = nullptr;
    }
    ui->textBrowser->append(text);
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
                window->onResponse(QString(result->text) + "\n");
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
