#ifndef ROUTER_HPP
#define ROUTER_HPP

#include <QObject>
#include <QJsonValue>
#include <QJsonObject>
#include <QStringList>
#include <QDebug>
#include <QRegularExpression>
#include <abstractrequest.hpp>
#include <abstractresponse.hpp>

class Obsidian;
class HTTPResponse;

typedef std::tuple<QString, QString> Controller;

class Router : public QObject {
	public:
		Router(Obsidian*);
		void route(AbstractRequest&, AbstractResponse&);
	protected:
		QVariant execute(const Controller&, QObjectList&);
		Controller getController(const QString& name) {
			auto spl = name.split("::");
			return Controller(spl.at(0).toLower(), spl.at(1).toLower());
		}
	private:
		Obsidian* m_app;
		QJsonObject m_config;
		QHash<QString, QJsonValue> m_routesInfo;
		QHash<QString, QPair<QStringList, QRegularExpression>> m_routes;
};

#endif // ROUTER_HPP
