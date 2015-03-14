#ifndef VIEW_HPP
#define VIEW_HPP

#include <QObject>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QFile>
#include <QStack>
#include "global.hpp"

class ViewManager : public QObject {
	Q_OBJECT

public:
	explicit ViewManager(QObject* parent = nullptr) : QObject(parent) {}
	virtual QByteArray render(const QString& name, const QVariantHash& params) = 0;
	virtual bool has(const QString&) const = 0;
};

Q_DECLARE_INTERFACE(ViewManager, "com.obsidian.ViewManager")

class OTPManager : public ViewManager {
	Q_OBJECT
	Q_INTERFACES(ViewManager)

public:
	explicit OTPManager(QObject* parent = nullptr) : ViewManager(parent) {}
	virtual QByteArray render(const QString& name, const QVariantHash& params);
	virtual bool has(const QString &) const;

private:
	typedef std::tuple<QString, QVariantHash, QVariantHash> Scope;
	QStack<Scope> m_scopes;
};

#endif // VIEW_HPP
