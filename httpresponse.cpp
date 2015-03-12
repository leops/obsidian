#include "httpresponse.hpp"

HTTPResponse::HTTPResponse(QTcpSocket* socket, View* view, QObject *parent) : QObject(parent), m_socket(socket), m_view(view) {
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
	headers("Content-Length", QString("%1").arg(m_data.size()));

	std::stringstream stream;
	stream << "HTTP/1.1 " << status << " OK\n";
	for(auto h : m_headers.toStdMap()) {
		stream << h.first.toStdString() << ": " << h.second.toStdString() << "\n";
	}
	stream << "\n";

	m_socket->write(stream.str().c_str());
	m_socket->write(m_data);
	m_socket->close();
}

void HTTPResponse::render(const QString &name, const QScriptValue &params) {
	m_data = m_view->render(name, params);
	close(200);
}
