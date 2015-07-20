#include "router.hpp"
#include "obsidian.hpp"
#include "controller.hpp"

QRegularExpression makeRegExp(QStringList routes) {
	QStringList matchers;
	for(int i = 0; i < routes.size(); i++) {
		matchers << QString("(?<route%1>%2)").arg(i).arg(routes.at(i));
	}
	return QRegularExpression("^(?:" + matchers.join('|') + ")$", QRegularExpression::ExtendedPatternSyntaxOption);
}

Router::Router(Obsidian* app) {
	m_app = app;
	m_config = app->getConfig("routes");

	auto keys = m_config.keys();
	for(int i = 0; i < keys.size(); i++) {
		auto key = keys.at(i);
		auto spl = key.split(" ");

		auto method = spl.at(0).toUpper();
		if(!m_routes.contains(method)) {
			m_routes.insert(method, QPair<QStringList, QRegularExpression>());
		}

		auto url = spl.at(1);
		m_routesInfo.insert(QString("route%1").arg(i), m_config.value(key));
		m_routes[method].first.insert(i, url);
		m_routes[method].second = makeRegExp(m_routes[method].first);
	}
}

QVariant Router::execute(const Controller& cont, QObjectList& args) {
	auto controllers = m_app->getControllers();
	foreach(auto controller, controllers) {
		if (controller->has(cont)) {
			return controller->execute(cont, args);
		}
	}
	return QVariant();
}

void Router::route(AbstractRequest& req, AbstractResponse& res) {
	auto method = req.method().toUpper();
	if(!m_routes.contains(method)) {
		qDebug() << " -> Method not allowed" << method;
		return res.close(405);
	}

	auto regex = m_routes.value(method).second;
	auto url = req.url().toString();

	auto data = regex.match(url);
	if(!data.hasMatch()) {
		qDebug() << " -> No matching route found";
		return res.close(404);
	}

	auto txts = data.capturedTexts();
	int i = 1; for(; txts.at(i).isEmpty(); i++);

	auto groups = regex.namedCaptureGroups();
	int j = 1; for(; (i + j) < groups.size() && groups.at(i + j).isEmpty(); j++);

	auto info = m_routesInfo.value(groups.at(i));
	req.setMatch(txts.mid(i, j));

	if(info.isString())
		info = QJsonArray({info});

	int status = 404;
	QObjectList args = QObjectList() << &req << &res;
	Q_FOREACH(auto controller, info.toArray()) {
		auto cont = getController(controller.toString());
		qDebug().noquote().nospace() << " -> Using " << std::get<0>(cont) << "::" << std::get<1>(cont);

		auto ret = execute(cont, args);
		if (res.isClosed()) {
			break;
		} else {
			if (ret.isNull()) {
				qDebug() << " -> Controller not found";
			} else if (static_cast<QMetaType::Type>(ret.type()) == QMetaType::Int) {
				status = ret.toInt();
				qDebug() << " -> Controller returned status" << status;
			}
		}
	}

	if(!res.isClosed())
		res.close(status);
}
