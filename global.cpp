#include "global.hpp"

QDir getDir(QString name) {
	auto dir = QDir(qApp->applicationDirPath());

#if defined(Q_OS_WIN)
	if (dir.dirName().toLower() == "debug" || dir.dirName().toLower() == "release") {
		dir.cdUp();
	}
#elif defined(Q_OS_MAC)
	if (dir.dirName() == "MacOS") {
		dir.cdUp();
	}
#endif

	dir.cd(name);
	return dir;
}

QJsonValue toJSON(const QScriptValue& var) {
	if(var.isBool()) {
		return var.toBool();
	} else if(var.isString() || var.isDate() || var.isRegExp()) {
		return var.toString();
	} else if(var.isNumber()) {
		return var.toNumber();
	} else if(var.isArray()) {
		QJsonArray array;
		QScriptValueIterator it(var);
		while (it.hasNext()) {
			it.next();
			array.append(toJSON(it.value()));
		}
		return array;
	} else if(var.isObject()) {
		QJsonObject obj;
		QScriptValueIterator it(var);
		while (it.hasNext()) {
			it.next();
			obj[it.name()] = toJSON(it.value());
		}
		return obj;
	} else {
		return QJsonValue::Null;
	}
}

QString toString(const QScriptValue& var) {
	if(var.isArray()) {
		return QJsonDocument(toJSON(var).toArray()).toJson();
	} else if(var.isObject()) {
		return QJsonDocument(toJSON(var).toObject()).toJson();
	} else {
		return var.toString();
	}
}

QScriptValue toScriptValue(QScriptEngine* engine, const QVariant& var) {
	switch(var.type()) {
		case QMetaType::QVariantHash:
			return toScriptValue(engine, qvariant_cast<QVariantHash>(var));
		case QMetaType::QVariantList:
			return toScriptValue(engine, qvariant_cast<QVariantList>(var));
		case QMetaType::Bool:
			return var.toBool();
		case QMetaType::QString:
			return var.toString();
		case QMetaType::Int:
		case QMetaType::Long:
		case QMetaType::LongLong:
			return var.toInt();
		default:
			qDebug() << "Unknown type" << var.typeName();
			return engine->newVariant(var);
	}
}

QScriptValue toScriptValue(QScriptEngine* engine, const QVariantHash& hash) {
	auto obj = engine->newObject();
	for(auto i = hash.constBegin(); i != hash.constEnd(); ++i) {
		obj.setProperty(i.key(), toScriptValue(engine, i.value()));
	}
	return obj;
}

QScriptValue toScriptValue(QScriptEngine* engine, const QVariantList& list) {
	auto arr = engine->newArray(list.length());
	for(int i = 0; i < list.length(); i++) {
		arr.setProperty(i, toScriptValue(engine, list[i]));
	}
	return arr;
}

QScriptValue toScriptValue(QScriptEngine* engine, const QList<QVariantHash>& list) {
	auto arr = engine->newArray(list.length());
	for(int i = 0; i < list.length(); i++) {
		arr.setProperty(i, toScriptValue(engine, list[i]));
	}
	return arr;
}

void fromScriptValue(const QScriptValue& obj, QVariantHash& hash) {
	QScriptValueIterator it(obj);
	while(it.hasNext()) {
		it.next();
		hash[it.name()] = it.value().toVariant();
	}
}

void fromScriptValue(const QScriptValue& obj, QVariantList& list) {
	QScriptValueIterator it(obj);
	while(it.hasNext()) {
		it.next();
		list.append(it.value().toVariant());
	}
}

void fromScriptValue(const QScriptValue& obj, QList<QVariantHash>& list) {
	QScriptValueIterator it(obj);
	while(it.hasNext()) {
		it.next();
		list.append(qvariant_cast<QVariantHash>(it.value().toVariant()));
	}
}
