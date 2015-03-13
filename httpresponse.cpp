#include "httpresponse.hpp"
#include "obsidian.hpp"

HTTPResponse::HTTPResponse(QTcpSocket* socket, QObject *parent) : QObject(parent), m_socket(socket) {
	initCodes();
	m_socket->waitForConnected();
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

void HTTPResponse::json(const QScriptValue &data, const bool indented) {
	headers("Content-Type", "application/json");
	auto json = toJSON(data);
	auto format = QJsonDocument::JsonFormat::Compact;
	if(indented)
		format = QJsonDocument::JsonFormat::Indented;
	if(json.isArray()) {
		auto arr = json.toArray();
		arr.removeLast();
		m_data = QJsonDocument(arr).toJson(format);
	} else if(json.isObject()) {
		m_data = QJsonDocument(json.toObject()).toJson(format);
	}
	close(200);
}

void HTTPResponse::close(quint16 status) {
	headers("Content-Length", QString::number(m_data.size()));

	QString str;
	QTextStream stream(&str);
	stream << "HTTP/1.1 " << status << " " << m_codes[status] << "\n";
	for(auto i = m_headers.constBegin(); i != m_headers.constEnd(); ++i) {
		stream << i.key() << ": " << i.value() << "\n";
	}
	stream << "\n";

	m_socket->write(str.toUtf8());
	m_socket->write(m_data);
	m_socket->close();
}

void HTTPResponse::render(const QString& name, const QVariantHash& params) {
	auto viewList = static_cast<Obsidian*>(parent())->getViews();
	foreach(auto mgr, viewList) {
		auto manager = static_cast<ViewManager*>(mgr);
		if(manager->has(name)) {
			m_data = manager->render(name, params);
			return close(200);
		}
	}
	close(404);
}

void HTTPResponse::initCodes() {
	m_codes[200] = "OK";
	m_codes[404] = "NOT FOUND";
	m_codes[500] = "SERVER ERROR";
}
