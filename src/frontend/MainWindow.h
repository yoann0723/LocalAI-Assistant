#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "localai_c_api.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void onResponse(const QString& text);

public slots:
	void on_btn_send_txt_clicked();

private:
    Ui::MainWindow *ui;
    LocalAI_ChatSession* session_ = nullptr;
    LocalAI_Request* current_request_ = nullptr;
};

#endif // MAINWINDOW_H
