#ifndef QLABELCOMBOBOX_H
#define QLABELCOMBOBOX_H

#include "Common/UI/qcommonuiinterface.h"
#include <QComboBox>
#include <QEvent>
#include <QFont>
#include <QLabel>
#include <QWidget>

namespace Ui {
class QLabelComboBox;
}

class QLabelComboBox : public QCommonUiInterface
{
    Q_OBJECT

public:
    explicit QLabelComboBox(QWidget *parent = nullptr);
    ~QLabelComboBox();
    void init();
    void initConnect();
    /**
     * @brief setValue 添加comboBox item
     * @param  label显示字段，comboBox 下拉列表
     * @return
     */
    void     setValue(QString lstr, QStringList cstr);
    QString  getValue(int index);
    int      getCurrentIndex();
    void     setCurrentIndex(int index);
    void     setLabel(QString str);
    void     setLabelFont(QFont font);
    void     setMouseTracking(bool flag) override;
    QString  Label();
    void     setValue(uint32_t value) override;
    uint32_t getValue() override;
    void     setUiDataMiddleware(const QPointer<QUiDataMiddleware> &value) override;
    void     setUiDataMiddleware(const QUiDataMiddleware &value) override;
    void     setTextRegValuePairList(QList<QPair<QString, uint32_t>> list);
    void setComboBoxWidth(int width);
    void     setSpacing(int spacing);
signals:
    void signalValueChange(QString);
    void signalValueChange(int);
    void indexChange(int);
    void release(QString);

public slots:
    void recvValueChanged(int index);

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    Ui::QLabelComboBox             *ui;
    std::shared_ptr<QLabel>         label           = nullptr;
    std::shared_ptr<QComboBox>      comboBox        = nullptr;
    bool                            isMouseTracking = false;
    QList<QPair<QString, uint32_t>> textRegValuePairList;
};

#endif // QLABELCOMBOBOX_H
