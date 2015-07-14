#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QVariant>
#include <QDir>
#include <QDebug>


QDir getDir(QString name);
QJsonObject getConfig(QString name);

#endif // GLOBAL_HPP

