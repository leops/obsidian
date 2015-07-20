#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include <abstractresponse.hpp>
#include <QAbstractSocket>

class HTTPResponse : public AbstractResponse {
	Q_OBJECT

	public:
		explicit HTTPResponse(QAbstractSocket* socket);

	public Q_SLOTS:
		virtual void close(quint16 status = 200);

	private:
		QAbstractSocket* m_socket;
};

#endif // HTTPRESPONSE_HPP
