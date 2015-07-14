#ifndef OBSIDIAN_HPP
#define OBSIDIAN_HPP

#include <QCoreApplication>
#include <QTcpServer>
#include <QMetaObject>
#include <QProcessEnvironment>

#include "httprequest.hpp"
#include "httpresponse.hpp"
#include "global.hpp"
#include "router.hpp"
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

private:
	QTcpServer m_server;
	QList<ModelManager*> m_models;
	QList<ControllerManager*> m_controllers;
	QList<ViewManager*> m_views;
	Router m_router;
};

#endif // OBSIDIAN_HPP
