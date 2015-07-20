#ifndef WSRESPONSE_HPP
#define WSRESPONSE_HPP

#include <QWebSocket>
#include <abstractresponse.hpp>

class WSResponse : public AbstractResponse {
	Q_OBJECT

	public:
		explicit WSResponse(QWebSocket* socket);

	public Q_SLOTS:
		virtual void close(quint16 status = 200);
		void reset();

	private:
		QWebSocket* m_socket;
};

#endif // WSRESPONSE_HPP

