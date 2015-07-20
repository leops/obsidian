#ifndef MODEL_HPP
#define MODEL_HPP

#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVariant>
#include <QDebug>
#include <functional>

class Model {
	public:
		virtual QVariantHash findOne(const QVariantHash& selector = QVariantHash(), const QStringList& projection = QStringList()) = 0;
		virtual QVariantList find(const QVariantHash& selector = QVariantHash(), const QStringList& projection = QStringList()) = 0;
		virtual bool update(const QVariantHash& selector, const QVariantHash& update) = 0;
		virtual int create(const QVariantHash& object) = 0;
		virtual int create(const QVariantList& objects) = 0;
		virtual bool destroy(const QVariantHash& selector, int limit = 0) = 0;
		static int VariantHashList;
};

class ModelManager {
	public:
		virtual QObject* createModel(const QString& name, const QJsonObject& data) = 0;

		QStringList models() const {
			return m_models.keys();
		}

		QObject* models(const QString& name) const {
			return m_models[name];
		}

	protected:
		QHash<QString, QObject*> m_models;
};

Q_DECLARE_INTERFACE(ModelManager, "com.obsidian.ModelManager")

#endif // MODEL_HPP
