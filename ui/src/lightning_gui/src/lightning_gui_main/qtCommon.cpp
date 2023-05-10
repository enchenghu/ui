#include "qtCommon.hpp"

void setReadOnlyLineEdit(QLineEdit* line)
{
	line->setReadOnly(true);
	QPalette palette = line->palette();
	palette.setBrush(QPalette::Base,palette.brush(QPalette::Disabled, QPalette::Base));
	line->setPalette(palette);
}

void setLED(QLabel* label, LED_COLOR color)
{
    // 将label中的文字清空
	int size = 20;
    label->setText("");
    // 先设置矩形大小
    // 如果ui界面设置的label大小比最小宽度和高度小，矩形将被设置为最小宽度和最小高度；
    // 如果ui界面设置的label大小比最小宽度和高度大，矩形将被设置为最大宽度和最大高度；
    QString min_width = QString("min-width: %1px;").arg(size);              // 最小宽度：size
    QString min_height = QString("min-height: %1px;").arg(size);            // 最小高度：size
    QString max_width = QString("max-width: %1px;").arg(size);              // 最小宽度：size
    QString max_height = QString("max-height: %1px;").arg(size);            // 最小高度：size
    // 再设置边界形状及边框
    QString border_radius = QString("border-radius: %1px;").arg(size/2);    // 边框是圆角，半径为size/2
    QString border = QString("border:1px solid black;");                    // 边框为1px黑色
    // 最后设置背景颜色
    QString background = "background-color:";
    switch (color) {
    case C_GRAY:
        // 灰色
        background += "rgb(190,190,190)";
        break;
    case C_RED:
        // 红色
        background += "rgb(255,0,0)";
        break;
    case C_GREEN:
        // 绿色
        background += "rgb(0,255,0)";
        break;
    case C_YELLOW:
        // 黄色
        background += "rgb(255,255,0)";
        break;
    default:
        break;
    }

    const QString SheetStyle = min_width + min_height + max_width + max_height + border_radius + border + background;
    label->setStyleSheet(SheetStyle);
}

void setCheckBoxUnvaild(QCheckBox* checkBox, bool check)
{
	checkBox->setCheckable(check);
	QPalette palette = checkBox->palette();
	if(!check)
		palette.setBrush(QPalette::Base,palette.brush(QPalette::Inactive, QPalette::Base));
	else
		palette.setBrush(QPalette::Base,palette.brush(QPalette::Active, QPalette::Base));

	checkBox->setPalette(palette);
	checkBox->setStyleSheet("QCheckBox::indicator {width: 20px; height: 20px;}");	
}

void setCheckBoxUnvaild(QCheckBox* checkBox)
{
	checkBox->setCheckable(false);
	QPalette palette = checkBox->palette();
	palette.setBrush(QPalette::Base,palette.brush(QPalette::Disabled, QPalette::Base));
	checkBox->setPalette(palette);
	checkBox->setStyleSheet("QCheckBox::indicator {width: 1px; height: 1px;}");	
}

void setButtonStyle(QPushButton* btn)
{
	if(!btn) return;
	btn->setStyleSheet("QPushButton{background-color:rgba(192, 192, 192, 100);}"
	//"QPushButton:hover{background-color:rgba(0, 255, 0, 100);border:2px solid black;border-radius:10px;}"
	"QPushButton:pressed{background-color:rgba(127, 255, 0, 100);}");
}