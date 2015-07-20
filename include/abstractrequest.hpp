#ifndef ABSTRACTREQUEST
#define ABSTRACTREQUEST

#include <QObject>
#include <QUrl>
#include <QHash>

class AbstractRequest : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString method READ method)
	Q_PROPERTY(QStringList path READ path)
	Q_PROPERTY(QUrl url READ url)
	Q_PROPERTY(QStringList match READ match)

	public:
		explicit AbstractRequest(QObject* parent = nullptr) : QObject(parent) {};

		QString method() const {
			return m_method;
		}

		QList<QString> path() const {
			return m_path;
		}

		QUrl url() const {
			return m_url;
		}

		void setMatch(const QStringList& match) {
			m_match = match;
		}

		QStringList match() const {
			return m_match;
		}

	public Q_SLOTS:
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

	protected:
		QString m_method;
		QUrl m_url;
		QList<QString> m_path;
		QHash<QString, QString> m_headers;
		QHash<QString, QString> m_params;
		QHash<QString, QString> m_post;
		QHash<QString, QString> m_cookies;
		QStringList m_match;
};

#endif // ABSTRACTREQUEST

