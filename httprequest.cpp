#include "httprequest.hpp"

HTTPRequest::HTTPRequest(QTcpSocket* socket, QObject *parent) : QObject(parent), m_socket(socket) {
	m_socket->waitForReadyRead();
	auto data = m_socket->readAll();

	auto request = data.mid(0, data.indexOf('\n') + 1).trimmed().split(' ');
	m_method = request[0];
	m_url = QUrl(request[1]);
	m_path = m_url.path().split("/").filter(QRegExp("^.+$"));
	auto query = m_url.query().split("&");
	foreach(auto param, query) {
		auto p = param.split("=");
		m_params[p[0]] = p[1];
	}

	data = data.mid(data.indexOf('\n') + 1).trimmed();
	foreach(auto line, data.split('\n')) {
		auto colon = line.indexOf(':');
		auto headerName = line.left(colon).trimmed();
		auto headerValue = line.mid(colon + 1).trimmed();
		m_headers.insertMulti(headerName, headerValue);
	}
}
