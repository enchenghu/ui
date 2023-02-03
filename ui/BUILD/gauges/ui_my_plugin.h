/********************************************************************************
** Form generated from reading UI file 'my_plugin.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MY_PLUGIN_H
#define UI_MY_PLUGIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Gauge
{
public:

    void setupUi(QWidget *Gauge)
    {
        if (Gauge->objectName().isEmpty())
            Gauge->setObjectName(QString::fromUtf8("Gauge"));
        Gauge->resize(40, 40);

        retranslateUi(Gauge);

        QMetaObject::connectSlotsByName(Gauge);
    } // setupUi

    void retranslateUi(QWidget *Gauge)
    {
        Gauge->setWindowTitle(QApplication::translate("Gauge", "Gauge", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Gauge: public Ui_Gauge {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MY_PLUGIN_H
