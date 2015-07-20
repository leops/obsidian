#include <fs.hpp>
#include <obsidian.hpp>

FSModel::FSModel(const QString& name, const QJsonObject& data, QObject *parent) : QObject(parent) {
	setObjectName(name);
	m_path = Obsidian::getDir(data.value("path").toString());
}

QStringList FSModel::list() {
	return m_path.entryList(QDir::Files);
}

QByteArray FSModel::read(QString name) {
	QFile file(m_path.absoluteFilePath(name));
	file.open(QIODevice::ReadOnly);

	return file.readAll();
}

bool FSModel::write(QString name, QByteArray data) {
	QFile file(m_path.absoluteFilePath(name));
	file.open(QIODevice::WriteOnly);

	if(file.write(data) == -1) {
		qWarning() << "Could not write" << name;
		return false;
	}

	return true;
}

bool FSModel::remove(QString name) {
	QFile file(m_path.absoluteFilePath(name));
	if(!file.remove()) {
		qWarning() << "Could not delete" << name;
		return false;
	}
	return true;
}

FSManager::FSManager(QObject* parent) : QObject(parent) {
	auto mdlDir = Obsidian::getDir("models");
	foreach (QString fileName, mdlDir.entryList(QDir::Files)) {
		QFile file(mdlDir.absoluteFilePath(fileName));
		file.open(QIODevice::ReadOnly | QIODevice::Text);

		auto name = fileName.split(".").first();
		qDebug().noquote() << "Loading model" << name;

		QJsonParseError err;
		auto model = QJsonDocument::fromJson(file.readAll(), &err).object();
		if(err.error != QJsonParseError::NoError)
			qWarning().noquote() << err.errorString();

		if(model.value("type").toString() == "fs")
			m_models[name] = createModel(name, model);

		file.close();
	}
}

QObject* FSManager::createModel(const QString &name, const QJsonObject &data) {
	return new FSModel(name, data, this);
}
