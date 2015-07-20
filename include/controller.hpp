#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include <QObject>
#include <httprequest.hpp>
#include <httpresponse.hpp>

typedef std::tuple<QString, QString> Controller;

class ControllerManager {
	public:
		virtual QVariant execute(const Controller& , QObjectList&) = 0;
		virtual bool has(const Controller&) = 0;
};

Q_DECLARE_INTERFACE(ControllerManager, "com.obsidian.ControllerManager")

#endif // CONTROLLER_HPP
