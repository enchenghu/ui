#include "qhuegraph.h"
#include "Common/UI/qcommonuiinterface.h"
#include <QBrush>
#include <QCoreApplication>
#include <QLabel>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QPointer>

QHueGraph::QHueGraph(QWidget *parent)
    : QCommonUiInterface(parent)
    , mouseTracking(false)
    , toolTipWidget(nullptr)
    , toolTipLabel(nullptr)
{
    init();
}

void QHueGraph::setValue(uint32_t value) { }

uint32_t QHueGraph::getValue()
{
    return 0;
}

void QHueGraph::setMouseTracking(bool flag)
{
    mouseTracking = flag;
}
void QHueGraph::setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value)
{
    QCommonUiInterface::setUiDataMiddleware(value);
    if (uiDataMiddleware)
        splitCount = uiDataMiddleware->getCombinedRegisterList().size();
}

void QHueGraph::setUiDataMiddleware(const QUiDataMiddleware &value)
{
    QCommonUiInterface::setUiDataMiddleware(value);
    if (uiDataMiddleware)
        splitCount = uiDataMiddleware->getCombinedRegisterList().size();
}

bool QHueGraph::eventFilter(QObject *watched, QEvent *event)
{
    if (!isVisible() || !uiDataMiddleware || splitCount == 0) {
        toolTipWidget->hide();
        return QWidget::eventFilter(watched, event);
    }
    QPointF localPos;
    int     index;
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        int          width      = qMin(this->width(), this->height()) / 2 * 2;
        localPos                = this->mapFromGlobal(mouseEvent->globalPosition());
        auto distance = qHypot(localPos.x() - this->rect().center().x(), localPos.y() - this->rect().center().y());
        if (distance > width / 2) {
            toolTipWidget->hide();
            return QWidget::eventFilter(watched, event);
        }
        // 计算点击位置的角度
        qreal angle = 0 - qAtan2(localPos.y() - this->rect().center().y(), localPos.x() - this->rect().center().x());
        angle = qRadiansToDegrees(angle);
        angle += 360 / splitCount / 2;
        if (angle < 0) {
            angle += 360;
        }
        index = angle * splitCount / 360;
        // mouse move
        if (event->type() == QEvent::MouseMove) {
            // tooltip显示当前位置的值
            toolTipLabel->setText(QString::number(index));
            toolTipWidget->move(mouseEvent->globalPosition().x(),
                mouseEvent->globalPosition().y() - toolTipWidget->height());
            toolTipWidget->show();
            return true;
        } else if (event->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton) {
            QSharedPointer<int>         value = QSharedPointer<int>(new int(index));
            QPointer<QUiDataMiddleware> singleUiDataMiddleware =
                new QUiDataMiddleware(uiDataMiddleware->getUiModuleName(), uiDataMiddleware->getUiControlName(), value);
            emit releaseUiDataMiddleware(singleUiDataMiddleware);
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}

void QHueGraph::init()
{
    splitCount    = 0;
    toolTipWidget = new QWidget(this);
    toolTipWidget->setWindowFlags(Qt::ToolTip);
    toolTipWidget->setBackgroundRole(QPalette::ToolTipBase);
    toolTipWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    toolTipWidget->setLayout(new QHBoxLayout(toolTipWidget));
    toolTipWidget->layout()->setContentsMargins(0, 0, 0, 0);
    toolTipWidget->layout()->setSpacing(0);
    toolTipLabel = new QLabel(toolTipWidget);
    toolTipLabel->setAlignment(Qt::AlignCenter);
    toolTipLabel->setBackgroundRole(QPalette::ToolTipBase);
    toolTipWidget->layout()->addWidget(toolTipLabel);
    QCoreApplication::instance()->installEventFilter(this);
}

void QHueGraph::paintEvent(QPaintEvent *event)
{
    static const QVector<QColor> colorVec = { QColor(255, 0, 0), QColor(255, 255, 0), QColor(0, 255, 0),
        QColor(0, 255, 255), QColor(0, 0, 255), QColor(255, 0, 255) };
    QConicalGradient             conicalGradient(this->rect().center(), 0);
    qreal                        interval = 1.0 / colorVec.size();
    // 设置渐变颜色
    for (int i = 0; i < colorVec.size(); ++i) {
        conicalGradient.setColorAt(interval * i, colorVec[i]);
    }
    conicalGradient.setColorAt(1.0, colorVec[0]);
    // 绘制圆环
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(conicalGradient);
    painter.setPen(QPen(Qt::NoPen));
    int   width = qMin(this->width() - 2, this->height() - 2) / 2 * 2;
    QRect centerRect(this->rect().center().x() - width / 2, this->rect().center().y() - width / 2, width, width);
    painter.drawEllipse(centerRect);

    QRadialGradient radialGradient(this->rect().center(), width / 2, this->rect().center());
    radialGradient.setColorAt(0, QColor(255, 255, 255, 255));
    radialGradient.setColorAt(1, QColor(255, 255, 255, 0));
    painter.setBrush(radialGradient);
    painter.setPen(QPen(Qt::NoPen));
    painter.drawEllipse(centerRect);

    // 绘制边缘
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(centerRect);
}
