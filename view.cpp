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

int getNext(std::initializer_list<int> tokens) {
	for(int i : tokens) {
		if(i != -1) {
			bool smaller = false;
			for(int j : tokens) {
				if(j != -1 && j < i) {
					smaller = true;
					break;
				}
			}
			if(!smaller)
				return i;
		}
	}
	return -1;
}

QByteArray OTPManager::render(const QString& name, const QVariantHash& params) {
	const QString expr = "([\\w\\.\\[\\]]+)";
	const QRegExp print("<\\?=\\s*" + expr + "\\s*\\?>");
	const QRegExp condition("<\\?\\s*if\\(\\s*" + expr + "\\s*\\)\\s*\\{\\s*\\?>");
	const QRegExp loop("<\\?\\s*for\\(\\s*(\\w+)\\s*:\\s*" + expr + "\\s*\\)\\s*\\{\\s*\\?>");
	const QRegExp variable("<\\?\\s*var\\s+(\\w+)\\s*=\\s*" + expr + "\\s*\\?>");
	const QRegExp close("<\\?\\s*([{}])\\s*\\?>");

	m_scopes.push(Scope("", params, QVariantHash()));

	QFile view(getDir("views").absoluteFilePath(name + ".html"));
	if (view.open(QIODevice::ReadOnly | QIODevice::Text)) {
		QString data = view.readAll();
		int pos = 0;

		while(pos < data.length()) {
			int nextPrint = print.indexIn(data, pos),
				nextCond = condition.indexIn(data, pos),
				nextClose = close.indexIn(data, pos),
				nextVar = variable.indexIn(data, pos),
				nextLoop = loop.indexIn(data, pos),
				next = getNext({nextPrint, nextCond, nextClose, nextVar, nextLoop});

			if(next == -1) {
				break;
			}

			std::get<0>(m_scopes.top()) += data.mid(pos, next - pos);

			if(next == nextCond) {
				auto nScope = m_scopes.top();
				std::get<0>(nScope) = "";
				std::get<2>(nScope).insert("type", "condition");
				std::get<2>(nScope).insert("value", eval(condition.cap(1), std::get<1>(nScope)));
				m_scopes.push(nScope);
				data.remove(next, condition.matchedLength());
			} else if(next == nextVar) {
				auto val = eval(variable.cap(2), std::get<1>(m_scopes.top()));
				std::get<1>(m_scopes.top()).insert(variable.cap(1), val);
				data.remove(next, variable.matchedLength());
			} else if(next == nextLoop) {
				auto copy = m_scopes.top();
				auto varName = loop.cap(1);
				auto array = qvariant_cast<QVariantList>(eval(loop.cap(2), std::get<1>(m_scopes.top())));
				for(int i = 0; i < array.length(); i++) {
					auto nScope = copy;
					std::get<0>(nScope) = "";
					std::get<1>(nScope).insert(varName, array[i]);
					std::get<2>(nScope).insert("type", "loop");
					std::get<2>(nScope).insert("start", next);
					std::get<2>(nScope).insert("last", i == 0);
					m_scopes.push(nScope);
				}
				data.remove(next, loop.matchedLength());
			} else if(next == nextPrint) {
				auto val = eval(print.cap(1), std::get<1>(m_scopes.top())).toString();
				std::get<0>(m_scopes.top()) += val;
				next += print.matchedLength();
			} else if(next == nextClose) {
				auto scope = m_scopes.pop();
				auto info = std::get<2>(scope);
				bool clear = true;
				if(info.value("type") == "condition") {
					if(info.value("value").toBool()) {
						std::get<0>(m_scopes.top()) += std::get<0>(scope);
					}
				} else if(info.value("type") == "loop") {
					std::get<0>(m_scopes.top()) += std::get<0>(scope);
					if(!info.value("last").toBool()) {
						next = info.value("start").toInt();
						clear = false;
					}
				}
				if(clear)
					data.remove(next, close.matchedLength());
			}

			pos = next;
		}

		view.close();
	}

	return std::get<0>(m_scopes.top()).toUtf8();
}

bool OTPManager::has(const QString& name) const {
	QFile view(getDir("views").absoluteFilePath(name + ".html"));
	return view.exists();
}

