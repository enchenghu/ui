#ifndef QIMGFMTTAB_H
#define QIMGFMTTAB_H

#include "Common/UI/qlabelbutton.h"
#include "Common/UI/qlabelcheckbox.h"
#include "Common/UI/qlabelcombobox.h"
#include "Common/UI/qtabwndcomm.h"
#include "qtransfermanager.h"
#include <QAbstractItemModel>
#include <QPointer>
#include <QWidget>

class QImgFmtTab : public QTabWndComm
{
	Q_OBJECT

public:
	explicit QImgFmtTab(QWidget *parent = nullptr);
	~QImgFmtTab();
	void updateValue();
	template <typename T>
    void updateUIValue(T& vector,const QString &extraname="")
    {
        for(auto item:vector){
            uint32_t value=0;
            auto reg=combinedRegisterMap.value(item->Label()+extraname);
            for(int i=0;i<reg.size();i++){
                readCombinedRegisterValue(reg.at(i), value);
                if (item->Label() == "Width" || item->Label() == "Height")
					item->setValue(QString("%1").arg(value));
				else
                	item->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
            }
        }
    }
private:
	void init();
	void initConnect();
	void recordUIRegister();
	void recordFWUI(std::string name, std::string UIName);
	void recordHWUI(std::string name, std::string UIName, int bitmask);
	void writeHWbyBitMask(QString str, bool isChecked);
	void updateCheckBox();
	void updateCheckBoxByLabel(QString str, bool bNewValue);
	void checkBoxValueUpdate(QString strLabelValue, bool isChecked);
        void setCombinedReg(std::vector<CombinedRegister> regVector);
public slots:
	void recvDualCam(bool flag);
	void recvCamNum(bool flag);
	void recvBLC(QString str);
	void recvRGBIR(QString str);
	void recvGSHDR(bool flag);
	void recvHDRMode(QString str);
	void recvCheckBox(QString str, bool isChecked);
	void recvCheckBoxLabel(QString str);
        void recValueByHighLowBits(int value);
        // template <typename T>
   //  void updateUIValue(T& vector,const QString &extraname="")
   //  {
   //      for(auto item:vector){
                        // uint32_t value=0;

                        // if (item->Label() == "CurTemp")
                        // {
                        // 	uint16_t u16Value = 0;
                        // 	QUsbCommunication::getInstance().readBurst(0x31003910, &u16Value, ADDRESS_LENGTH, VALUE_LENGTH * 2);
                        // 	value = u16Value;
                        // 	item->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(4,'0')));
                        // 	continue;
                        // }
            
   //          auto reg=combinedRegisterMap.value(item->Label()+extraname);
   //          for(int i=0;i<reg.size();i++){
   //              readCombinedRegisterValue(reg.at(i), value);
   //              item->setValue(QString("0x%1").arg(QString::number(value,16).toUpper().rightJustified(VALUE_HEX_INFO,'0')));
   //          }
   //      }
   //  }

private:
	QPointer<QLabelButton>                     DualCam   = nullptr;
	QPointer<QLabelButton>                     CamNum    = nullptr;
	QPointer<QLabelEditGroup>                  Width     = nullptr;
	QPointer<QLabelEditGroup>                  Height    = nullptr;
	QPointer<QLabelEditGroup>                  BLC       = nullptr;
	QPointer<QLabelEditGroup>                  CurTemp   = nullptr;
	QPointer<QLabelComboBox>                   RGBIR     = nullptr;
	QPointer<QLabelButton>                     GSHDR     = nullptr;
	QPointer<QLabelComboBox>                   HDRMode   = nullptr;
	bool                                       isDualCam = false;
	bool                                       isRGBIR   = false;
	bool                                       isGSHDR   = false;
	QMap<QString, QPointer<QLabelCheckBox>>    checkBoxMap;
	QMap<QString, QStringList>                 FWAddressMap;
	QMap<QString, std::vector<std::pair<int, QString>>> HWAddressMap;
        std::vector<CombinedRegister> HDRcombinevector;
        std::vector<CombinedRegister> RGBIRcombinevector;
	uint32_t RGBIR_address_0      = 0;
	uint32_t RGBIR_address_1      = 0;
        uint32_t HDR_Mode_address     = 0;
        std::vector<QPointer<QLabelEditGroup>> Img_edit;
        std::vector<QPointer<QLabelEditGroup>> CurUpdateEdit;

};

#endif // QIMGFMTTAB_H
