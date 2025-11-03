#ifndef QLABELCHECKBOX_H
#define QLABELCHECKBOX_H

#include "Common/UI/qcommonuiinterface.h"
#include "DatabaseOrm/databaseorm.h"
#include <QCheckBox>
#include <QEvent>
#include <QLabel>
#include <QPointer>
#include <QWidget>

using namespace DatabaseOrm;

namespace Ui {
class QLabelCheckBox;
}

class QLabelCheckBox : public QCommonUiInterface
{
    Q_OBJECT

public:
    explicit QLabelCheckBox(QWidget *parent = nullptr);
    ~QLabelCheckBox();
    void                          setBold(bool isBold);
    void                          setMouseTracking(bool flag) override;
    void                          setLabel(QString str);
    void                          getLabel(QString &str);
    QString                       Label();
    void                          setCheckable(bool isCheckable);
    void                          setChecked(bool isChecked);
    void                          setSensorValue(uint32_t value);
    void                          getSensorValue(uint32_t &value);
    void                          setCombineRegister(QString uiModuleName);
    void                          setCombineRegister(CombinedRegister reg);
    std::vector<CombinedRegister> getCombineRegister();
    void                          setValue(uint32_t value) override;
    uint32_t                      getValue() override;
    bool                          checkState();
    void                          setValue(int value);
    void                          setValue(QString str);
    void                          setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value) override;
    void                          setUiDataMiddleware(const QUiDataMiddleware &value) override;
    void                          setSpacing(int spacing);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    std::vector<CombinedRegister> combineReg;
    Ui::QLabelCheckBox           *ui;
    QPointer<QLabel>              label           = nullptr;
    QPointer<QCheckBox>           checkBox        = nullptr;
    bool                          isMouseTracking = false;

    void init();
    void initConnect();

signals:
    void checkSignal(QString, bool);
    void release(QString);
    void releaseUpadteCombineReg(std::vector<CombinedRegister>);
    void boxClicked(QString, bool);

private slots:
    void recCheckBoxState(int state);
    void recCheckBoxClicked();

private:
};

#endif // QLABELCHECKBOX_H
