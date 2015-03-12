#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <QObject>
#include <QScriptEngine>
#include "global.hpp"
#include "httprequest.hpp"
#include "httpresponse.hpp"
#include "model.hpp"

class Controller : public QObject {
	Q_OBJECT
public:
	explicit Controller(Model* model, QObject* parent = nullptr);
	virtual void query(HTTPRequest&, HTTPResponse&);
protected:
	void loadModels();
private:
	QScriptEngine m_engine;
	QHash<QString, QScriptValue> m_controllers;
	Model* m_model;
};

#endif // CONTROLLER_HPP
