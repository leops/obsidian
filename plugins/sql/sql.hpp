#ifndef PHP_HPP
#define PHP_HPP

#include <model.hpp>

class SQLModel : public QObject, public Model {
	Q_OBJECT

	public:
		explicit SQLModel(const QString& name, const QJsonObject& data, QObject* parent = nullptr);

	public Q_SLOTS:
		QVariantHash findOne(const QVariantHash& selector = QVariantHash(), const QStringList& projection = QStringList());
		QVariantList find(const QVariantHash& selector = QVariantHash(), const QStringList& projection = QStringList());
		bool update(const QVariantHash& selector, const QVariantHash& update);
		int create(const QVariantHash& object);
		int create(const QVariantList& objects);
		bool destroy(const QVariantHash& selector, int limit = 0);

	protected:
		QString parseSelector(const QVariantHash& selector);

	private:
		QHash<QString, QString> m_attributes;
};

class SQLManager : public QObject, public ModelManager {
	Q_OBJECT
	Q_INTERFACES(ModelManager)
	Q_PLUGIN_METADATA(IID "com.obsidian.ModelManager" FILE "sql.json")

	public:
		explicit SQLManager(QObject *parent = nullptr);
		Model* createModel(const QString& name, const QJsonObject& data);
};

#endif // PHP_HPP

