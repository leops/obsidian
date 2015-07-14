#ifndef VIEW_HPP
#define VIEW_HPP

#include <QObject>
#include <QFile>
#include <QStack>
#include <tuple>
#include "global.hpp"

class ViewManager {
	public:
		virtual QByteArray render(const QString& name, const QVariantHash& params) = 0;
		virtual bool has(const QString&) const = 0;
};

Q_DECLARE_INTERFACE(ViewManager, "com.obsidian.ViewManager")

/*class OTPManager : public ViewManager {
	Q_OBJECT
	Q_INTERFACES(ViewManager)

public:
	explicit OTPManager(QObject* parent = nullptr) : ViewManager(parent) {}
	virtual QByteArray render(const QString& name, const QVariantHash& params);
	virtual bool has(const QString &) const;

protected:
	QString parse(const QString& data, const QVariantHash& context);
	QVariant eval(QString expression, const QVariantHash& context);
	int getNext(std::initializer_list<int> tokens);

private:
	typedef std::tuple<QString, QVariantHash, QVariantHash> Scope;
};*/

#endif // VIEW_HPP
