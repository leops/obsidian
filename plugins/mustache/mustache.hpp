#ifndef PHP_HPP
#define PHP_HPP

#include <view.hpp>
#include <parser.h>

class MustacheManager : public QObject, public ViewManager {
	Q_OBJECT
	Q_INTERFACES(ViewManager)
	Q_PLUGIN_METADATA(IID "com.obsidian.ViewManager" FILE "mustache.json")

	public:
		explicit MustacheManager(QObject* parent = nullptr);
		virtual QByteArray render(const QString& name, const QVariantHash& params);
		virtual bool has(const QString &) const;

	private:
		QDir m_basePath;
		Mustache::PartialFileLoader m_resolver;
		Mustache::Renderer m_renderer;
};

#endif // PHP_HPP

