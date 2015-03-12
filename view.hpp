#ifndef VIEW_HPP
#define VIEW_HPP

#include <QObject>
#include <QScriptValue>
#include <QScriptValueIterator>
#include <QFile>
#include "global.hpp"

class View : public QObject {
	Q_OBJECT

public:
	explicit View(QObject *parent = 0);

public slots:
	virtual QByteArray render(const QString& name, const QScriptValue& params = QScriptValue());
};

#endif // VIEW_HPP
