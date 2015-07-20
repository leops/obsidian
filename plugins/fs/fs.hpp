#ifndef FS_HPP
#define FS_HPP

#include <model.hpp>
#include <QDir>

class FSModel : public QObject {
	Q_OBJECT

	public:
		explicit FSModel(const QString& name, const QJsonObject& data, QObject* parent = nullptr);

	public Q_SLOTS:
		QStringList list();
		QByteArray read(QString name);
		bool write(QString name, QByteArray data);
		bool remove(QString name);

	private:
		QDir m_path;
};

class FSManager : public QObject, public ModelManager {
	Q_OBJECT
	Q_INTERFACES(ModelManager)
	Q_PLUGIN_METADATA(IID "com.obsidian.ModelManager" FILE "fs.json")

	public:
		explicit FSManager(QObject *parent = nullptr);
		QObject* createModel(const QString& name, const QJsonObject& data);
};

#endif // FS_HPP

