#include <abstractresponse.hpp>
#include <obsidian.hpp>

void AbstractResponse::serveStatic(QString name) {
	auto astDir = Obsidian::getDir("assets");
	auto fileName = astDir.absoluteFilePath(name);
	if(QFile::exists(fileName)) {
		QFile file(fileName);
		file.open(QIODevice::ReadOnly | QIODevice::Text);
		m_data = file.readAll();
		close();
		file.close();
	} else {
		close(404);
	}
}

void AbstractResponse::json(const QVariantHash& data, const bool indented) {
	headers("Content-Type", "application/json");

	auto format = indented ? QJsonDocument::JsonFormat::Indented : QJsonDocument::JsonFormat::Compact;
	auto json = QJsonObject::fromVariantHash(data);
	m_data = QJsonDocument(json).toJson(format);

	close(200);
}

void AbstractResponse::cookies(const QString &key, const QString& value) {
	headers("Set-Cookie", key + "=" + value, true);
}

void AbstractResponse::render(const QString& name, const QVariantHash& params) {
	auto viewList = static_cast<Obsidian*>(QCoreApplication::instance())->getViews();
	Q_FOREACH(auto mgr, viewList) {
		auto manager = static_cast<ViewManager*>(mgr);
		if(manager->has(name)) {
			m_data = manager->render(name, params);
			return close(200);
		}
	}
	close(404);
}

void AbstractResponse::initCodes() {
	m_codes[100] = "CONTINUE";
	m_codes[101] = "SWITCHING PROTOCOLS";

	m_codes[200] = "OK";
	m_codes[201] = "CREATED";
	m_codes[202] = "ACCEPTED";

	m_codes[301] = "MOVED";
	m_codes[302] = "FOUND";
	m_codes[303] = "SEE OTHER";
	m_codes[304] = "NOT MODIFIED";

	m_codes[400] = "BAD REQUEST";
	m_codes[401] = "UNAUTHORIZED";
	m_codes[403] = "FORBIDDEN";
	m_codes[404] = "NOT FOUND";
	m_codes[405] = "METHOD NOT ALLOWED";
	m_codes[408] = "REQUEST TIMEOUT";
	m_codes[409] = "CONFLICT";
	m_codes[410] = "GONE";
	m_codes[429] = "TOO MANY REQUESTS";

	m_codes[500] = "SERVER ERROR";
	m_codes[501] = "NOT IMPLEMENTED";
	m_codes[503] = "UNAVAILABLE";
	m_codes[512] = "IM A TEAPOT";
}
