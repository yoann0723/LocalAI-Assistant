#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QThread>
#include "IAudioCapture.h"
#include "AIModelHub.h"

void initializeModelHub()
{
	AIModelHub::instance().initialize();
}

int main(int argc, char* argv[])
{
    initializeModelHub();
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/Frontend/qml/Main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}