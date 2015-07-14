#include <sql.hpp>
#include <global.hpp>
#include <QSqlQuery>

SQLModel::SQLModel(const QString& name, const QJsonObject& data, QObject *parent) : QObject(parent) {
	setObjectName(name);
	auto model = data.toVariantMap();
	QStringList columns = {};

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
		} else if (type == "primary") {
			sqlType = "INTEGER PRIMARY KEY";
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

int SQLModel::create(const QVariantHash& object) {
	return create(QVariantList{object});
}

int SQLModel::create(const QVariantList& objects) {
	QSqlQuery query;

	QStringList rows, fields;
	for(auto i = objects.constBegin(); i != objects.constEnd(); ++i) {
		fields.clear();
		auto hash = i->toHash();
		for(auto j = hash.constBegin(); j != hash.constEnd(); ++j) {
			fields.append(j.key());
		}
		rows.append(fields.join(", :").prepend(':'));
	}

	query.prepare(QString("INSERT INTO %1 (%2) VALUES (%3);").arg(objectName()).arg(fields.join(", ")).arg(rows.join("), (")));

	/*for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		query.bindValue(":" + i.key(), i.value());
	}*/

	query.exec();
	return query.lastInsertId().toInt();
}

QString SQLModel::parseSelector(const QVariantHash& selector) {
	QStringList values;
	auto keys = selector.keys();
	std::transform(keys.constBegin(), keys.constEnd(), std::back_inserter(values), [=](auto key) {
		return key + " = :" + key;
	});
	return values.join(" AND ");
}

QVariantHash SQLModel::findOne(const QVariantHash& selector, const QStringList& projection) {
	return find(selector, projection).first().toHash(); // TODO: Limit 1
}

QVariantList SQLModel::find(const QVariantHash& selector, const QStringList& projection) {
	QSqlQuery find;
	QString cond;

	QStringList fields{"*"};
	if(projection.size() > 0) {
		fields = projection;
		fields.prepend("id");
	}

	if(selector.size() > 0)
		cond = "WHERE " + parseSelector(selector);

	auto req = QString("SELECT %3 FROM %1 %2;").arg(objectName()).arg(cond).arg(fields.join(", "));
	qDebug() << req;
	find.prepare(req);

	if(selector.size() > 0) {
		for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
			find.bindValue(":" + i.key(), i.value());
		}
	}

	find.exec();

	QVariantList result;
	while (find.next()) {
		QVariantHash row;

		for(auto i = m_attributes.constBegin(); i != m_attributes.constEnd(); ++i) {
			row[i.key()] = find.value(i.key());
		}

		result.append(row);
	}

	qDebug() << result;

	return result;
}

bool SQLModel::update(const QVariantHash& selector, const QVariantHash& values) {
	QSqlQuery query;

	QStringList columns;
	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		columns.append(QString("%1 = :%1_val").arg(i.key()));
	}

	auto req = QString("UPDATE %1 SET %2 WHERE %3;").arg(objectName()).arg(columns.join(", ")).arg(parseSelector(selector));
	qDebug() << req;
	query.prepare(req);

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		query.bindValue(":" + i.key(), i.value());
	}

	for(auto i = values.constBegin(); i != values.constEnd(); ++i) {
		query.bindValue(":" + i.key() + "_val", i.value());
	}

	return query.exec();
}

bool SQLModel::destroy(const QVariantHash& selector, int limit) {
	QSqlQuery del;
	QStringList cond;

	QString lmt;
	if (limit > 0)
		lmt = QString("LIMIT %1").arg(limit);

	auto req = QString("DELETE FROM %1 WHERE %2 %3;").arg(objectName()).arg(parseSelector(selector)).arg(lmt);
	qDebug() << req;
	del.prepare(req);

	for(auto i = selector.constBegin(); i != selector.constEnd(); ++i) {
		del.bindValue(":" + i.key(), i.value());
	}

	return del.exec();
}

SQLManager::SQLManager(QObject* parent) : QObject(parent) {
	auto config = getConfig("database");
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
