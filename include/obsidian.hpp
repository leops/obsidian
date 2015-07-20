#ifndef OBSIDIAN_HPP
#define OBSIDIAN_HPP

#include <QCoreApplication>
#include <QTcpServer>
#include <QWebSocketServer>
#include <QMetaObject>
#include <QProcessEnvironment>
#include <QDir>

#include <httprequest.hpp>
#include <httpresponse.hpp>
#include <router.hpp>
#include <controller.hpp>
#include <view.hpp>
#include <model.hpp>

class Obsidian : public QCoreApplication {

public:
	Obsidian(int& argc, char** argv);

	QList<ModelManager*> getModels() const {
		return m_models;
	}
	QList<ViewManager*> getViews() const {
		return m_views;
	}
	QList<ControllerManager*> getControllers() const {
		return m_controllers;
	}

	static QDir getDir(QString name) {
		auto dir = QDir::current();
		dir.cd(name);
		return dir;
	}

	static QJsonObject getConfig(QString name) {
		QFile cfg(getDir("config").absoluteFilePath(name + ".json"));

		cfg.open(QIODevice::ReadOnly | QIODevice::Text);
		auto data = cfg.readAll();
		cfg.close();

		return QJsonDocument::fromJson(data).object();
	}

private:
	QTcpServer m_server;
	QWebSocketServer m_websocket;

	QList<ModelManager*> m_models;
	QList<ControllerManager*> m_controllers;
	QList<ViewManager*> m_views;
	Router m_router;
};

#endif // OBSIDIAN_HPP
