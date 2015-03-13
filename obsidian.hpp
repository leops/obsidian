#ifndef OBSIDIAN_HPP
#define OBSIDIAN_HPP

#include <QCoreApplication>
#include <QTcpServer>
#include <QMetaObject>
#include <QProcessEnvironment>
#include "httprequest.hpp"
#include "httpresponse.hpp"
#include "global.hpp"

class Obsidian : public QCoreApplication {

public:
	Obsidian(int& argc, char** argv);
	QObjectList getModels() const {
		return m_models;
	}
	QObjectList getViews() const {
		return m_views;
	}

private:
	QTcpServer m_server;
	QObjectList m_models;
	QObjectList m_controllers;
	QObjectList m_views;
};

#endif // OBSIDIAN_HPP
