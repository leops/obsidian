#include "view.hpp"

View::View(QObject *parent) : QObject(parent) {

}

QScriptValue eval(QString expression, const QScriptValue& context) {
	auto path = expression.remove("]").split(QRegExp("[(\\.\\[]"));
	QScriptValue val = context;
	foreach(auto str, path) {
		val = val.property(str);
	}
	return val;
}

QByteArray View::render(const QString& name, const QScriptValue& params) {
	QFile view(getDir("views").absoluteFilePath(name + ".html"));
	view.open(QIODevice::ReadOnly | QIODevice::Text);
	QString data = view.readAll();
	view.close();

	QString expr = "([\\w\\.\\[\\]]+)";
	QRegExp print("<\\?=\\s*" + expr + "\\s*\\?>");
	int pos = 0;
	while ((pos = print.indexIn(data, pos)) != -1) {
		auto len = print.matchedLength();
		auto val = eval(print.cap(1), params);
		data.replace(pos, len, toString(val));
		pos += len;
	}

	QRegExp condition("<\\?\\s*if\\(\\s*" + expr + "\\s*\\)\\s*\\{\\s*\\?>");
	QRegExp close("<\\?\\s*([{}])\\s*\\?>");
	pos = 0;
	while ((pos = condition.indexIn(data, pos)) != -1) {
		int len = condition.matchedLength(),
			len2 = 0,
			end = pos,
			open = 1;

		while ((end = close.indexIn(data, end)) != -1) {
			len2 = close.matchedLength();
			end += len2;
			auto symbol = close.cap(1);
			if(symbol == "{")
				open++;
			if(symbol == "}")
				open--;
			if(open <= 0)
				break;
		}

		if(eval(condition.cap(1), params).toBool()) {
			data.remove(pos, len);
			data.remove(end - len2, len2);
		} else {
			data.remove(pos, end - pos);
		}

		pos += len;
	}

	return data.toUtf8();
}

