#include "httpresponse.hpp"

HTTPResponse::HTTPResponse(QAbstractSocket* socket) : AbstractResponse(socket), m_socket(socket) {
	initCodes();
	m_socket->waitForConnected();

	if(!m_socket->isWritable())
		close(408);

	headers("Server", "Obsidian/0.0.1");
	headers("Content-Type", "text/html; charset=utf-8");
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
