#include "controller.hpp"
#include "model.hpp"

bool uncaughtException(QScriptEngine* engine){
	if (engine->hasUncaughtException()) {
		auto line = engine->uncaughtExceptionLineNumber();
		qWarning() << "Uncaught exception at line" << line << ":" << engine->uncaughtException().toString();
		qWarning() << engine->uncaughtExceptionBacktrace().join("\n\t").prepend("\t");
		return true;
	}
	return false;
}

Controller::Controller(Model* model, QObject* parent) : QObject(parent), m_model(model) {
	loadModels();
	auto ctrlDir = getDir("controllers");
	foreach (QString fileName, ctrlDir.entryList(QDir::Files)) {
		auto name = fileName.split(".").first();
		qDebug() << "Loading controller" << name;
		QFile script(ctrlDir.absoluteFilePath(fileName));
		script.open(QIODevice::ReadOnly);

		auto code = QString("(function(exports) { %1; return exports; })({})").arg(script.readAll().toStdString().c_str());
		m_controllers[name] = m_engine.evaluate(code);
		uncaughtException(&m_engine);

		script.close();
	}
}

QScriptValue find(QScriptContext* context, QScriptEngine* engine) {
	auto ctx = qobject_cast<Model*>(qvariant_cast<QObject*>(context->callee().property("ctx").toVariant()));
	auto model = context->callee().property("model").toString();

	Model::Row selector;
	if(context->argumentCount() > 0) {
		QScriptValueIterator it(context->argument(0).toObject());
		while(it.hasNext()) {
			it.next();
			selector[it.name()] = it.value().toVariant();
		}
	}

	auto result = ctx->find(model, selector);

	auto ret = engine->newArray();
	for(int i = 0; i < result.length(); i++) {
		auto row = engine->newObject();
		for(auto j = result[i].constBegin(); j != result[i].constEnd(); ++j) {
			row.setProperty(j.key(), toScript(j.value()));
		}
		ret.setProperty(i, row);
	}
	return ret;
}

QScriptValue create(QScriptContext* context, QScriptEngine* engine) {
	Q_UNUSED(engine);
	auto ctx = qobject_cast<Model*>(qvariant_cast<QObject*>(context->callee().property("ctx").toVariant()));
	auto model = context->callee().property("model").toString();

	Model::Row values;
	QScriptValueIterator it(context->argument(0).toObject());
	while(it.hasNext()) {
		it.next();
		values[it.name()] = it.value().toVariant();
	}

	return ctx->create(model, values);
}

QScriptValue update(QScriptContext* context, QScriptEngine* engine) {
	Q_UNUSED(engine);
	auto ctx = qobject_cast<Model*>(qvariant_cast<QObject*>(context->callee().property("ctx").toVariant()));
	auto model = context->callee().property("model").toString();

	Model::Row selector;
	QScriptValueIterator it(context->argument(0).toObject());
	while(it.hasNext()) {
		it.next();
		selector[it.name()] = it.value().toVariant();
	}

	Model::Row values;
	QScriptValueIterator it2(context->argument(1).toObject());
	while(it2.hasNext()) {
		it2.next();
		values[it2.name()] = it2.value().toVariant();
	}

	return ctx->update(model, selector, values);
}

QScriptValue destroy(QScriptContext* context, QScriptEngine* engine) {
	Q_UNUSED(engine);
	auto ctx = qobject_cast<Model*>(qvariant_cast<QObject*>(context->callee().property("ctx").toVariant()));
	auto model = context->callee().property("model").toString();

	Model::Row selector;
	QScriptValueIterator it(context->argument(0).toObject());
	while(it.hasNext()) {
		it.next();
		selector[it.name()] = it.value().toVariant();
	}

	return ctx->destroy(model, selector);
}

void Controller::loadModels() {
	auto global = m_engine.globalObject();
	auto list = m_model->modelList();
	for(int i = 0; i < list.length(); i++) {
		auto handle = m_engine.newObject();

		typedef std::tuple<QString, QScriptEngine::FunctionSignature, int> Method;
		auto methods = {
			Method("find", find, 0),
			Method("create", create, 1),
			Method("update", update, 2),
			Method("destroy", destroy, 1)
		};

		for(auto j : methods) {
			auto func = m_engine.newFunction(std::get<1>(j), std::get<2>(j));
			func.setProperty("model", list[i]);
			func.setProperty("ctx", m_engine.newVariant(QVariant(QMetaType::QObjectStar, &m_model)));
			handle.setProperty(std::get<0>(j), func);
		}

		global.setProperty(list[i], handle);
	}
}

void Controller::query(HTTPRequest& req, HTTPResponse& res) {
	auto path = req.path();
	QString ctrl = "index",
			func = "index";
	if(path.length() > 0)
		ctrl = path[0];
	if(path.length() > 1)
		func = path[1];

	QScriptValueList args;
	args << m_engine.newQObject(&req) << m_engine.newQObject(&res);

	if(m_controllers.constFind(ctrl) != m_controllers.constEnd()) {
		auto code = m_controllers[ctrl].property(func);
		if(code.isFunction()) {
			code.call(code, args);
			if(uncaughtException(&m_engine))
				res.close(500);
		} else {
			res.close(404);
		}
	} else {
		res.close(404);
	}
}
