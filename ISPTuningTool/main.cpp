#include "qmainwnd.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 设置当前工作目录为程序所在目录
    QMainWnd w;
    w.initWnd();
    w.setWindowIcon(QIcon(QCoreApplication::applicationDirPath() + "\\FlyingChip.png")); // 设置图标
    w.show();
    return a.exec();
}
