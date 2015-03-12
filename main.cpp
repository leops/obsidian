#include <QCoreApplication>
#include <QTcpServer>
#include <QMetaObject>
#include <QProcessEnvironment>
#include "httprequest.hpp"
#include "httpresponse.hpp"
#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"
#include "global.hpp"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);
	QTcpServer server;
	Model model;
	View view;
	Controller ctrl(&model);

	QObject::connect(&server, &QTcpServer::newConnection, [&] () {
		auto socket = server.nextPendingConnection();
		HTTPRequest req(socket);
		HTTPResponse res(socket, &view);

		qDebug() << req.method() << req.url().toString();

		ctrl.query(req, res);
	});

	QFile cfg(getDir("config").absoluteFilePath("server.json"));
	cfg.open(QIODevice::ReadOnly | QIODevice::Text);
	auto config = QJsonDocument::fromJson(cfg.readAll()).object();
	cfg.close();

	quint16 port = QProcessEnvironment::systemEnvironment().value("PORT", "80").toInt();
	if(config.contains("port"))
		port = config["port"].toInt();
	server.listen(QHostAddress::Any, port);

	return a.exec();
}
