#ifndef PHP_HPP
#define PHP_HPP

#include <controller.hpp>
#include <QScriptEngine>
#include <QScriptValue>

class ScriptManager : public QObject, public ControllerManager {
	Q_OBJECT
	Q_INTERFACES(ControllerManager)
	Q_PLUGIN_METADATA(IID "com.obsidian.ControllerManager" FILE "js.json")

	public:
		explicit ScriptManager(QObject* parent = nullptr);
		virtual QVariant execute(const Controller&, QObjectList&);
		virtual bool has(const Controller&);

	protected:
		void loadModels();

	private:
		QScriptEngine m_engine;
		QHash<QString, QScriptValue> m_controllers;
};

#endif // PHP_HPP

