#ifndef PHP_HPP
#define PHP_HPP

#include <controller.hpp>
#include <QCache>

class AssetsManager : public QObject, public ControllerManager {
	Q_OBJECT
	Q_INTERFACES(ControllerManager)
	Q_PLUGIN_METADATA(IID "com.obsidian.ControllerManager" FILE "assets.json")

	public:
		explicit AssetsManager(QObject* parent = nullptr);
		virtual QVariant execute(const Controller&, QObjectList&);
		virtual bool has(const Controller&);

	private:
		QCache<QString, QString> m_cache;
		static QHash<QString, QString> s_types;
};

#endif // PHP_HPP

