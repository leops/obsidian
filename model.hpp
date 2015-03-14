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

Q_DECLARE_METATYPE(QList<QVariantHash>)
Q_DECLARE_METATYPE(QVariantHash)
Q_DECLARE_METATYPE(QVariantList)

class Model : public QObject {
	Q_OBJECT

public:
	explicit Model(QObject *parent = nullptr) : QObject(parent) {}

public slots:
	virtual int create(const QVariantHash&) = 0;
	virtual QList<QVariantHash> find(const QVariantHash& = QVariantHash()) = 0;
	virtual bool update(const QVariantHash&, const QVariantHash&) = 0;
	virtual bool destroy(const QVariantHash&) = 0;
};

Q_DECLARE_INTERFACE(Model, "com.obsidian.Model")

class ModelManager : public QObject {
	Q_OBJECT

public:
	explicit ModelManager(QObject *parent = nullptr) : QObject(parent) {}
	virtual Model* createModel(const QString& name, const QJsonObject& data) = 0;

	QStringList models() const {
		return m_models.keys();
	}

	Model* models(const QString& name) const {
		return m_models[name];
	}

protected:
	QHash<QString, Model*> m_models;
};

Q_DECLARE_INTERFACE(ModelManager, "com.obsidian.ModelManager")

class SQLModel : public Model {
	Q_OBJECT
	Q_INTERFACES(Model)

public:
	explicit SQLModel(const QString& name, const QJsonObject& data, QObject *parent = nullptr);

public slots:
	virtual int create(const QVariantHash&);
	virtual QList<QVariantHash> find(const QVariantHash& = QVariantHash());
	virtual bool update(const QVariantHash&, const QVariantHash&);
	virtual bool destroy(const QVariantHash&);

protected:
	QHash<QString, QString> m_attributes;
};

class SQLManager : public ModelManager {
	Q_OBJECT
	Q_INTERFACES(ModelManager)

public:
	explicit SQLManager(QObject *parent = nullptr);
	virtual Model* createModel(const QString& name, const QJsonObject& data);
};

#endif // MODEL_HPP
