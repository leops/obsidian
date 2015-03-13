#include "view.hpp"

QVariant eval(QString expression, const QVariantHash& context) {
	auto path = expression.remove("]").split(QRegExp("[(\\.\\[]"));
	QVariant var;
	QVariantHash hash = context;
	QVariantList list;
	QVariantMap map;

	foreach(auto str, path) {
		if(!hash.empty()) {
			var = hash.value(str);
		} else if(!list.empty()) {
			var = list.value(str.toInt());
		} else if(!map.empty()) {
			var = map.value(str);
		} else {
			return var;
		}

		switch(var.type()) {
			case QMetaType::QVariantMap:
				map = qvariant_cast<QVariantMap>(var);
				list.clear();
				hash.clear();
				break;
			case QMetaType::QVariantHash:
				hash = qvariant_cast<QVariantHash>(var);
				list.clear();
				map.clear();
				break;
			case QMetaType::QVariantList:
				list = qvariant_cast<QVariantList>(var);
				hash.clear();
				map.clear();
				break;
			default:
				return var;
		}
	}

	return var;
}

QByteArray OTPManager::render(const QString& name, const QVariantHash& params) {
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
		data.replace(pos, len, val.toString());
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
			data.remove(end - (len + len2), len2);
		} else {
			data.remove(pos, end - pos);
		}

		pos += len;
	}

	return data.toUtf8();
}

bool OTPManager::has(const QString& name) const {
	QFile view(getDir("views").absoluteFilePath(name + ".html"));
	return view.exists();
}

