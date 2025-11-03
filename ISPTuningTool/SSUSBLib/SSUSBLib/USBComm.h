#include <vector>
#include <string>
#include "libusb.h"
#ifdef _WIN32
#include <initguid.h>
#include <SetupAPI.h>
extern "C"
{
#include <hidsdi.h>
}
#endif
using namespace std;

#define LOG		0	//输出log日志信息

#ifndef _USB_COMM_H
#define _USB_COMM_H

#define SSUSBDEV_PID	0x0040
#define SSUSBDEV_VID    0xC200


#define SSUSBDEV_CMDEPIN      (2 | 0x80)
#define SSUSBDEV_CMDEPOUT     (2 | 0x00)

#define SSUSBDEV_IMGEPIN      (1 | 0x80)
#define SSUSBDEV_DEBUGIN      (4 | 0x80)

#define SSUSBDEV_INTERFACE	  (0)

#define MAX_PACKET_PAYLOAD_SIZE (1024-24)

#define MAX_ATTEMPT_COUNT    3

#define USBDATA_BUFF_SIZE	(21*1024*1024)

#define USB_IMGRCV_BUFF_SIZE (64*1024)
#define USB_TRANSFER_NUM	(32)

#define MAXD_EVICE_SUM 8 //Maximum enumerable device
#define	DEVICE_SUM		2	//only support 2 device
/* USB vendor request to read the 8 byte firmware ID. This will return content
 * of glFirmwareID array. */
#define CY_FX_RQT_ID_CHECK                      (0xB0)

 /* USB vendor request to write to I2C EEPROM connected. The EEPROM page size is
  * fixed to 64 bytes. The I2C EEPROM address is provided in the value field. The
  * memory address to start writing is provided in the index field of the request.
  * The maximum allowed request length is 4KB. */
#define CY_FX_RQT_I2C_EEPROM_WRITE              (0xBA)

  /* USB vendor request to read from I2C EEPROM connected. The EEPROM page size is
   * fixed to 64 bytes. The I2C EEPROM address is provided in the value field. The
   * memory address to start reading from is provided in the index field of the
   * request. The maximum allowed request length is 4KB. */
#define CY_FX_RQT_I2C_EEPROM_READ               (0xBB)

   /* USB vendor request to read data from SYS_MEM starting at 32-bit aligned given
	* address. The MS 16-bit of start address is provided in the index field and the
	* LS 16-bit is provided in the value field of the request. */
#define CY_FX_RQT_SYS_MEM_READ                  (0xC0)

	/* USB vendor request to write data to SPI flash connected. The flash page size is
	 * fixed to 256 bytes. The memory address to start the write is provided in the
	 * index field of the request. The maximum allowed request length is 4KB. */
#define CY_FX_RQT_SPI_FLASH_WRITE               (0xC2)

	 /* USB vendor request to read data from SPI flash connected. The flash page size is
	  * fixed to 256 bytes. The memory address to start the read is provided in the index
	  * field of the request. The maximum allowed request length is 4KB. */
#define CY_FX_RQT_SPI_FLASH_READ                (0xC3)

	  /* USB vendor request to erase a sector on SPI flash connected. The flash sector
	   * size is fixed to 64KB. The sector address is provided in the index field of
	   * the request. The erase is carried out if the value field is non-zero. If this
	   * is zero, then the request returns the write in progress (WIP) bit. WIP should
	   * be 0 before issuing any further transactions. */
#define CY_FX_RQT_SPI_FLASH_ERASE_POLL          (0xC4)

	   /* USB vendor request to get the firmware version. */
#define CY_FX_RQT_GET_FW_VERSION                (0xC8)

/* USB Request Class types */
#define CY_U3P_USB_TYPE_MASK                    (0x60)     /* The request type mask */
#define CY_U3P_USB_STANDARD_RQT                 (0x00)     /* The USB standard request */
#define CY_U3P_USB_CLASS_RQT                    (0x20)     /* The USB class request */
#define CY_U3P_USB_VENDOR_RQT                   (0x40)     /* The USB vendor request */
#define CY_U3P_USB_RESERVED_RQT                 (0x60)     /* The USB reserved request */

#define CHAR_MAX_LEN 256
#define CURRENT_SIZE 16	//每路电流大小占4Bytes，共4路,16Bytes

enum
{
	CMD_READVERINFO = 0x00,
	CMD_SENSORINTCFG = 0x01,
	CMD_WRITESENSORREG = 0x02,
	CMD_READSENSORREG = 0x03,
	CMD_GETFRAME = 0x04,
	CMD_IOEXPIOCFG = 0x05,
	CMD_IOEXPIOVALUE = 0x06,
	CMD_WRITEFPGAREG = 0x07,
	CMD_READFPGAREG = 0x08,
	CMD_DEBUGINFO = 0x09,
	CMD_RESETSESNOR = 0x0A,
	CMD_WRITEEXPIOREG = 0x10,
	CMD_READEXPIOREG = 0x11,
	CMD_READCURRENT = 0x12,
	CMD_USB2I2CWRITEDISCRETE = 0xE1,
	CMD_USB2I2CREADDISCRETE = 0xE2,
	CMD_USB2I2CCFG = 0xF0,
	CMD_USB2I2CWRITE = 0xF1,
	CMD_USB2I2CREAD = 0xF2,
	CMD_RESETFX3 = 0xF3,
	CMD_GETVERSIONFX3 = 0XF4,
	CMD_POWERCFG = 0xF5,
	CMD_CLOCKFLAG = 0xF6,
	CMD_CURRENTTEST = 0xF7,
	CMD_PWDNADC = 0xF8,
	CMD_I2CCONFIG = 0xF9,
	CMD_POWERTEST=0xFA,
	//CMD_CHANGEIICBUS = 0xFB,
	//CMD_FX3FWUPDATE = 0xFC,
	CMD_USB2I2CWRITEBURST = 0xFB,
	CMD_USB2I2CREADBURST = 0xFC,
	CMD_USB2SPICONFIG = 0xFD,
	CMD_USB2SPIWRITE = 0xFE,
	CMD_USB2SPIREAD = 0xFF,
	CMD_USB2SPIWRITEBURST = 0x100,
	CMD_USB2SPIREADBURST = 0x101,
	CMD_CX3MIPICONFIG = 0xF00,
	CMD_HISPEEDSPI=0x1000000,
	CMD_MAX
};

typedef libusb_device_handle* PER_DEVICE_HANDLE;
typedef libusb_context * PER_DEVICE_CONTEXT;

typedef struct PacketCmd
{
	UINT32		VerifyCode;
	UINT32		Version;
	UINT32		Command;
	UINT32		Crc;
	UINT32		Address;
	UINT32		DataSize;
	UINT8		pData[MAX_PACKET_PAYLOAD_SIZE];
}PacketCmd_t;

struct SmartsensDev
{
	libusb_device **devs;
	PER_DEVICE_HANDLE devhandlelist[MAXD_EVICE_SUM] = { 0 };
	int *EnumAddr = NULL;
	int DevCurrent[MAXD_EVICE_SUM] = { -1 };
	int DevCnt = 0;
	unsigned char uSerialNumber[MAXD_EVICE_SUM][CHAR_MAX_LEN];
};

HANDLE* usb_dev_open(int &DevCnt);
HANDLE* usb_dev_open_addr(SmartsensDev *sdev, int DevCnt, int *DevIndexaddr, int OpenDevCnt = 1);
int usb_dev_enum(SmartsensDev *sdev);
int usb_dev_close(HANDLE* hDev);
int usb_send_cmd(HANDLE hDev, void *pBufIn, uint32_t SizeIn, void *pBufOut, uint32_t *pSizeOut);
int usb_capture_start(HANDLE hDev);
int usb_capture_stop(HANDLE hDev);
int usb_get_frame(HANDLE hDev, void* pFrame, int BufferSize, int* pFrameSize, uint32_t Timeout);

void USBDataValidCallBack0(struct libusb_transfer *pTransfer);
void USBDataValidCallBack1(struct libusb_transfer *pTransfer);
void USBDbgValidCallBack(struct libusb_transfer *pTransfer);
int libusb_open_device_with_address(libusb_context* ctx, PER_DEVICE_HANDLE* devhandlelist, int ctxIndex = 0);
int libusb_open_device_with_addressEx(libusb_device **devs, libusb_context* ctx, PER_DEVICE_HANDLE& devhandle, int ctxIndex);
void ResetGlobalResource();
int libusb_enum_devs(SmartsensDev *Sdev);
//int usb_emum_device(vector<string> &vecdevlist);

#endif // 
