#include <assets.hpp>
#include <obsidian.hpp>

QHash<QString, QString> AssetsManager::s_types = {
	{"css", "text/css"}
};

AssetsManager::AssetsManager(QObject* parent) : QObject(parent) {
	auto conf = Obsidian::getConfig("server");
	if(conf.contains("cacheSize"))
		m_cache.setMaxCost(conf.value("cacheSize").toInt());
}

QVariant AssetsManager::execute(const Controller& cont, QObjectList& arguments) {
	auto req = static_cast<HTTPRequest*>(arguments.first());
	auto res = static_cast<HTTPResponse*>(arguments.last());
	auto type = std::get<1>(cont);

	auto url = req->url().toString();
	qDebug() << type << url;

	res->headers("Content-Type", s_types.value(type));

	auto data = res->data();
	if(m_cache.contains(url)) {
		*data = m_cache.object(url)->toUtf8();
	} else {
		auto astDir = Obsidian::getDir("assets");
		QFile file(astDir.absoluteFilePath(astDir.path() + url));
		if(file.exists()) {
			file.open(QIODevice::ReadOnly | QIODevice::Text);

			auto txt = file.readAll();
			*data = txt;
			m_cache.insert(url, new QString(txt), file.size());

			file.close();
		} else {
			return 404;
		}
	}

	return 200;
}

bool AssetsManager::has(const Controller& cont) {
	return std::get<0>(cont).compare("Assets", Qt::CaseInsensitive) == 0 && s_types.contains(std::get<1>(cont));
}
