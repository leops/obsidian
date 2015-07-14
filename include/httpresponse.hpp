#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <QObject>
#include <QTcpSocket>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <sstream>
#include "global.hpp"
#include "view.hpp"

class HTTPResponse : public QObject {
	Q_OBJECT

public:
	explicit HTTPResponse(QTcpSocket* socket, QObject* parent = nullptr);

public Q_SLOTS:
	QString headers(QString key) const {
		return m_headers.value(key);
	}

	QString headers(const QString& key, const QString& value, const bool& multi = false) {
		if(multi)
			m_headers.insertMulti(key, value);
		else
			m_headers.insert(key, value);
		return headers(key);
	}

	QByteArray* data() {
		return &m_data;
	}

	void redirect(QString path, quint16 status = 301) {
		headers("Location", path);
		close(status);
	}

	void json(const QVariantHash& data, const bool indented = false);
	void render(const QString& name, const QVariantHash& params);
	void cookies(const QString& key, const QString& value);

	void serveStatic(QString name);
	void close(quint16 status = 200);
	bool isClosed() {
		return m_isClosed;
	}

protected:
	void initCodes();

private:
	QTcpSocket* m_socket;
	QHash<QString, QString> m_headers;
	QHash<quint16, QString> m_codes;
	QByteArray m_data;
	bool m_isClosed;
};

#endif // HTTPRESPONSE_HPP
