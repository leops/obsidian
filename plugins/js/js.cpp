#include <js.hpp>
#include <obsidian.hpp>
#include <model.hpp>
#include <global.hpp>
#include <QScriptValueIterator>

bool uncaughtException(QScriptEngine* engine){
	if (engine->hasUncaughtException()) {
		auto line = engine->uncaughtExceptionLineNumber();
		qWarning().noquote() << "Uncaught exception at line" << line << ":" << engine->uncaughtException().toString();
		qWarning().noquote() << engine->uncaughtExceptionBacktrace().join("\n");
		return true;
	}
	return false;
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
	switch(static_cast<QMetaType::Type>(var.type())) {
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

ScriptManager::ScriptManager(QObject* parent) : QObject(parent) {
	qScriptRegisterMetaType<QVariantHash>(&m_engine, toScriptValue, fromScriptValue);
	qScriptRegisterMetaType<QList<QVariantHash>>(&m_engine, toScriptValue, fromScriptValue);

	auto app = dynamic_cast<Obsidian*>(Obsidian::instance());
	if(app) {
		auto global = m_engine.globalObject();
		auto modelList = app->getModels();
		Q_FOREACH(auto manager, modelList) {
			auto list = manager->models();
			for(int i = 0; i < list.length(); i++) {
				auto model = dynamic_cast<QObject*>(manager->models(list[i]));
				if(model) {
					auto handle = m_engine.newQObject(model);
					global.setProperty(list[i], handle);
				}
			}
		}
	}

	auto ctrlDir = getDir("controllers");
	Q_FOREACH (QString fileName, ctrlDir.entryList({"*.js"}, QDir::Files)) {
		auto name = fileName.split(".").first().toLower();
		qDebug().noquote() << "Loading controller" << name;
		QFile script(ctrlDir.absoluteFilePath(fileName));
		script.open(QIODevice::ReadOnly);

		auto code = QString("(function(controller) { %1; return controller; })({})").arg(QString(script.readAll()));
		m_controllers[name] = m_engine.evaluate(code);
		uncaughtException(&m_engine);

		script.close();
	}
}

QVariant ScriptManager::execute(const Controller& cont, QObjectList& arguments) {
	auto ctrl = m_controllers.value(std::get<0>(cont));
	auto func = ctrl.property(std::get<1>(cont));

	QScriptValueList args;
	Q_FOREACH(auto arg, arguments) {
		args << m_engine.newQObject(arg);
	}

	if(func.isFunction()) {
		auto ret = func.call(func, args).toVariant();
		if (uncaughtException(&m_engine)) {
			return 500;
		} else {
			return ret;
		}
	} else {
		return 404;
	}
}

bool ScriptManager::has(const Controller& cont) {
	auto ctrl = std::get<0>(cont);
	auto func = std::get<1>(cont);

	if(m_controllers.constFind(ctrl) == m_controllers.constEnd())
		return false;
	if(!m_controllers.value(ctrl).property(func).isFunction())
		return false;
	return true;
}
