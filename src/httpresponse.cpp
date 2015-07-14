#include "httpresponse.hpp"
#include "obsidian.hpp"

HTTPResponse::HTTPResponse(QTcpSocket* socket, QObject *parent) : QObject(parent), m_socket(socket), m_isClosed(false) {
	initCodes();
	m_socket->waitForConnected();

	if(!m_socket->isWritable())
		close(408);

	headers("Server", "Obsidian/0.0.1");
	headers("Content-Type", "text/html; charset=utf-8");
}

void HTTPResponse::serveStatic(QString name) {
	auto astDir = getDir("assets");
	auto fileName = astDir.absoluteFilePath(name);
	if(QFile::exists(fileName)) {
		QFile file(fileName);
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		m_data = file.readAll();
		close();
		file.close();
	} else {
		close(404);
	}
}

void HTTPResponse::json(const QVariantHash& data, const bool indented) {
	headers("Content-Type", "application/json");

	auto format = indented ? QJsonDocument::JsonFormat::Indented : QJsonDocument::JsonFormat::Compact;
	auto json = QJsonObject::fromVariantHash(data);
	m_data = QJsonDocument(json).toJson(format);

	close(200);
}

void HTTPResponse::cookies(const QString &key, const QString& value) {
	headers("Set-Cookie", key + "=" + value, true);
}

void HTTPResponse::close(quint16 status) {
	if (!isClosed()) {
		headers("Content-Length", QString::number(m_data.size()));

		QString str;
		QTextStream stream(&str);
		stream << "HTTP/1.1 " << status << " " << m_codes[status] << "\n";
		for (auto i = m_headers.constBegin(); i != m_headers.constEnd(); ++i) {
			stream << i.key() << ": " << i.value() << "\n";
		}
		stream << "\n";

		m_socket->write(str.toUtf8());
		m_socket->write(m_data);
		m_socket->close();
		m_isClosed = true;
	}
}

void HTTPResponse::render(const QString& name, const QVariantHash& params) {
	qDebug() << "render" << params;
	auto viewList = static_cast<Obsidian*>(parent())->getViews();
	Q_FOREACH(auto mgr, viewList) {
		auto manager = static_cast<ViewManager*>(mgr);
		if(manager->has(name)) {
			m_data = manager->render(name, params);
			return close(200);
		}
	}
	close(404);
}

void HTTPResponse::initCodes() {
	m_codes[100] = "CONTINUE";
	m_codes[101] = "SWITCHING PROTOCOLS";

	m_codes[200] = "OK";
	m_codes[201] = "CREATED";
	m_codes[202] = "ACCEPTED";

	m_codes[301] = "MOVED";
	m_codes[302] = "FOUND";
	m_codes[303] = "SEE OTHER";
	m_codes[304] = "NOT MODIFIED";

	m_codes[400] = "BAD REQUEST";
	m_codes[401] = "UNAUTHORIZED";
	m_codes[403] = "FORBIDDEN";
	m_codes[404] = "NOT FOUND";
	m_codes[405] = "METHOD NOT ALLOWED";
	m_codes[408] = "REQUEST TIMEOUT";
	m_codes[409] = "CONFLICT";
	m_codes[410] = "GONE";
	m_codes[429] = "TOO MANY REQUESTS";

	m_codes[500] = "SERVER ERROR";
	m_codes[501] = "NOT IMPLEMENTED";
	m_codes[503] = "UNAVAILABLE";
	m_codes[512] = "IM A TEAPOT";
}
