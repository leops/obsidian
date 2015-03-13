#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <QObject>
#include <QScriptEngine>
#include "global.hpp"
#include "httprequest.hpp"
#include "httpresponse.hpp"
#include "obsidian.hpp"

class ControllerManager : public QObject {
	Q_OBJECT

public:
	explicit ControllerManager(QObject* parent = nullptr) : QObject(parent) {}
	virtual void query(HTTPRequest&, HTTPResponse&) = 0;
	virtual bool has(const QString&, const QString&) = 0;
};

Q_DECLARE_INTERFACE(ControllerManager, "com.obsidian.ControllerManager")

class ScriptManager : public ControllerManager {
	Q_OBJECT
	Q_INTERFACES(ControllerManager)

public:
	explicit ScriptManager(QObject* parent = nullptr);
	virtual void query(HTTPRequest&, HTTPResponse&);
	virtual bool has(const QString&, const QString&);

protected:
	void loadModels();

private:
	QScriptEngine m_engine;
	QHash<QString, QScriptValue> m_controllers;
};

#endif // CONTROLLER_HPP
