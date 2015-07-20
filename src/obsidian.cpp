#include "obsidian.hpp"
#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"
#include <QPluginLoader>
#include <wsrequest.hpp>
#include <wsresponse.hpp>

Obsidian::Obsidian(int& argc, char** argv) : QCoreApplication(argc, argv), m_server(this), m_websocket("", QWebSocketServer::NonSecureMode, this), m_router(this) {
	auto pluginsDir = getDir("plugins");

	foreach (QString fileName, pluginsDir.entryList({"mdl_*"}, QDir::Files)) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		auto plugin = loader.instance();
		if (plugin) {
			ModelManager* model = qobject_cast<ModelManager*>(plugin);
			if(model)
				m_models.append(model);
		} else {
			qDebug() << loader.errorString();
		}
	}

	foreach (QString fileName, pluginsDir.entryList({"ctrl_*"}, QDir::Files)) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		auto plugin = loader.instance();
		if (plugin) {
			ControllerManager* controller = qobject_cast<ControllerManager*>(plugin);
			if(controller)
				m_controllers.append(controller);
		} else {
			qDebug() << loader.errorString();
		}
	}

	//m_views.append(new OTPManager(this));
	foreach (QString fileName, pluginsDir.entryList({"view_*"}, QDir::Files)) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		auto plugin = loader.instance();
		if (plugin) {
			ViewManager* view = qobject_cast<ViewManager*>(plugin);
			if(view)
				m_views.append(view);
		} else {
			qDebug() << loader.errorString();
		}
	}

	connect(&m_server, &QTcpServer::newConnection, [&] () {
		auto socket = m_server.nextPendingConnection();
		HTTPRequest req(socket, this);
		HTTPResponse res(socket);

		qDebug().noquote() << req.method() << req.url().toString();
		return m_router.route(req, res);
	});

	connect(&m_websocket, &QWebSocketServer::newConnection, [&] () {
		auto socket = m_websocket.nextPendingConnection();
		connect(socket, &QWebSocket::textMessageReceived, [=] (const QString& data) {
			WSRequest req(data, this);
			WSResponse res(socket);

			qDebug().noquote() << "WS" << req.method() << req.url().toString();
			return m_router.route(req, res);
		});
	});

	quint16 port = 80;

	auto config = getConfig("server");
	if(config.contains("port"))
		port = config["port"].toInt();

	auto env = QProcessEnvironment::systemEnvironment();
	if(env.contains("PORT"))
		port = env.value("PORT").toInt();

	m_server.listen(QHostAddress::Any, port);
	qDebug() << "Listening on port" << m_server.serverPort();

	m_websocket.listen();
	qDebug() << "Websocket on port" << m_websocket.serverPort();
}
