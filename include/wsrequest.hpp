#ifndef WSREQUEST_HPP
#define WSREQUEST_HPP

#include <QWebSocket>
#include <abstractrequest.hpp>

class WSRequest : public AbstractRequest {
	Q_OBJECT

	public:
		explicit WSRequest(const QString& socket, QObject* parent = nullptr);
};

#endif // WSREQUEST_HPP

