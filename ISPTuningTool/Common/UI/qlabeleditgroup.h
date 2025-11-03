#ifndef QLABELEDITGROUP_H
#define QLABELEDITGROUP_H

#include "Common/UI/qcommonuiinterface.h"
#include "DatabaseOrm/databaseorm.h"
#include <QEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>
#include <QWidget>

using namespace DatabaseOrm;

namespace Ui {
class QLabelEditGroup;
}

class QLabelEditGroup : public QCommonUiInterface
{
    Q_OBJECT

public:
    explicit QLabelEditGroup(QWidget *parent = nullptr);
    ~QLabelEditGroup();
    void                          init();
    void                          initConnect();
    void                          setValue(QString editstr);
    void                          setLabelValue(QString labelstr);
    void                          setEditWidth(int width);
    void                          setlabelWidth(int width);
    QString                       Label();
    QString                       value();
    void                          getValue(QString &labelstr, QString &editstr);
    void                          setEditable(bool flag);
    void                          setMouseTracking(bool flag) override;
    void                          setLabelFont(QFont font);
    void                          setSensorValue(uint32_t value);
    void                          getSensorValue(uint32_t &value);
    void                          setCombineRegister(QString uiModuleName);
    void                          setCombineRegister(CombinedRegister reg);
    std::vector<CombinedRegister> getCombineRegister();

    void     setValue(uint32_t value) override;
    uint32_t getValue() override;
    void     setLabelVisible(bool isVisible);
    void     setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value) override;
    void     setUiDataMiddleware(const QUiDataMiddleware &value) override;
    void     setSpacing(int spacing);
signals:
    void signalValueChange(uint32_t);
    void release();
    void releaseLabel(QString);
    void floatValueChange(float);
    void releaseUpadteCombineReg(std::vector<CombinedRegister>);

public slots:
    void recvValueChanged();

protected:
    void         mouseReleaseEvent(QMouseEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event) override;

private:
    std::vector<CombinedRegister> combineReg;
    Ui::QLabelEditGroup          *ui;
    std::shared_ptr<QLabel>       label           = nullptr;
    QPointer<QLineEdit>           lineEdit        = nullptr;
    bool                          isMouseTracking = false;
};

#endif // QLABELEDITGROUP_H
