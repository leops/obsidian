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
		return m_headers.value(key, QString());
	}

	QString params(const QString& key) const {
		return m_params.value(key, QString());
	}

	QString post(const QString& key) const {
		return m_post.value(key, QString());
	}

	QString cookies(const QString& key) const {
		return m_cookies.value(key, QString());
	}

private:
	QTcpSocket* m_socket;
	QString m_method;
	QUrl m_url;
	QStringList m_path;
	QHash<QString, QString> m_headers;
	QHash<QString, QString> m_params;
	QHash<QString, QString> m_post;
	QHash<QString, QString> m_cookies;
};

#endif // HTTPREQUEST_HPP
