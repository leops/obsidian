#ifndef ABSTRACTRESPONSE_HPP
#define ABSTRACTRESPONSE_HPP

#include <QObject>
#include <QVariantHash>

class AbstractResponse : public QObject {
	Q_OBJECT

	public:
		explicit AbstractResponse(QObject* parent = nullptr) : QObject(parent), m_isClosed(false) {}

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
		void render(const QString& name, const QVariantHash& params = QVariantHash());
		void cookies(const QString& key, const QString& value);
		void serveStatic(QString name);

		virtual void close(quint16 status = 200) = 0;

		bool isClosed() {
			return m_isClosed;
		}

	protected:
		void initCodes();

		QHash<QString, QString> m_headers;
		QHash<quint16, QString> m_codes;
		QByteArray m_data;
		bool m_isClosed;
};

#endif // ABSTRACTRESPONSE_HPP

