#ifndef QTABWNDCOMMFACTORY_H
#define QTABWNDCOMMFACTORY_H

#include "Common/UI/qtabwndcomm.h"

template <typename T>
QPointer<QTabWndComm> createTabWndCommTemplate(QWidget *parent = nullptr)
{
	return new T(parent);
}

class QTabWndCommFactory
{
public:
	static QTabWndCommFactory &instance();
	QPointer<QTabWndComm> createTabWndComm(const QString &name, QWidget *parent = nullptr);

        static void QTabWndUpdateUserLevel(uint level);
private:
	QTabWndCommFactory();
	template <typename T>
	void registerTabWndComm(const QString &name)
	{
		m_map[name] = &createTabWndCommTemplate<T>;
	}
	QMap<QString, QPointer<QTabWndComm> (*)(QWidget *parent)> m_map;
};

#endif // QTABWNDCOMMFACTORY_H
