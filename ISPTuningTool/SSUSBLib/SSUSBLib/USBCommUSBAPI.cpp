#include "stdafx.h"
#include "USBComm.h"
#include <stdio.h>
#include <stdlib.h>
#include "libusb.h"
#include "BufferPool.h"
#include <memory>
#include <sstream>
#include <thread>
#include <future>
#ifdef _WIN32
#include <process.h>
#endif
#pragma comment (lib,"libusb-1.0.lib")

static unsigned __stdcall ThreadMain0(void *pParam);
static unsigned __stdcall ThreadMain1(void *pParam);

int ImgLen[DEVICE_SUM] = { 1280 * 1080 };
int ImgbufferLen[DEVICE_SUM] = { 1280 * 1080 };
int actSize[DEVICE_SUM] = { 0 };//sum(usb packet size)
HANDLE gHandleList[DEVICE_SUM] = { 0 };//each handle pointer each device 
uint32_t gAdrress[DEVICE_SUM] = { 0 };
int gDevCnt = 0;//emumerater device count
int gLibMode = 0;//Lib工作模式
BOOL bDataContinue = TRUE;//serdes Car Pro 用于标记帧结束，UVC传输协议
BYTE bUVCHeader = 0, bUVCHeader_last = 0;//serdes Car Pro 用于记录前后帧中 包头数据（正常情况下，一帧中所有小包的包头数据相同），UVC传输协议

//test s
#if LOG
int pacCnt = 0;
int gFrameCnt = 0;
int gSaveCnt = 10;
#endif
//test e

typedef struct LIBUSBContex
{
	HANDLE hDev[DEVICE_SUM];
 	PER_DEVICE_CONTEXT hDevCtx[DEVICE_SUM];
//	PER_DEVICE_CONTEXT hDevCtx;
	CBufferPool *pBufferPool[DEVICE_SUM];
	std::thread hThread[DEVICE_SUM];
	int nWatch[DEVICE_SUM];
	uint8_t* pUSBRecvBuffer[DEVICE_SUM][USB_TRANSFER_NUM];
	struct libusb_transfer *pUSBTransfer[DEVICE_SUM][USB_TRANSFER_NUM];
	uint8_t* pUSBDbgBuff;
	struct libusb_transfer *pUSBDbgTransfer;
}LIBUSBContex_t;

static LIBUSBContex_t s_SSUSBCtx = { 0 };
static uint8_t* pBuffAddr[DEVICE_SUM] = {0};

void ResetGlobalResource()
{
	for (int i=0; i<DEVICE_SUM; i++)
	{
		ImgLen[i] = 1280 * 1080;
		actSize[i] = 0;
		pBuffAddr[i] = NULL;
		gHandleList[i] = NULL;
	}
	memset(&s_SSUSBCtx, 0, sizeof(LIBUSBContex_t));
	gDevCnt = 0;
}
HANDLE* usb_dev_open(int &DevCnt)
{	
	PER_DEVICE_HANDLE dev_handle[DEVICE_SUM] = { 0 };

	int nRet = -1;
	nRet = libusb_init(&s_SSUSBCtx.hDevCtx[0]);
	if (nRet < 0)
	{
		return NULL;
	}
	DevCnt = libusb_open_device_with_address(s_SSUSBCtx.hDevCtx[0], dev_handle, 0);

	gHandleList[0] = dev_handle[0];
	if (DevCnt == 0)//no device 
	{
		libusb_close(dev_handle[0]);
		libusb_exit(s_SSUSBCtx.hDevCtx[0]);
		return NULL;
	}


	//two device open this block s
// 	nRet = -1;
// 	nRet = libusb_init(&s_SSUSBCtx.hDevCtx[1]);
// 	if (nRet == 0)
// 	{
// 		int Cnt1 = libusb_open_device_with_address(s_SSUSBCtx.hDevCtx[1], dev_handle, 1);
// 		if (Cnt1 > 0)
// 		{
// 			gHandleList[1] = dev_handle[1];
// 			DevCnt ++;
// 		}
// 	}
	//two device open this block e

	//获取设备描述符
	int len = 0;
	unsigned char data[20];
	nRet = libusb_get_descriptor(dev_handle[0], LIBUSB_DT_STRING, 3, data, 20);

	//one device open this block s
	if (DevCnt == DEVICE_SUM)
	{
		nRet = libusb_init(&s_SSUSBCtx.hDevCtx[1]);
		DevCnt = libusb_open_device_with_address(s_SSUSBCtx.hDevCtx[1], dev_handle, 1);
		gHandleList[1] = dev_handle[1];
	}
	//one device open this block e

	gDevCnt = DevCnt;

	if (!dev_handle[0] && !dev_handle[1])
	{
		libusb_close(dev_handle[0]);
		libusb_exit(s_SSUSBCtx.hDevCtx[0]);
		libusb_close(dev_handle[1]);
		libusb_exit(s_SSUSBCtx.hDevCtx[1]);
		return NULL;
	}
	else 
	{
		for (int i=0; i<DevCnt; i++)
		{
			if (dev_handle[i])
			{
				nRet = libusb_claim_interface(dev_handle[i], SSUSBDEV_INTERFACE);
				if (nRet < 0)
				{
					libusb_close(dev_handle[i]);
				}
				nRet = libusb_set_interface_alt_setting(dev_handle[i], SSUSBDEV_INTERFACE , 0);
				s_SSUSBCtx.hDev[i] = (HANDLE)dev_handle[i];


				for (int j = 0; j < USB_TRANSFER_NUM; j++)
				{
					if (s_SSUSBCtx.pUSBRecvBuffer[i][j] == NULL)
					{
						s_SSUSBCtx.pUSBRecvBuffer[i][j] = new uint8_t[USB_IMGRCV_BUFF_SIZE];
					}

					if (s_SSUSBCtx.pUSBTransfer[i][j] == NULL)
					{
						s_SSUSBCtx.pUSBTransfer[i][j] = libusb_alloc_transfer(0);
					}
				}
			}

		}
	}
 	
	return (HANDLE*)gHandleList;	
}

HANDLE* usb_dev_open_addr(SmartsensDev *sdev, int DevCnt, int *DevIndexaddr, int OpenDevCnt)
{
	int nRet = -1;
	if (DevCnt <= 0)//no device 
		return NULL;
	if (DevCnt < OpenDevCnt)//Number of devices less than open number
		return NULL;

	for (int devindex = 0; devindex < OpenDevCnt; devindex++)
	{
		nRet = libusb_init(&s_SSUSBCtx.hDevCtx[devindex]);
		if (nRet < 0)
			return NULL;

		nRet = libusb_open_device_with_addressEx(sdev->devs, s_SSUSBCtx.hDevCtx[devindex], sdev->devhandlelist[devindex],DevIndexaddr[devindex]);
		if (nRet < 0)
			return NULL;//打开设备失败

		gHandleList[devindex] = sdev->devhandlelist[devindex];

		//获取设备描述符
		int len = 0;
		unsigned char data[20];
		nRet = libusb_get_descriptor(sdev->devhandlelist[devindex], LIBUSB_DT_STRING, 3, data, 20);

		if (!sdev->devhandlelist[devindex])
		{
			libusb_close(sdev->devhandlelist[devindex]);
			libusb_exit(s_SSUSBCtx.hDevCtx[devindex]);
			return NULL;
		}
		
		nRet = libusb_claim_interface(sdev->devhandlelist[devindex], SSUSBDEV_INTERFACE);
		if (nRet < 0)
		{
			libusb_close(sdev->devhandlelist[devindex]);
		}
		nRet = libusb_set_interface_alt_setting(sdev->devhandlelist[devindex], SSUSBDEV_INTERFACE, 0);
		s_SSUSBCtx.hDev[devindex] = (HANDLE)sdev->devhandlelist[devindex];


		for (int j = 0; j < USB_TRANSFER_NUM; j++)
		{
			if (s_SSUSBCtx.pUSBRecvBuffer[devindex][j] == NULL)
			{
				s_SSUSBCtx.pUSBRecvBuffer[devindex][j] = new uint8_t[USB_IMGRCV_BUFF_SIZE];
			}

			if (s_SSUSBCtx.pUSBTransfer[devindex][j] == NULL)
			{
				s_SSUSBCtx.pUSBTransfer[devindex][j] = libusb_alloc_transfer(0);
			}
		}

	}


	gDevCnt = DevCnt;
	return (HANDLE*)gHandleList;
}

int usb_dev_enum(SmartsensDev *sdev)
{
	int r;

	//libusb_device *dev;
	//uint32_t gAdrress[8] = { 0 };
	ssize_t cnt;

	r = libusb_init(NULL);
	if (r < 0)
		return r;

	cnt = libusb_get_device_list(NULL, &(sdev->devs));
	if (cnt < 0)
		return (int)cnt;

	libusb_enum_devs(sdev);
	libusb_free_device_list(sdev->devs, 1);

	libusb_exit(NULL);
	return 0;
}


int usb_dev_close(HANDLE* hDev)
{
	for (int i=0; i<gDevCnt; i++)
	{ 
		if (hDev[i])
		{
			libusb_close((PER_DEVICE_HANDLE)hDev[i]);
			if (s_SSUSBCtx.pBufferPool[i])
			{
				delete s_SSUSBCtx.pBufferPool[i];
				s_SSUSBCtx.pBufferPool[i] = NULL;
			}

			for (int j = 0; j < USB_TRANSFER_NUM; j++)
			{
				if (s_SSUSBCtx.pUSBTransfer[i][j] != NULL)
				{
					libusb_free_transfer(s_SSUSBCtx.pUSBTransfer[i][j]);
					s_SSUSBCtx.pUSBTransfer[i][j] = NULL;
				}

				if (s_SSUSBCtx.pUSBRecvBuffer[i][j] != NULL)
				{
					delete[] s_SSUSBCtx.pUSBRecvBuffer[i][j];
					s_SSUSBCtx.pUSBRecvBuffer[i][j] = NULL;
				}
			}

			libusb_exit(s_SSUSBCtx.hDevCtx[i]);
		}
	}
	
// 	if (gDevCnt == 1)//init twice, so exit twice
// 	{
// 		libusb_exit(s_SSUSBCtx.hDevCtx[1]);
// 	}

	ResetGlobalResource();
	return 0;
}

int usb_send_cmd(HANDLE hDev, void *pBufIn, uint32_t SizeIn, void *pBufOut, uint32_t *pSizeOut)
{
	int nRet = -1;
	uint32_t AttemptCount;
	static uint8_t VerifyCode = 0;
	PacketCmd_t *pCMD;
	int data_written = 0;

	if (hDev == NULL || pBufIn == NULL || pBufOut == NULL || pSizeOut == NULL)
	{
		return FALSE;
	}
	pCMD = (PacketCmd_t*)pBufIn;
	pCMD->VerifyCode = VerifyCode++;

	nRet = libusb_bulk_transfer((libusb_device_handle*)hDev, SSUSBDEV_CMDEPOUT, (uint8_t*)pBufIn, SizeIn, &data_written, 1000);

	if (nRet != 0)
	{
// 		printf("OUT Transfer USB ERROR(%s)!\n", libusb_strerror((libusb_error)nRet));

// 		AttemptCount = 0;
// 		while (AttemptCount++ < MAX_ATTEMPT_COUNT)
// 		{
// 			nRet = libusb_bulk_transfer((libusb_device_handle*)hDev, SSUSBDEV_CMDEPIN, (uint8_t*)pBufOut, (int)*pSizeOut, (int*)pSizeOut, 1000);
// 
// 			if (nRet != 0)
// 			{
// 				// 			printf("IN Transfer USB ERROR(%s)!\n", libusb_strerror((libusb_error)nRet));
// 				continue;
// 			}
// 
// 			if (memcmp(pBufIn, pBufOut, 3) == 0)
// 			{
// 				break;
// 			}
// 			else
// 			{
// 				nRet = -1;
// 			}
// 		}

	}

	AttemptCount = 0;
	while (AttemptCount++ < MAX_ATTEMPT_COUNT)
	{
		nRet = libusb_bulk_transfer((libusb_device_handle*)hDev, SSUSBDEV_CMDEPIN, (uint8_t*)pBufOut, (int)*pSizeOut, (int*)pSizeOut, 1000);

		if (nRet != 0)
		{
			// 			printf("IN Transfer USB ERROR(%s)!\n", libusb_strerror((libusb_error)nRet));
			continue;
		}

		if (memcmp(pBufIn, pBufOut, 3) == 0)
		{
			break;
		}
		else
		{
			nRet = -1;
		}
	}

	return nRet;
}

int usb_capture_start(HANDLE hDev)
{	
	for (int i=0; i<gDevCnt; i++)
	{
		if (hDev == gHandleList[i])
		{
			s_SSUSBCtx.nWatch[i] = 1;
			s_SSUSBCtx.pBufferPool[i] = new CBufferPool(NULL, 5 * (ImgbufferLen[i]), 5);
			if (!s_SSUSBCtx.pBufferPool[i])
			{
				libusb_close((PER_DEVICE_HANDLE)gHandleList[i]);
				printf("error!");
			}
			if (i==0)
			{
				// s_SSUSBCtx.hThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadMain0, (void*)&s_SSUSBCtx, 0, NULL);
				s_SSUSBCtx.hThread[i] = std::thread(ThreadMain0, (void*)&s_SSUSBCtx);
			}
			else
			{
				// s_SSUSBCtx.hThread[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadMain1, (void*)&s_SSUSBCtx, 0, NULL);
				s_SSUSBCtx.hThread[i] = std::thread(ThreadMain1, (void*)&s_SSUSBCtx);
			}
			if (!s_SSUSBCtx.hThread[i].joinable())
			{
				return -2;
			}
			if (i == 1)
			{
				#ifdef _WIN32
				SetPriorityClass(s_SSUSBCtx.hThread[i].native_handle(), HIGH_PRIORITY_CLASS);
				#endif
			}
			else
			{
				#ifdef _WIN32
				SetPriorityClass(s_SSUSBCtx.hThread[i].native_handle(), HIGH_PRIORITY_CLASS);
				#endif
			}
			break;
		}
	}

	return 0;
}

int usb_capture_stop(HANDLE hDev)
{
	if ((!hDev))
	{
		return -1;
	}

// 	UINT iWaitCnt = 0;
#ifdef _WIN32
	UINT iStartTime = GetTickCount();
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	UINT iStartTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
	for (int i=0; i<gDevCnt; i++)
	{
		if (hDev == gHandleList[i])
		{
			s_SSUSBCtx.nWatch[i] = 0;

			if (!s_SSUSBCtx.hThread[i].joinable())
			{
				return -1;
			}
			else
			{
				std::async(&std::thread::join, &s_SSUSBCtx.hThread[i]).wait_for(std::chrono::seconds(10));
				// terminate thread
#ifdef _WIN32
				TerminateThread(s_SSUSBCtx.hThread[i].native_handle(), 0);
#else
				if (s_SSUSBCtx.hThread[i].joinable())
				{
					s_SSUSBCtx.hThread[i].detach();
				}
#endif
			}

			break;
		}
	}
	//s_SSUSBCtx.nWatch = 0;

	return 0;
}


static unsigned __stdcall ThreadMain0(void *pParam)
{
	LIBUSBContex_t *pThis = (LIBUSBContex_t*)pParam;
	int nRet = 0;
	int read_size = 0;
	uint32_t totalsize = 0;
	uint32_t packetcnt = 0;

	if (!pBuffAddr[0] && s_SSUSBCtx.pBufferPool[0])
	{
		s_SSUSBCtx.pBufferPool[0]->LockBackFreeItem((void**)&pBuffAddr[0], ImgbufferLen[0]);
	}

	for (int i = 0; i < USB_TRANSFER_NUM; i++)
	{
		if (gLibMode == 0x0f)//车载数据处理
		{
			libusb_fill_bulk_transfer(pThis->pUSBTransfer[0][i],
				(libusb_device_handle*)pThis->hDev[0],
				SSUSBDEV_IMGEPIN, pThis->pUSBRecvBuffer[0][i],
				USB_IMGRCV_BUFF_SIZE,
				USBDataValidCallBack0,
				NULL, 0);
		}
		else
		{
			libusb_fill_bulk_transfer(pThis->pUSBTransfer[0][i],
				(libusb_device_handle*)pThis->hDev[0],
				SSUSBDEV_IMGEPIN, pThis->pUSBRecvBuffer[0][i],
				USB_IMGRCV_BUFF_SIZE,
				USBDataValidCallBack0,
				NULL, 1500);
		}
		libusb_submit_transfer(pThis->pUSBTransfer[0][i]);
	}

#ifdef DEBUG_PORT
	if (pThis->pUSBDbgBuff == NULL)
	{
		pThis->pUSBDbgBuff = new uint8_t[1024];
	}

	if (pThis->pUSBDbgTransfer == NULL)
	{
		pThis->pUSBDbgTransfer = libusb_alloc_transfer(0);
	}

	libusb_fill_bulk_transfer(pThis->pUSBDbgTransfer,
		(libusb_device_handle*)pThis->hDev,
		SSUSBDEV_DEBUGIN, pThis->pUSBDbgBuff,
		1024,
		USBDbgValidCallBack,
		NULL, 1000);
	nRet = libusb_submit_transfer(pThis->pUSBDbgTransfer);
	if (nRet != 0)
	{

	}
#endif

	timeval tv;
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	while (pThis->nWatch[0])
	{
// 		nRet = libusb_handle_events(pThis->hDevCtx[0]);
		nRet = libusb_handle_events_timeout(pThis->hDevCtx[0], &tv);
		if (nRet < 0)
		{
			if ((nRet != LIBUSB_ERROR_BUSY) &&
				(nRet != LIBUSB_ERROR_TIMEOUT) &&
				(nRet != LIBUSB_ERROR_OVERFLOW) &&
				(nRet != LIBUSB_ERROR_INTERRUPTED))
			{
				break;
			}
		}
	}

#ifdef DEBUG_PORT
	if (pThis->pUSBDbgBuff != NULL)
	{
		delete[] pThis->pUSBDbgBuff;
	}

	if (pThis->pUSBDbgTransfer != NULL)
	{
		libusb_free_transfer(pThis->pUSBDbgTransfer);
	}
#endif

	return 0;
}
static unsigned __stdcall ThreadMain1(void *pParam)
{
	LIBUSBContex_t *pThis = (LIBUSBContex_t*)pParam;
	int nRet = 0;
	int read_size = 0;
	uint32_t totalsize = 0;
	uint32_t packetcnt = 0;

	if (!pBuffAddr[1] && s_SSUSBCtx.pBufferPool[1])
	{
		s_SSUSBCtx.pBufferPool[1]->LockBackFreeItem((void**)&pBuffAddr[1], ImgbufferLen[1]);
	}

	for (int i = 0; i < USB_TRANSFER_NUM; i++)
	{
		libusb_fill_bulk_transfer(pThis->pUSBTransfer[1][i],
			(libusb_device_handle*)pThis->hDev[1],
			SSUSBDEV_IMGEPIN, pThis->pUSBRecvBuffer[1][i],
			USB_IMGRCV_BUFF_SIZE,
			USBDataValidCallBack1,
			NULL, 1500);
		libusb_submit_transfer(pThis->pUSBTransfer[1][i]);
	}

#ifdef DEBUG_PORT
	if (pThis->pUSBDbgBuff == NULL)
	{
		pThis->pUSBDbgBuff = new uint8_t[1024];
	}

	if (pThis->pUSBDbgTransfer == NULL)
	{
		pThis->pUSBDbgTransfer = libusb_alloc_transfer(0);
	}

	libusb_fill_bulk_transfer(pThis->pUSBDbgTransfer,
		(libusb_device_handle*)pThis->hDev,
		SSUSBDEV_DEBUGIN, pThis->pUSBDbgBuff,
		1024,
		USBDbgValidCallBack,
		NULL, 1000);
	nRet = libusb_submit_transfer(pThis->pUSBDbgTransfer);
	if (nRet != 0)
	{

	}
#endif

	timeval tv;
	tv.tv_sec = 3;
	tv.tv_usec = 0;

	while (pThis->nWatch[1])
	{
		nRet = libusb_handle_events(pThis->hDevCtx[1]);
// 		nRet = libusb_handle_events_timeout(pThis->hDevCtx, &tv);
		if (nRet < 0)
		{
			if ((nRet != LIBUSB_ERROR_BUSY) &&
				(nRet != LIBUSB_ERROR_TIMEOUT) &&
				(nRet != LIBUSB_ERROR_OVERFLOW) &&
				(nRet != LIBUSB_ERROR_INTERRUPTED))
			{
				break;
			}
		}
	}

	return 0;
}

void USBDataValidCallBack0(struct libusb_transfer *pTransfer)
{
	int nRet = 0;
	static int curFrame = -1;
	static int tSize = 0;
	//static int actSize = 0;
	static int pCount = 0;
	static int errCnt = 0;
	int BufferSize = ImgbufferLen[0];// USBDATA_BUFF_SIZE / 2;

	if (!pTransfer)
	{
		return;
	}
#if LOG
	//test first frame s
// 	BYTE* ptem = (BYTE*)malloc(ImgLen[0]);
// 	memset(ptem, 0, ImgLen[0]);
	//test first frame e
#endif

	if (pTransfer->status == LIBUSB_TRANSFER_COMPLETED)
	{
#ifdef TEST_USB_SPEED
		if (packetcnt == 12000)
		{
			Time[1] = GetTickCount();

			tSize = tSize / 1024 / 1024;
			speed = (double)((tSize * 1000 ) / (double)(Time[1] - Time[0]));
			printf("Packet Count: %d, speed is %3.2fMByte/s, ZLP count is %d\r\n", packetcnt, speed, zlp_cnt);
			packetcnt = 0;
			tSize = 0;
			Time[0] = GetTickCount();
			zlp_cnt = 0;
		}
		if (pTransfer->actual_length == 0)
		{
			zlp_cnt++;
		}

		if (pTransfer->actual_length != USB_IMGRCV_BUFF_SIZE)
		{
			printf("pTransfer->actual_length = %u\r\n", pTransfer->actual_length);
		}

		tSize += pTransfer->actual_length;
		packetcnt++;
#else
		if ((pTransfer->actual_length == 0) /*|| (pTransfer->actual_length < USB_IMGRCV_BUFF_SIZE)*/)
		{

			if (pBuffAddr[0])
			{
				if ((actSize[0] >= ImgLen[0]) && ((actSize[0] % USB_IMGRCV_BUFF_SIZE) == 0))
				{
						*(uint32_t*)pBuffAddr[0] = ImgLen[0];
						nRet = s_SSUSBCtx.pBufferPool[0]->UnlockBackFreeItemAndSave(BufferSize);
#if LOG
// 						memcpy_s(ptem, ImgLen[0], pBuffAddr + 4, ImgLen[0]);
#endif

						if (nRet != 0)
						{
// 							printf("push to buffer err:%d\n", nRet);
						}
// 						emppaccnt = 0;
						//test e
						pBuffAddr[0] = NULL;

						nRet = s_SSUSBCtx.pBufferPool[0]->LockBackFreeItem((void**)&pBuffAddr[0], BufferSize);//get buffer addr for next pac data
						if (nRet != 0)
						{
							if (nRet == -2)
							{
// 								printf("Buffer pool is full\n");
							}
							else
							{
// 								printf("save data to buffer ERROR:%d\r\n", nRet);
							}
						}
					}

					curFrame++;
#if LOG
// 					if (gSaveCnt < 10)
// 					{
// 						char fileNm[100];
// 						_snprintf_s(fileNm, sizeof(fileNm), "D:\\%d.raw", gFrameCnt);
// 
// 						FILE* fp = NULL;
// 						fopen_s(&fp, fileNm, "wb");
// 						int ret = fwrite(ptem, 1, ImgLen[0], fp);
// 						fclose(fp);
// 
// // 						ostringstream ostr2;
// // 						ostr2 << "write ret:" << ret << "\r\n";
// // 						string strMeg2 = ostr2.str();
// // 						OutputDebugStringA(strMeg2.c_str());
// 
// 						gSaveCnt++;
// 					}

					ostringstream ostr1;
					ostr1 << "111 end:" << pacCnt << "\r\n";
					string strMeg1 = ostr1.str();
					OutputDebugStringA(strMeg1.c_str());
					pacCnt = 0;

					gFrameCnt++;

#endif

					actSize[0] = 0;
			}
			else//buffer pool is full
			{
				s_SSUSBCtx.pBufferPool[0]->LockBackFreeItem((void**)&pBuffAddr[0], ImgbufferLen[0]);
			}
		}
		else
		{
			if (pBuffAddr[0])
			{

				if (gLibMode == 0x0f)//车载数据处理
				{
					if ((BufferSize - actSize[0] - 4) >= (pTransfer->actual_length-12))//12字节包头
					{
						BYTE bZLPFlag = pTransfer->buffer[1];
						if (bZLPFlag == 0x8f || bZLPFlag == 0x8e)//帧结束标记
						{
							bDataContinue = FALSE;
						}
						else if (bZLPFlag == 0x8c || bZLPFlag == 0x8d)//小包包头数据标记
						{
							bUVCHeader = bZLPFlag;
							bDataContinue = TRUE;
							if (bUVCHeader_last == bUVCHeader)
								actSize[0] = 0;
						}

						memcpy_s(pBuffAddr[0] + actSize[0] + 4, BufferSize - actSize[0] - 4, pTransfer->buffer + 12, pTransfer->actual_length-12);
						actSize[0] += pTransfer->actual_length-12;

						if(bZLPFlag != 0x8f && bZLPFlag != 0x8e && bZLPFlag != 0x8c && bZLPFlag != 0x8d)//小包包头数据中其他标记，直接置位，丢弃数据
						{
							actSize[0] = 0;
						}

#if LOG
						pacCnt++;
						ostringstream ostr1;
						ostr1 << "222 memcpy cnt:" << pacCnt << "\r\n";
						string strMeg1 = ostr1.str();
						OutputDebugStringA(strMeg1.c_str());

						if (pacCnt > 623)
						{
							ostr1 << "333 more:" << pacCnt << "\r\n";
							strMeg1 = ostr1.str();
							OutputDebugStringA(strMeg1.c_str());
						}
#endif

						if ((bDataContinue == FALSE))
						{

							//*(uint32_t*)pBuffAddr = ImgLen;
							//memcpy(pBuffAddr, &ImgLen, 4);
							if ((actSize[0] == ImgLen[0]) && (bUVCHeader_last != bUVCHeader))
							{
								//*(uint32_t*)pBuffAddr = ImgLen;
								//memcpy(pBuffAddr, &ImgLen, 4);
								memcpy_s(pBuffAddr[0], 4, &ImgLen, 4);
								s_SSUSBCtx.pBufferPool[0]->UnlockBackFreeItemAndSave(BufferSize);
								pBuffAddr[0] = NULL;


								nRet = s_SSUSBCtx.pBufferPool[0]->LockBackFreeItem((void**)&pBuffAddr[0], BufferSize);
								bUVCHeader_last = bUVCHeader;
							}
							else
							{
								//pBuffAddr[0] = NULL;
							}
							curFrame++;
							actSize[0] = 0;

// 							ostringstream ostr1;
// 							ostr1 << "8f end:" << curFrame << "\r\n";
// 							string strMeg1 = ostr1.str();
// 							OutputDebugStringA(strMeg1.c_str());
							// 						printf("ImgLen > size, No empty packet");
						}
					}
				}
				else
				{
				if ((BufferSize - actSize[0] - 4) >= pTransfer->actual_length)
				{
					memcpy_s(pBuffAddr[0] + actSize[0] + 4, BufferSize - actSize[0] - 4, pTransfer->buffer, pTransfer->actual_length);
					actSize[0] += pTransfer->actual_length;
#if LOG
					pacCnt++;
					ostringstream ostr1;
					ostr1 << "222 memcpy cnt:" << pacCnt << "\r\n";
					string strMeg1 = ostr1.str();
					OutputDebugStringA(strMeg1.c_str());

					if (pacCnt > 623)
					{
						ostr1 << "333 more:" << pacCnt << "\r\n";
						strMeg1 = ostr1.str();
						OutputDebugStringA(strMeg1.c_str());
					}
#endif

					if (pTransfer->actual_length != USB_IMGRCV_BUFF_SIZE)
					{
						// 						printf("pTransfer->actual_length %u, size %u\r\n", pTransfer->actual_length, actSize);
					}

					if ((actSize[0] >= ImgLen[0]) && ((actSize[0] % USB_IMGRCV_BUFF_SIZE) != 0))
					{
						//*(uint32_t*)pBuffAddr = ImgLen;
						//memcpy(pBuffAddr, &ImgLen, 4);
						memcpy_s(pBuffAddr[0], 4, &ImgLen, 4);
						s_SSUSBCtx.pBufferPool[0]->UnlockBackFreeItemAndSave(BufferSize);
						pBuffAddr[0] = NULL;


						nRet = s_SSUSBCtx.pBufferPool[0]->LockBackFreeItem((void**)&pBuffAddr[0], BufferSize);
						if (nRet != 0)
						{
							//printf("Lock Buffer ERROR\r\n");
						}

						if (curFrame % 100 == 0)
						{
							//printf("Frame Index : %d, length is %u, l = %u\r\n", curFrame, actSize, pTransfer->actual_length);
						}
						curFrame++;
						actSize[0] = 0;

						// 						printf("ImgLen > size, No empty packet");

					}
				}
				}
			}
			else//buffer pool is full
			{
				s_SSUSBCtx.pBufferPool[0]->LockBackFreeItem((void**)&pBuffAddr[0], ImgbufferLen[0]);
			}
		}
#endif
	}


#if LOG
	//test s
// 	if (ptem != NULL)
// 	{
// 		free(ptem);
// 		ptem = NULL;
// 	}
	//test e
#endif
	nRet = libusb_submit_transfer(pTransfer);
	if (nRet < 0)
	{
 		//printf("Submit error (%d)\r\n", nRet);
	}
}
void USBDataValidCallBack1(struct libusb_transfer *pTransfer)
{
	int nRet = 0;
	static int curFrame = -1;
	static int tSize = 0;
// 	static int actSize = 0;
	static int pCount = 0;
	static int errCnt = 0;
	int BufferSize = 21 * 1024 * 1024;// USBDATA_BUFF_SIZE / 2;

	if (!pTransfer)
	{
		return;
	}

	if (pTransfer->status == LIBUSB_TRANSFER_COMPLETED)
	{
#ifdef TEST_USB_SPEED
		if (packetcnt == 12000)
		{
			Time[1] = GetTickCount();

			tSize = tSize / 1024 / 1024;
			speed = (double)((tSize * 1000) / (double)(Time[1] - Time[0]));
			printf("Packet Count: %d, speed is %3.2fMByte/s, ZLP count is %d\r\n", packetcnt, speed, zlp_cnt);
			packetcnt = 0;
			tSize = 0;
			Time[0] = GetTickCount();
			zlp_cnt = 0;
}
		if (pTransfer->actual_length == 0)
		{
			zlp_cnt++;
		}

		if (pTransfer->actual_length != USB_IMGRCV_BUFF_SIZE)
		{
			printf("pTransfer->actual_length = %u\r\n", pTransfer->actual_length);
		}

		tSize += pTransfer->actual_length;
		packetcnt++;
#else
		if ((pTransfer->actual_length == 0) /*|| (pTransfer->actual_length < USB_IMGRCV_BUFF_SIZE)*/)
		{

			if (pBuffAddr[1])
			{
				if ((actSize[1] >= ImgLen[1]) && ((actSize[1] % USB_IMGRCV_BUFF_SIZE) == 0))
				{
					*(uint32_t*)pBuffAddr[1] = ImgLen[1];
					nRet = s_SSUSBCtx.pBufferPool[1]->UnlockBackFreeItemAndSave(BufferSize);

					if (nRet != 0)
					{
						// 							printf("push to buffer err:%d\n", nRet);
					}

					// 						emppaccnt = 0;
											//test e
					pBuffAddr[1] = NULL;
					nRet = s_SSUSBCtx.pBufferPool[1]->LockBackFreeItem((void**)&pBuffAddr[1], BufferSize);//get buffer addr for next pac data
					if (nRet != 0)
					{
						if (nRet == -2)
						{
							// 								printf("Buffer pool is full\n");
						}
						else
						{
							// 								printf("save data to buffer ERROR:%d\r\n", nRet);
						}
					}
				}

				if (curFrame % 100 == 0)
				{
					//printf("Frame Index : %d, length is %u, img is %u\r\n", curFrame, actSize, ImgLen);
				}

				curFrame++;

				actSize[1] = 0;
			}
			else//buffer pool is full
			{
				s_SSUSBCtx.pBufferPool[1]->LockBackFreeItem((void**)&pBuffAddr[1], ImgbufferLen[1]);
			}
		}
		else
		{
			if (pBuffAddr[1])
			{
				if ((BufferSize - actSize[1] - 4) >= pTransfer->actual_length)
				{
					memcpy_s(pBuffAddr[1] + actSize[1] + 4, BufferSize - actSize[1] - 4, pTransfer->buffer, pTransfer->actual_length);
					actSize[1] += pTransfer->actual_length;


					if (pTransfer->actual_length != USB_IMGRCV_BUFF_SIZE)
					{
						// 						printf("pTransfer->actual_length %u, size %u\r\n", pTransfer->actual_length, actSize);
					}

					if ((actSize[1] >= ImgLen[1]) && ((actSize[1] % USB_IMGRCV_BUFF_SIZE) != 0))
					{
						//*(uint32_t*)pBuffAddr = ImgLen;
						//memcpy(pBuffAddr, &ImgLen, 4);
						memcpy_s(pBuffAddr[1], 4, &ImgLen[1], 4);
						s_SSUSBCtx.pBufferPool[1]->UnlockBackFreeItemAndSave(BufferSize);
						pBuffAddr[1] = NULL;


						nRet = s_SSUSBCtx.pBufferPool[1]->LockBackFreeItem((void**)&pBuffAddr[1], BufferSize);
						if (nRet != 0)
						{
							//printf("Lock Buffer ERROR\r\n");
						}

						if (curFrame % 100 == 0)
						{
							//printf("Frame Index : %d, length is %u, l = %u\r\n", curFrame, actSize, pTransfer->actual_length);
						}
						curFrame++;
						actSize[1] = 0;

						// 						printf("ImgLen > size, No empty packet");

					}
				}
			}
			else//buffer pool is full
			{
				s_SSUSBCtx.pBufferPool[1]->LockBackFreeItem((void**)&pBuffAddr[1], ImgbufferLen[1]);
			}
		}
#endif
	}

	nRet = libusb_submit_transfer(pTransfer);
	if (nRet < 0)
	{
		//printf("Submit error (%d)\r\n", nRet);
	}
}

#ifdef DEBUG_PORT
void USBDbgValidCallBack(struct libusb_transfer *pTransfer)
{
	int nRet = 0;
	char msg[1024] = { 0 };
	static int cnt = 0;

	if (pTransfer->status == LIBUSB_TRANSFER_COMPLETED)
	{
		if (pTransfer->actual_length < 1024)
		{
			cnt++;
			memcpy_s(msg, 1024, pTransfer->buffer, pTransfer->actual_length);
			printf("%s", msg);
		}
		else
		{
			printf("Debug information overflow!\r\n");
		}
	}

	nRet = libusb_submit_transfer(pTransfer);
	if (nRet < 0)
	{
		printf("Debug Submit error (%d)\r\n", nRet);
	}
}
#endif

int usb_get_frame(HANDLE hDev, void* pFrame, int BufferSize, int* pFrameSize, uint32_t Timeout)
{
	int ret = 0;
	uint8_t* pImgBuff = NULL;
	int size = 0;
	CBufferPool *pBuffer = NULL;

	for (int i=0; i<gDevCnt; i++)
	{
		if (hDev == gHandleList[i])
		{
			pBuffer = s_SSUSBCtx.pBufferPool[i];
			break;
		}
	}

	if (pBuffer == NULL)
	{
		return -4;
	}

	if (pFrame == NULL)
	{
		return -2;
	}

	ret = pBuffer->LockFrontItem((void**)&pImgBuff, &size, Timeout);
	if (ret != 0)
	{
		return ret;
	}

	*pFrameSize = *(uint32_t*)pImgBuff;

	if (BufferSize >= *pFrameSize)
	{
		memcpy_s(pFrame, BufferSize, pImgBuff + 4, *pFrameSize);
		pBuffer->UnlockFrontItemAndDel();
	}
	else
	{
		pBuffer->UnlockFrontItemAndDel();
		return -1;
	}
	
	return 0;
}

struct libusb_device_descriptor desc;

int libusb_enum_devs(SmartsensDev *Sdev)
{
	libusb_device *dev;
	int i = 0, j = 0;
	uint8_t path[8];
	int nRet = 0;

	while ((dev = Sdev->devs[i++]) != NULL) {
		struct libusb_device_descriptor desc;
		int r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0) {
			fprintf(stderr, "failed to get device descriptor");
			return -1;
		}

		//Select smartsens dev
		if (desc.idVendor == SSUSBDEV_VID && desc.idProduct == SSUSBDEV_PID)
		{
			Sdev->EnumAddr[Sdev->DevCnt] = libusb_get_device_address(dev);
			{//Find SerialNumber			
				PER_DEVICE_HANDLE devhandle;
				if (libusb_open(dev, &devhandle) < 0)
				{
					devhandle = NULL;
					nRet = -1;//当前设备无法打开
					continue;
				}
				libusb_get_string_descriptor_ascii(devhandle, desc.iSerialNumber, Sdev->uSerialNumber[Sdev->DevCnt], CHAR_MAX_LEN);
				libusb_close(devhandle);
				
				printf("SerialNumber=%s \n", Sdev->uSerialNumber[Sdev->DevCnt]);
			}
			printf("%04x:%04x (bus %d, device %d)",
				desc.idVendor, desc.idProduct,
				libusb_get_bus_number(dev), Sdev->EnumAddr[Sdev->DevCnt]);

			r = libusb_get_port_numbers(dev, path, sizeof(path));
			if (r > 0) {
				printf(" path: %d", path[0]);
				for (j = 1; j < r; j++)
					printf(".%d", path[j]);
			}
			printf("\n");
			Sdev->DevCnt++;
		}
	}
	printf(" 0 DevCnt = %d \n", Sdev->DevCnt);
	return Sdev->DevCnt;
}
//
//int libusb_enum_smartsens_device(SmartsensDev *Sdev)
//{
//	int r;
//
//	//libusb_device *dev;
//	uint32_t gAdrress[8] = { 0 };
//	ssize_t cnt;
//
//	r = libusb_init(NULL);
//	if (r < 0)
//		return r;
//
//	cnt = libusb_get_device_list(NULL, &(Sdev->devs));
//	if (cnt < 0)
//		return (int)cnt;
//
//	libusb_enum_devs(Sdev);
//	libusb_free_device_list(Sdev->devs, 1);
//
//	libusb_exit(NULL);
//	return 0;
//}

int libusb_open_device_with_address(libusb_context* ctx, PER_DEVICE_HANDLE* devhandlelist, int ctxIndex)
 {
 	struct libusb_device** devs;
 	struct libusb_device* dev;
	uint8_t path[8];

 	size_t i = 0, j = 0;
 	int r = 0;
 	if (libusb_get_device_list(ctx, &devs) < 0)
 		return -1;

	int ndevnum = 0;
 	while ((dev = devs[i++]) != NULL)
 	{
// 		struct libusb_device_descriptor desc;

 		r = libusb_get_device_descriptor(dev, &desc);
		if (r < 0)
			return -1;

		if (desc.idVendor == SSUSBDEV_VID && desc.idProduct == SSUSBDEV_PID)
		{		
			if (ctxIndex == 0)
			{
				gAdrress[0] = libusb_get_device_address(dev);
				r = libusb_open(dev, &devhandlelist[ndevnum]);
				if (r < 0)
				{
					devhandlelist[ndevnum] = NULL;
					break;
				}

				ndevnum++;

				break;
			}
			else
			{
				if (ndevnum == 0 && gAdrress[0] != 0)
				{
					ndevnum++;
					continue;
				}
				else
				{
					if (gAdrress[1] != gAdrress[0])
					{
						gAdrress[1] = libusb_get_device_address(dev);
						r = libusb_open(dev, &devhandlelist[ndevnum]);
						if (r < 0)
						{
							devhandlelist[ndevnum] = NULL;
							break;
						}
						ndevnum++;

						break;

					}
				}
			}
		}
 	}

 	libusb_free_device_list(devs, 1);

 	return ndevnum;
 }


int libusb_open_device_with_addressEx(libusb_device **devs, libusb_context* ctx, PER_DEVICE_HANDLE &devhandle, int ctxIndex)
{
	//struct libusb_device** devs;
	//struct libusb_device* dev;
	//uint8_t path[8];

	//size_t i = 0, j = 0;
	//int r = 0;
	//list出所有的USB设备
	int nRet = -1;
	int DevCnt = 0;
	DevCnt = libusb_get_device_list(ctx, &(devs));
	if (DevCnt < 0)
		return -1;

	int ndevnum = 0;
	for (int i = 0; i < DevCnt; i++)
	{
		struct libusb_device* dev;
		while ((dev = devs[i++]) != NULL)
		{				
			int r = libusb_get_device_descriptor(dev, &desc);
			if (r < 0)
				return -1;

			if (desc.idVendor == SSUSBDEV_VID && desc.idProduct == SSUSBDEV_PID)
			{
				int tempdev = libusb_get_device_address(dev);
				if (tempdev == ctxIndex)
				{
					int r = libusb_open(dev, &devhandle);
					//r = -1;
					if (r < 0)
					{
						devhandle = NULL;
						nRet = -1;//当前设备无法打开
						break;
					}
					nRet = 0;
				}

			}
				
		}

	}
	libusb_free_device_list(devs, 1);

	return nRet;
}