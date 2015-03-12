#ifndef MODEL_HPP
#define MODEL_HPP

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QDebug>
#include <QScriptEngine>
#include <QScriptContext>
#include <QScriptValue>
#include <QSqlQuery>
#include <QScriptValueIterator>
#include <functional>
#include "global.hpp"

class Model : public QObject
{
	Q_OBJECT

public:
	explicit Model(QObject *parent = nullptr);

	QList<QString> modelList() const {
		return m_modelList;
	}

	typedef QHash<QString, QVariant> Row;

	virtual int create(const QString&, const Row&);
	virtual QList<Row> find(const QString&, const Row& = {});
	virtual bool update(const QString&, const Row&, const Row&);
	virtual bool destroy(const QString&, const Row&);

private:
	QHash<QString, QHash<QString, QString>> m_models;
	QList<QString> m_modelList;

};

#endif // MODEL_HPP
