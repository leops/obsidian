#include "view.hpp"

View::View(QObject *parent) : QObject(parent) {

}

QByteArray View::render(const QString& name, const QScriptValue& params) {
	QFile view(getDir("views").absoluteFilePath(name + ".html"));
	view.open(QIODevice::ReadOnly | QIODevice::Text);
	QString data = view.readAll();
	view.close();

	QScriptValueIterator it(params);
	while(it.hasNext()) {
		it.next();
		QRegExp match(QString("<\\?=\\s?%1\\s?\\?>").arg(it.name()));
		data.replace(match, toString(it.value()));
	}

	return data.toUtf8();
}

