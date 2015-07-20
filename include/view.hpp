#ifndef VIEW_HPP
#define VIEW_HPP

#include <QObject>
#include <QFile>
#include <QStack>
#include <tuple>

class ViewManager {
	public:
		virtual QByteArray render(const QString& name, const QVariantHash& params) = 0;
		virtual bool has(const QString&) const = 0;
};

Q_DECLARE_INTERFACE(ViewManager, "com.obsidian.ViewManager")

#endif // VIEW_HPP
