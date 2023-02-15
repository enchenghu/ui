#ifndef ACUMULATOR_API_H
#define ACUMULATOR_API_H

#include <stdint.h>

#include "Raf_Api.h"
#include "ver.h"



#ifdef __cplusplus
extern "C"
{
#endif


	typedef void (STD_CALL* Callback)(int32_t typeID, uint8_t* buffer, uint32_t len);




	typedef struct MessageHandler
	{
		uint16_t type;

		uint32_t msgLen;

		uint32_t unRemainMessageLength;

		uint32_t unHeaderIsCopy;

		uint32_t unTotalMsgLen;

		uint32_t unMessageTail;

	}MessageHandler;

	typedef struct CyclicBuffer
	{
		uint8_t* pucBufferStartPoint;

		uint8_t* pucBufferCyclicLocation;

		uint32_t unBufferSize;

		MessageHandler tMessageHandler;


	}CyclicBuffer;

	typedef struct TAccumulator_Handler
	{
		CyclicBuffer* tCurrentBufferCyclic;
		
		CyclicBuffer tCyclicBufferAckMessage;

		CyclicBuffer tCyclicBufferRequstedInformation;

		TRAF_API_Header		tHeader;

		bool		bIsMiddleOfHeaderState;

		bool		bHeaderMybeCutoff;

		uint32_t	unMiddleHeaderLength;

		bool		bNeedMoreData;

		uint16_t usLastReceivedtype;

		Callback callback;


	}TAccumulator_Handler;




	DLL_EXPIMP void RAF_DRV_CallbackInit(uint8_t* pucBufferPointer, uint32_t unLength, Callback ptCallback);

	DLL_EXPIMP void RAF_DRV_AccumulateRxData(uint8_t* pucReceivedBuffer, uint32_t unReceivedBufferLength);




    DLL_EXPIMP void RAF_API_EXT_SysCfg_ConfigurePcOutput(ptArbeApiMailBox ptMailboxHandler, TConfigurePointCloudsInfo* info);
    DLL_EXPIMP void RAF_API_EXT_SysCfg_ConfigPcBufOutput(ptArbeApiMailBox ptMailboxHandler, TConfigPCInfo* info);

    DLL_EXPIMP void RAF_API_EXT_CalUtil_RecordFrameRawData(ptArbeApiMailBox ptMailboxHandler, TRecordFrameRawDataInfo* info);


#ifdef __cplusplus
}
#endif


#endif
