#include "controller.hpp"
#include "model.hpp"

bool uncaughtException(QScriptEngine* engine){
	if (engine->hasUncaughtException()) {
		auto line = engine->uncaughtExceptionLineNumber();
		qWarning().noquote() << "Uncaught exception at line" << line << ":" << engine->uncaughtException().toString();
		qWarning().noquote() << engine->uncaughtExceptionBacktrace().join("\n");
		return true;
	}
	return false;
}

ScriptManager::ScriptManager(QObject* parent) : ControllerManager(parent) {
	qScriptRegisterMetaType<QVariantHash>(&m_engine, toScriptValue, fromScriptValue);
	qScriptRegisterMetaType<QList<QVariantHash>>(&m_engine, toScriptValue, fromScriptValue);

	auto global = m_engine.globalObject();
	auto modelList = static_cast<Obsidian*>(parent)->getModels();
	foreach(auto mgr, modelList) {
		auto manager = static_cast<ModelManager*>(mgr);
		auto list = manager->models();
		for(int i = 0; i < list.length(); i++) {
			auto model = manager->models(list[i]);
			auto handle = m_engine.newQObject(model);
			global.setProperty(list[i], handle);
		}
	}

	auto ctrlDir = getDir("controllers");
	foreach (QString fileName, ctrlDir.entryList(QDir::Files)) {
		auto name = fileName.split(".").first();
		qDebug().noquote() << "Loading controller" << name;
		QFile script(ctrlDir.absoluteFilePath(fileName));
		script.open(QIODevice::ReadOnly);

		auto code = QString("(function(controller) { %1; return controller; })({})").arg(QString(script.readAll()));
		m_controllers[name] = m_engine.evaluate(code);
		uncaughtException(&m_engine);

		script.close();
	}
}

void ScriptManager::query(HTTPRequest& req, HTTPResponse& res) {
	auto path = req.path();
	auto ctrl = m_controllers.value(path.value(0, "index"));
	auto func = ctrl.property(path.value(1, "index"));

	QScriptValueList args;
	args << m_engine.newQObject(&req) << m_engine.newQObject(&res);

	if(func.isFunction()) {
		func.call(func, args);
		if(uncaughtException(&m_engine))
			res.close(500);
	} else {
		res.close(404);
	}
}

bool ScriptManager::has(const QString& ctrl, const QString& func) {
	if(m_controllers.constFind(ctrl) == m_controllers.constEnd())
		return false;
	if(!m_controllers.value(ctrl).property(func).isFunction())
		return false;
	return true;
}
