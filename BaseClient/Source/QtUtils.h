#ifndef QTUTILS_HEADER_
#define QTUTILS_HEADER_

#include <QObject>
#include <QString>
#include <QStringList>

#include <string>

template<typename T>
T findParent(QObject* obj)
{
	while (obj) {
		T p = qobject_cast<T>(obj->parent());
		if (p) {
			return p;
		}
		obj = obj->parent();
	}

	return 0;
}

inline std::string toLocal8bit(const char* s)
{
	return QString(s).toLocal8Bit().data();
}

inline std::string toLocal8bit(const std::string& s)
{
	return QString(s.c_str()).toLocal8Bit().data();
}

inline std::string toLocal8bit(const QString& s)
{
	return s.toLocal8Bit().data();
}

inline std::string fromLocal8bit(const std::string& s)
{
	return QString::fromLocal8Bit(s.c_str()).toStdString();
}

inline QStringList parseCombinedArgString(const QString& program)
{
	QStringList args;
	QString tmp;
	int quoteCount = 0;
	bool inQuote = false;

	// handle quoting. tokens can be surrounded by double quotes
	// "hello world". three consecutive double quotes represent
	// the quote character itself.
	for (int i = 0; i < program.size(); ++i) {
		if (program.at(i) == QLatin1Char('"')) {
			++quoteCount;
			if (quoteCount == 3) {
				// third consecutive quote
				quoteCount = 0;
				tmp += program.at(i);
			}
			continue;
		}
		if (quoteCount) {
			if (quoteCount == 1)
				inQuote = !inQuote;
			quoteCount = 0;
		}
		if (!inQuote && program.at(i).isSpace()) {
			if (!tmp.isEmpty()) {
				args += tmp;
				tmp.clear();
			}
		}
		else {
			tmp += program.at(i);
		}
	}
	if (!tmp.isEmpty())
		args += tmp;

	return args;
}

#endif // QTUTILS_HEADER_

