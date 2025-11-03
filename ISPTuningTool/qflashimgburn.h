#ifndef QFLASHIMGBURN_H
#define QFLASHIMGBURN_H

#include "M1FlashTypeDef.h"
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPointer>
#include <QPushButton>
#include <QStandardItemModel>
#include <QWidget>

enum class CreateFlashType {
    New,
    File,
    Device,
};

namespace Ui {
class QFlashImgBurn;
}

class PartitionInfoWidget;

class QFlashImgBurn : public QWidget
{
    Q_OBJECT

public:
    static QFlashImgBurn *instance();
    ~QFlashImgBurn();
    void     initPartitionTypeName();
    uint32_t getPartitionOffset(PartitionType type);
    void     updatePartitionMap();
    void     updateUIDataRangeMap();
    bool     allCheckWell();
    bool     checkCurrentPart(QPointer<PartitionInfoWidget> partWidget);
    int      savePartitionDataToCfgFile(PartitionType type, QString path);
    int      loadPartitionDataFromCfgFile(PartitionType type, QString path);
    int      removePartition(PartitionType type);
    void     updatePartitionInfo(PartitionType type);

private:
    explicit QFlashImgBurn(QWidget *parent = nullptr);
    void loadFlashImage();

private slots:
    void on_loadHeaderBtn_clicked();
    void on_exportHeaderButton_clicked();
    void on_releaseFlashBtn_clicked();
    void on_createFlashBtn_clicked();
    void on_createFlashFromFileBtn_clicked();
    void on_createFlashFromDeviceBtn_clicked();
    void on_saveFlashBtn_clicked();
    void on_burnFlashBtn_clicked();
    void on_autoAdjustBtn_clicked();
    void on_addPartitionBtn_clicked();

private:
    Ui::QFlashImgBurn                                 *ui;
    static QFlashImgBurn                              *_instance;
    QMap<PartitionType, QPointer<PartitionInfoWidget>> partitionInfoMap;
    QString                                            configLoadPath;
    uint32_t                                           deviceIndex  = 0;
    uint32_t                                           voidDataSize = 0;
    FlashImg                                           _flashImg    = nullptr;
    PartitionMap                                       partitionMap;
    std::vector<PartitionType>                         defaultPartitionOrder;
    QMap<PartitionType, QString>                       partitionTypeName;
    QMap<PartitionType, std::pair<uint32_t, uint32_t>> partitionDataRangeMap;

    void createFlash(CreateFlashType type);
    void updateFlashBtnStatus();
};
class PartitionInfoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PartitionInfoWidget(QString partitionName = "", PartitionInfo info = PartitionInfo(),
        QWidget *parent = nullptr);
    ~PartitionInfoWidget();
    void          initGroupBox(PartitionInfo info);
    PartitionInfo partitionInfo();
    void          setPartitionInfo(PartitionInfo newPartition);

    uint32_t partitionOffSet();
    void     setPartitionOffSet(uint32_t val);

    void getInfo();
    void updateData();

private:
    QPointer<QGroupBox>   groupBox       = nullptr;
    QPointer<QLabel>      offsetlabel    = nullptr;
    QPointer<QLabel>      lengthlabel    = nullptr;
    QPointer<QLabel>      sizelabel      = nullptr;
    QPointer<QLineEdit>   offsetEdit     = nullptr;
    QPointer<QLineEdit>   lengthEdit     = nullptr;
    QPointer<QLineEdit>   sizeEdit       = nullptr;

    QPointer<QLabel>      configLabel     = nullptr;
    QPointer<QLineEdit>   configPathEdit  = nullptr;
    QPointer<QPushButton> exportConfigBtn = nullptr;
    QPointer<QPushButton> loadConfigBtn   = nullptr;

    QPointer<QPushButton> removeBtn = nullptr;

    PartitionInfo c_partitionInfo;
    uint32_t      offSet     = 0;
    uint32_t      dataLength = 0;
    uint32_t      size       = 0;
};

#endif // QFLASHIMGBURN_H
