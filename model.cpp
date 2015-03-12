#include "model.hpp"

Model::Model(QObject* parent) : QObject(parent) {
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
		qDebug() << "Loading model" << name;

		QJsonParseError err;
		auto model = QJsonDocument::fromJson(file.readAll(), &err).object().toVariantMap();
		qWarning() << err.errorString();

		QHash<QString, QString> queries;
		QStringList columns = {"id PRIMARY KEY"};

		auto attr = model.constBegin();
		while (attr != model.constEnd()) {
			auto info = attr.value().toMap();
			auto name = attr.key();
			auto type = info["type"].toString();
			queries[name] = type;

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

		m_models[name] = queries;
		m_modelList.append(name);

		QSqlQuery query;
		auto create = QString("CREATE TABLE IF NOT EXISTS %1 (%2);").arg(name).arg(columns.join(", "));
		qDebug() << create << query.exec(create);

		file.close();
	}
}

int Model::create(const QString& model, const Row& values) {
	QSqlQuery query;
	QStringList columns;

	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		columns.append(i.key());
	}

	query.prepare(QString("INSERT INTO %1 (%2) VALUES (%3);").arg(model).arg(columns.join(", ")).arg(columns.join(", :").prepend(":")));

	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		query.bindValue(":" + i.key(), i.value());
	}

	query.exec();
	return query.lastInsertId().toInt();
}

QList<Model::Row> Model::find(const QString& model, const Row& selector) {
	QSqlQuery find;
	QString cond;

	if(selector.size() > 0) {
		QStringList val;
		for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
			val.append(QString("%1 = :%1").arg(i.key()));
		}
		cond = "WHERE " + val.join(" AND ");
	}

	find.prepare(QString("SELECT * FROM %1 %2;").arg(model).arg(cond));

	if(selector.size() > 0) {
		for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
			find.bindValue(":" + i.key(), i.value());
		}
	}

	find.exec();

	QList<Row> result;
	while (find.next()) {
		Row row;

		for(auto i = m_models[model].constBegin(); i != m_models[model].constEnd(); ++i) {
			row[i.key()] = find.value(i.key());
		}

		result.append(row);
	}

	return result;
}

bool Model::update(const QString& model, const Row& selector, const Row& values) {
	QSqlQuery query;
	QStringList columns, conds;

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		columns.append(QString("%1 = :%1").arg(i.key()));
	}

	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		conds.append(QString("%1 = :%1").arg(i.key()));
	}

	query.prepare(QString("UPDATE %1 SET %2 WHERE %3;").arg(model).arg(columns.join(" ")).arg(conds.join(" AND ")));

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		query.bindValue(":" + i.key(), i.value());
	}

	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		query.bindValue(":" + i.key(), i.value());
	}

	return query.exec();
}

bool Model::destroy(const QString& model, const Row& selector) {
	QSqlQuery del;
	QStringList cond;

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		cond.append(QString("%1 = :%1").arg(i.key()));
	}

	del.prepare(QString("DELETE FROM %1 WHERE %2;").arg(model).arg(cond.join(" AND ")));

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		del.bindValue(":" + i.key(), i.value());
	}

	return del.exec();
}
