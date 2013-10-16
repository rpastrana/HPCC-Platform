#include <QtQuick/QQuickView>
#include <QGuiApplication>
#include "configuratorui.hpp"
#include <QQmlContext>


/*ConfiguratorUI::ConfiguratorUI(QObject *parent) :
    QObject(parent)
{
}*/


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickView view;

    ApplicationData data;

    //qmlRegisterType<ApplicationData>("");
    view.rootContext()->setContextProperty("ApplicationData", &data);
    //view.rootContext()->set
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl::fromLocalFile("MyItem.qml"));
    view.show();
    return app.exec();
}

extern "C" void StartQMLUI()
{
    main(0,NULL);
}
#include <QtQuick/QQuickView>
#include <QGuiApplication>
#include "configuratorui.hpp"
#include <QQmlContext>


/*ConfiguratorUI::ConfiguratorUI(QObject *parent) :
    QObject(parent)
{
}*/


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickView view;

    ApplicationData data;

    //qmlRegisterType<ApplicationData>("");
    view.rootContext()->setContextProperty("ApplicationData", &data);
    //view.rootContext()->set
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl::fromLocalFile("MyItem.qml"));
    view.show();
    return app.exec();
}

extern "C" void StartQMLUI()
{
    main(0,NULL);
}
#include <QtQuick/QQuickView>
#include <QGuiApplication>
#include "configuratorui.hpp"
#include <QQmlContext>


/*ConfiguratorUI::ConfiguratorUI(QObject *parent) :
    QObject(parent)
{
}*/


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickView view;

    ApplicationData data;

    //qmlRegisterType<ApplicationData>("");
    view.rootContext()->setContextProperty("ApplicationData", &data);
    //view.rootContext()->set
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setSource(QUrl::fromLocalFile("MyItem.qml"));
    view.show();
    return app.exec();
}

extern "C" void StartQMLUI()
{
    main(0,NULL);
}
