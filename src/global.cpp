#include "global.hpp"

QDir getDir(QString name) {
	auto dir = QDir(qApp->applicationDirPath());

#ifdef QT_DEBUG
#if defined(Q_OS_WIN)
	if (dir.dirName().toLower() == "debug" || dir.dirName().toLower() == "release") {
		dir.cdUp();
		dir.cdUp();
		dir.cdUp();
	}
#elif defined(Q_OS_MAC)
	if (dir.dirName() == "MacOS") {
		dir.cdUp();
	}
#endif
	//dir.cd("build");
	dir.cd("site");
#endif

	dir.cd(name);
	return dir;
}

QJsonObject getConfig(QString name) {
	QFile cfg(getDir("config").absoluteFilePath(name + ".json"));

	cfg.open(QIODevice::ReadOnly | QIODevice::Text);
	auto data = cfg.readAll();
	cfg.close();

	return QJsonDocument::fromJson(data).object();
}
