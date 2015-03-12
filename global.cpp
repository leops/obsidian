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

QScriptValue toScript(const QVariant& var) {
	switch(var.type()) {
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
			return QScriptValue::NullValue;
	}
}
