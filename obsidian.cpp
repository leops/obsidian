#include "obsidian.hpp"
#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"

Obsidian::Obsidian(int& argc, char** argv) : QCoreApplication(argc, argv) {

	m_models.append(new SQLManager(this));
	m_controllers.append(new ScriptManager(this));
	m_views.append(new OTPManager(this));

	connect(&m_server, &QTcpServer::newConnection, [&] () {
		auto socket = m_server.nextPendingConnection();
		HTTPRequest req(socket, this);
		HTTPResponse res(socket, this);

		qDebug().noquote() << req.method() << req.url().toString();

		auto path = req.path();
		auto cont = path.value(0, "index");
		auto func = path.value(1, "index");
		foreach(auto ctrl, m_controllers) {
			auto controller = qobject_cast<ControllerManager*>(ctrl);
			if(controller->has(cont, func))
				return controller->query(req, res);
		}

		return res.close(404);
	});

	QFile cfg(getDir("config").absoluteFilePath("server.json"));
	cfg.open(QIODevice::ReadOnly | QIODevice::Text);
	auto config = QJsonDocument::fromJson(cfg.readAll()).object();
	cfg.close();

	quint16 port = QProcessEnvironment::systemEnvironment().value("PORT", "80").toInt();
	if(config.contains("port"))
		port = config["port"].toInt();
	m_server.listen(QHostAddress::Any, port);

}
