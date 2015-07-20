#ifndef HTTPREQUEST_HPP
#define HTTPREQUEST_HPP

#include <abstractrequest.hpp>
#include <QAbstractSocket>

class HTTPRequest : public AbstractRequest {
	Q_OBJECT

	public:
		explicit HTTPRequest(QAbstractSocket* socket, QObject* parent = nullptr);

	private:
		QAbstractSocket* m_socket;
};

#endif // HTTPREQUEST_HPP
