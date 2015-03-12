#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <QObject>
#include <QTcpSocket>
#include <QUrl>

class HTTPRequest : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString method READ method)
	Q_PROPERTY(QStringList path READ path)
	Q_PROPERTY(QUrl url READ url)

public:
	explicit HTTPRequest(QTcpSocket* socket, QObject* parent = nullptr);

	QString method() const {
		return m_method;
	}

	QStringList path() const {
		return m_path;
	}

	QUrl url() const {
		return m_url;
	}

public slots:
	QString headers(QString key) const {
		return m_headers[key];
	}

	QString params(const QString& key) const {
		return m_params.value(key);
	}

private:
	QTcpSocket* m_socket;
	QString m_method;
	QUrl m_url;
	QStringList m_path;
	QHash<QString, QString> m_headers;
	QHash<QString, QString> m_params;
};

#endif // HTTPREQUEST_HPP
