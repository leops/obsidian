#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <QCoreApplication>
#include <QScriptValue>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QScriptValueIterator>
#include <QVariant>
#include <QDir>
#include <QDebug>


QDir getDir(QString name);
QJsonValue toJSON(const QScriptValue& var);
QString toString(const QScriptValue& var);
QScriptValue toScript(const QVariant& var);

#endif // GLOBAL_HPP

