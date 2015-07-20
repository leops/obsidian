#include <wsresponse.hpp>
#include <obsidian.hpp>

WSResponse::WSResponse(QWebSocket* socket) : AbstractResponse(socket), m_socket(socket) {
	initCodes();

	if(!m_socket->isValid())
		close(400);

	reset();
}

void WSResponse::reset() {
	m_headers.clear();
	headers("Server", "Obsidian/0.0.1");
	headers("Content-Type", "text/html; charset=utf-8");

	m_data.clear();
	m_isClosed = false;
}

void WSResponse::close(quint16 status) {
	if (!isClosed()) {
		QJsonDocument res;
		QJsonObject obj;

		obj.insert("code", status);
		obj.insert("status", m_codes[status]);

		QJsonObject heads;
		for (auto i = m_headers.constBegin(); i != m_headers.constEnd(); ++i)
			heads.insert(i.key(), i.value());
		obj.insert("head", heads);

		obj.insert("body", QString(m_data));
		res.setObject(obj);

		m_socket->sendTextMessage(res.toJson());
		m_isClosed = true;
	}
}
