#ifndef QTUTILS_HEADER_
#define QTUTILS_HEADER_

#include <QObject>

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

#endif // QTUTILS_HEADER_

