#ifndef QTUTILS_HEADER_
#define QTUTILS_HEADER_

#include <QObject>
#include <QString>

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

#endif // QTUTILS_HEADER_

