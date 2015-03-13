#include "httprequest.hpp"

HTTPRequest::HTTPRequest(QTcpSocket* socket, QObject *parent) : QObject(parent), m_socket(socket) {
	m_socket->waitForReadyRead();
	auto data = QString(m_socket->readAll());

	auto request = data.mid(0, data.indexOf('\n') + 1).trimmed().split(' ');
	m_method = request[0];
	m_url = QUrl(request[1]);
	m_path = m_url.path().split("/").filter(QRegExp("^.+$"));
	if(!m_url.query().isEmpty()) {
		auto query = m_url.query().split("&");
		foreach(auto param, query) {
			auto p = param.split("=");
			m_params[p[0]] = p[1];
		}
	}

	data = data.mid(data.indexOf('\n') + 1);
	QString head, body;
	auto split = QRegExp("\r\n\r\n");
	if(data.indexOf(split) != -1) {
		auto d = data.split(split);
		head = d.at(0).trimmed();
		body = d.at(1).trimmed();
	} else {
		head = data.trimmed();
		body = "";
	}

	foreach(auto line, head.split('\n')) {
		auto colon = line.indexOf(':');
		auto headerName = line.left(colon).trimmed();
		auto headerValue = line.mid(colon + 1).trimmed();
		m_headers.insertMulti(headerName, headerValue);
	}

	if(m_headers.contains("Cookie")) {
		auto cookies = headers("Cookie").split(QRegExp(";\s?"));
		foreach(auto c, cookies) {
			auto cookie = c.split("=");
			m_cookies.insert(cookie.at(0), cookie.at(1));
		}
	}

	if(!body.isEmpty()) {
		auto b = QUrl::fromEncoded(body.toUtf8()).toString().split("&");
		foreach(auto v, b) {
			auto d = v.split("=");
			m_post[d.at(0)] = d.at(1);
		}
	}
}
