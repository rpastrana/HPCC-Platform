#ifndef CONFIGURATORUI_HPP
#define CONFIGURATORUI_HPP

#include <QObject>
#include <QtQuick/QQuickView>
#include <QGuiApplication>
#include <QDateTime>

/*class ConfiguratorUI : public QObject
{
    Q_OBJECT
public:
    //explicit ConfiguratorUI(QObject *parent = 0);
    Q_INVOKABLE QDateTime getCurrentDateTime() const {
         return QDateTime::currentDateTime();
     }

signals:

public slots:

};*/



class ApplicationData : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QDateTime getCurrentDateTime() const {
        return QDateTime::currentDateTime();
    }
};

#endif // CONFIGURATORUI_HPP
#ifndef CONFIGURATORUI_HPP
#define CONFIGURATORUI_HPP

#include <QObject>
#include <QtQuick/QQuickView>
#include <QGuiApplication>
#include <QDateTime>

/*class ConfiguratorUI : public QObject
{
    Q_OBJECT
public:
    //explicit ConfiguratorUI(QObject *parent = 0);
    Q_INVOKABLE QDateTime getCurrentDateTime() const {
         return QDateTime::currentDateTime();
     }

signals:

public slots:

};*/



class ApplicationData : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QDateTime getCurrentDateTime() const {
        return QDateTime::currentDateTime();
    }
};

#endif // CONFIGURATORUI_HPP
#ifndef CONFIGURATORUI_HPP
#define CONFIGURATORUI_HPP

#include <QObject>
#include <QtQuick/QQuickView>
#include <QGuiApplication>
#include <QDateTime>

/*class ConfiguratorUI : public QObject
{
    Q_OBJECT
public:
    //explicit ConfiguratorUI(QObject *parent = 0);
    Q_INVOKABLE QDateTime getCurrentDateTime() const {
         return QDateTime::currentDateTime();
     }

signals:

public slots:

};*/



class ApplicationData : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE QDateTime getCurrentDateTime() const {
        return QDateTime::currentDateTime();
    }
};

#endif // CONFIGURATORUI_HPP
