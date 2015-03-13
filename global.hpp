#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <QCoreApplication>
#include <QScriptValue>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <QScriptEngine>
#include <QScriptValueIterator>
#include <QVariant>
#include <QDir>
#include <QDebug>


QDir getDir(QString name);
QJsonValue toJSON(const QScriptValue& var);
QString toString(const QScriptValue& var);
QScriptValue toScriptValue(QScriptEngine* engine, const QVariant& var);
QScriptValue toScriptValue(QScriptEngine* engine, const QVariantHash& hash);
QScriptValue toScriptValue(QScriptEngine* engine, const QVariantList& list);
QScriptValue toScriptValue(QScriptEngine* engine, const QList<QVariantHash>& list);
void fromScriptValue(const QScriptValue& obj, QVariantHash& hash);
void fromScriptValue(const QScriptValue& obj, QVariantList& list);
void fromScriptValue(const QScriptValue& obj, QList<QVariantHash>& list);

#endif // GLOBAL_HPP

