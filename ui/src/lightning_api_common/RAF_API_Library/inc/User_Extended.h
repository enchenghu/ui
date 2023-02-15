#ifndef USER_EXTENDED_H_
#define USER_EXTENDED_H_

#include <stdint.h>

#include "Raf_Api.h"

#ifdef RAF_API_EXTENDED

#ifdef __cplusplus
extern "C"
{
#endif

#define ETH_NUM_OF_SUPPORTED_PORTS 				2
#define TCP_BASE_PORT							6001
#define TCP_PORT_PC								(TCP_BASE_PORT)
#define TCP_PORT_DSP							(TCP_BASE_PORT +1)

#define	UDP_BASE_PORT							6003

#define ETH_PORT_TO_IDX(usPort)					(usPort - TCP_BASE_PORT)

#define ETHERNET_DEST_PC						(uint16_t)TCP_PORT_PC
#define ETHERNET_DEST_DSP						(uint16_t)TCP_PORT_DSP

#define TCP_PACKET_LENGTH						1400




#define ETH_SANITY_MAX_PACKET_LEN				5000



#define DSP_OUTPUT_PC										0x01
#define DSP_OUTPUT_SLAM										0x02
#define DSP_PROCESS_SLAM									0x04
#define DSP_SET_PARAMS_CMD 									0x08
#define DSP_GET_PARAMS_CMD 									0x10
#define DSP_PROCESS_AUTO_THRESHOLD_CMD 						0x20
#define DSP_GET_STATUS_CMD 									0x40

	/* ack data types */
#define DSP_ACK_DATA_NONE				0x1
#define DSP_ACK_DATA_PARAMS				0x2
#define DSP_ACK_DATA_STATUS				0x4

#define DSP_STATUS_BITMAP_IS_CONNECTED		0x00000001



	/*######################### ENUMERATION ###############################*/

	typedef enum
	{
		External_Cmd_None = 0x2000,
		AtcMode,
		DspJob,
		Last_External_CmdType
	}EExtendedCmdType;


	/*######################### BASE STRUCTURES ###############################*/

	typedef struct
	{
		uint32_t unEnableAtc;
		uint32_t unsensitivity;
	}TAtcInfo;
	typedef struct
	{
		TRAF_API_Header			tHeader;
		TAtcInfo				tAtcInfo;
	}TAtcMode;

	typedef struct
	{
		TMemoryOperationInfo	tMemoryOperationInfo;
		uint32_t 				unCurrentDataLength;
		uint32_t 				unSliceNumber;
		uint8_t					pucData[TCP_PACKET_LENGTH];
		uint32_t 				usCrc;
	}TMemoryDataOpInfo;


	typedef struct
	{
		TRAF_API_Header     tHeader;
		TMemoryDataOpInfo	tMemoryOperation;
	}TMemoryDataOp;


	typedef struct
	{
		uint32_t version;
		uint32_t bitmap;
	}TDspStatus;

	typedef struct
	{
		uint16_t 	usPrefix;
		uint16_t 	usType;
		uint32_t 	unLength;
		uint32_t 	unTime;
		uint32_t 	unMessageNumber;
	}TDSPjobHeader;
	typedef struct
	{
		uint16_t	usTag;
		uint32_t 	unJobList_lsb;
		uint32_t 	unJobList_msb;
		uint8_t*	pucData;
		uint32_t	unDataLength;
		//Additional data here
		uint8_t 	ucCrc[4];
	}TDspInfo;
	typedef struct
	{
		TDSPjobHeader tHeader;
		TDspInfo	  tDspInfo;
	}TDSPjob;




	DLL_EXPIMP void RAF_API_EXT_AtcMode(TAtcInfo* info);

	DLL_EXPIMP void RAF_API_EXT_WriteData(TMemoryDataOpInfo* SliceInfo);

	DLL_EXPIMP void RAF_API_EXT_DspJob(TDspInfo* info);



#ifdef __cplusplus
}
#endif


#endif

#endif
