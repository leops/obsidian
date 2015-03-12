#include <QCoreApplication>
#include <QTcpServer>
#include <QMetaObject>
#include "httprequest.hpp"
#include "httpresponse.hpp"
#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"

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

	server.listen(QHostAddress::Any, 1337);

	return a.exec();
}
