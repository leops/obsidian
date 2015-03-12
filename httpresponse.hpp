#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <sstream>
#include "global.hpp"
#include "view.hpp"

class HTTPResponse : public QObject {
	Q_OBJECT

public:
	explicit HTTPResponse(QTcpSocket* socket, View* view, QObject* parent = nullptr);

public slots:
	QString headers(QString key) const {
		return m_headers[key];
	}

	QString headers(QString key, QString value) {
		m_headers[key] = value;
		return headers(key);
	}

	QByteArray* data() {
		return &m_data;
	}

	void redirect(QString path, quint16 status = 301) {
		headers("Location", path);
		close(status);
	}

	void json(const QScriptValue&, const bool = false);
	void render(const QString&, const QScriptValue& = QScriptValue());

	void serveStatic(QString name);
	void close(quint16 status = 200);

private:
	QTcpSocket* m_socket;
	QMap<QString, QString> m_headers;
	QByteArray m_data;
	View* m_view;
};

#endif // HTTPRESPONSE_HPP
