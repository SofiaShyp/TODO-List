#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "TaskManager.h"

int main(int argc, char *argv[]) {
    qputenv("QT_QUICK_CONTROLS_STYLE", "Basic");

    QGuiApplication app(argc, argv);

    TaskManager taskManager;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("taskManager", &taskManager);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}