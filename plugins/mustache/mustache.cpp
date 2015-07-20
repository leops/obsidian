#include <mustache.hpp>
#include <parser.h>
#include <obsidian.hpp>

MustacheManager::MustacheManager(QObject* parent) : QObject(parent), m_basePath(Obsidian::getDir("views")), m_resolver(m_basePath.absolutePath()) {
	auto conf = Obsidian::getConfig("server");
	if(conf.contains("cacheSize"))
		m_resolver.setMaxCost(conf.value("cacheSize").toInt());
}

QByteArray MustacheManager::render(const QString& name, const QVariantHash& params) {
	Mustache::QtVariantContext context(params, &m_resolver);
	return m_renderer.render(m_resolver.getPartial(name), &context).toUtf8();
}

bool MustacheManager::has(const QString& key) const {
	return QFile(m_basePath.filePath(key + ".mustache")).exists();
}
