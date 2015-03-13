#include "model.hpp"

SQLModel::SQLModel(const QString& name, const QJsonObject& data, QObject *parent) : Model(parent) {
	setObjectName(name);
	auto model = data.toVariantMap();
	QStringList columns = {"id PRIMARY KEY"};

	auto attr = model.constBegin();
	while (attr != model.constEnd()) {
		auto info = attr.value().toMap();
		auto name = attr.key();
		auto type = info["type"].toString();
		m_attributes[name] = type;

		QString sqlType;
		if(type == "string") {
			sqlType = "TEXT";
		} else if(type == "integer") {
			sqlType = "INTEGER";
		}

		if(info.find("required") != info.end() && info["required"].toBool())
			sqlType += " NOT NULL";

		columns.append(name + " " + sqlType);

		++attr;
	}

	QSqlQuery query;
	auto create = QString("CREATE TABLE IF NOT EXISTS %1 (%2);").arg(name).arg(columns.join(", "));
	query.exec(create);
}

int SQLModel::create(const QVariantHash& values) {
	QSqlQuery query;
	QStringList columns;

	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		columns.append(i.key());
	}

	query.prepare(QString("INSERT INTO %1 (%2) VALUES (%3);").arg(objectName()).arg(columns.join(", ")).arg(columns.join(", :").prepend(":")));

	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		query.bindValue(":" + i.key(), i.value());
	}

	query.exec();
	return query.lastInsertId().toInt();
}

QList<QVariantHash> SQLModel::find(const QVariantHash& selector) {
	QSqlQuery find;
	QString cond;

	if(selector.size() > 0) {
		QStringList val;
		for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
			val.append(QString("%1 = :%1").arg(i.key()));
		}
		cond = "WHERE " + val.join(" AND ");
	}

	find.prepare(QString("SELECT * FROM %1 %2;").arg(objectName()).arg(cond));

	if(selector.size() > 0) {
		for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
			find.bindValue(":" + i.key(), i.value());
		}
	}

	find.exec();

	QList<QVariantHash> result;
	while (find.next()) {
		QVariantHash row;

		for(auto i = m_attributes.constBegin(); i != m_attributes.constEnd(); ++i) {
			row[i.key()] = find.value(i.key());
		}

		result.append(row);
	}

	return result;
}

bool SQLModel::update(const QVariantHash& selector, const QVariantHash& values) {
	QSqlQuery query;
	QStringList columns, conds;

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		columns.append(QString("%1 = :%1").arg(i.key()));
	}

	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		conds.append(QString("%1 = :%1").arg(i.key()));
	}

	query.prepare(QString("UPDATE %1 SET %2 WHERE %3;").arg(objectName()).arg(columns.join(" ")).arg(conds.join(" AND ")));

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		query.bindValue(":" + i.key(), i.value());
	}

	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		query.bindValue(":" + i.key(), i.value());
	}

	return query.exec();
}

bool SQLModel::destroy(const QVariantHash& selector) {
	QSqlQuery del;
	QStringList cond;

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		cond.append(QString("%1 = :%1").arg(i.key()));
	}

	del.prepare(QString("DELETE FROM %1 WHERE %2;").arg(objectName()).arg(cond.join(" AND ")));

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		del.bindValue(":" + i.key(), i.value());
	}

	return del.exec();
}

SQLManager::SQLManager(QObject* parent) : ModelManager(parent) {
	QFile cfg(getDir("config").absoluteFilePath("database.json"));
	cfg.open(QIODevice::ReadOnly | QIODevice::Text);
	auto config = QJsonDocument::fromJson(cfg.readAll()).object();
	cfg.close();

	auto type = config["type"].toString().toLower();
	if(type == "sqlite") {
		type = "QSQLITE";
	} else if(type == "mysql") {
		type = "QMYSQL";
	} else if(type == "postgres") {
		type = "QPSQL";
	}

	QSqlDatabase db = QSqlDatabase::addDatabase(type);

	if(config.contains("name"))
		db.setDatabaseName(config["name"].toString());
	if(config.contains("host"))
		db.setHostName(config["host"].toString());
	if(config.contains("port"))
		db.setPort(config["port"].toInt());
	if(config.contains("userName"))
		db.setUserName(config["userName"].toString());
	if(config.contains("password"))
		db.setPassword(config["password"].toString());

	if(!db.open())
		qWarning() << "Could not open database";

	auto mdlDir = getDir("models");
	foreach (QString fileName, mdlDir.entryList(QDir::Files)) {
		QFile file(mdlDir.absoluteFilePath(fileName));
		file.open(QIODevice::ReadOnly | QIODevice::Text);

		auto name = fileName.split(".").first();
		qDebug().noquote() << "Loading model" << name;

		QJsonParseError err;
		auto model = QJsonDocument::fromJson(file.readAll(), &err).object();
		if(err.error != QJsonParseError::NoError)
			qWarning().noquote() << err.errorString();
		m_models[name] = createModel(name, model);

		file.close();
	}
}

Model* SQLManager::createModel(const QString &name, const QJsonObject &data) {
	return new SQLModel(name, data, this);
}
