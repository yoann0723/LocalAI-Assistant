#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "InferenceEngine.h"

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QMetaObject::connectSlotsByName(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_send_clicked()
{
}
