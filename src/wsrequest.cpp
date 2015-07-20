#include <wsrequest.hpp>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

WSRequest::WSRequest(const QString& socket, QObject *parent) : AbstractRequest(parent) {
	auto data = QJsonDocument::fromJson(socket.toUtf8()).object();
	if(data.isEmpty())
		return;

	m_method = data.value("method").toString();
	m_url = QUrl(data.value("url").toString());
	m_path = m_url.path().split("/").filter(QRegExp("^.+$"));
	if(!m_url.query().isEmpty()) {
		auto query = m_url.query().split("&");
		Q_FOREACH(auto param, query) {
			auto p = param.split("=");
			m_params[p[0]] = p[1];
		}
	}

	auto head = data.value("head").toObject().toVariantHash();
	for(auto it = head.constBegin(); it != head.constEnd(); ++it) {
		m_headers.insert(it.key(), it.value().toString());
	}

	if(m_headers.contains("Cookie")) {
		auto cookies = headers("Cookie").split(QRegExp(";\\s?"));
		Q_FOREACH(auto c, cookies) {
			auto cookie = c.split("=");
			m_cookies.insert(cookie.at(0), cookie.at(1));
		}
	}

	auto body = data.value("body").toString();
	if(!body.isEmpty()) {
		auto b = QUrl::fromEncoded(body.toUtf8()).toString().split("&");
		Q_FOREACH(auto v, b) {
			auto d = v.split("=");
			m_post[d.at(0)] = d.at(1);
		}
	}
}
