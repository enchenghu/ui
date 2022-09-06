#include <math.h>
#include <stdio.h>
#include <stdarg.h>

#include <ctype.h>

#include "User_Extended.h"
#include "Utils.h"


#ifdef __cplusplus
extern "C"
{
#endif



	void RAF_API_EXT_AtcMode(TAtcInfo* info)
	{
		TAtcMode tAtcMode;

		int _size = sizeof(TAtcMode);

		memset(&tAtcMode, 0, _size);


		BuildHeader(AtcMode, _size, &tAtcMode.tHeader);

		tAtcMode.tAtcInfo.unEnableAtc = info->unEnableAtc;
		tAtcMode.tAtcInfo.unsensitivity = info->unsensitivity;


		GetApiMailBox()->sendCmd(AtcMode, (uint8_t*)&tAtcMode, _size);
	}

	void RAF_API_EXT_DspJob(TDspInfo* info) 
	{
		uint8_t buff[256];
		uint8_t sizeOfCRC = 4;
		uint8_t ucTLVHeaderLength = 0;


		TDSPjob tDspJob;

		TRAF_API_Header tTRAF_API_Header;

		BuildHeader(DspJob, sizeof(TDSPjob), &tTRAF_API_Header);

		tDspJob.tHeader.usPrefix = tTRAF_API_Header.usPrefix;
		tDspJob.tHeader.usType = tTRAF_API_Header.usType;
		tDspJob.tHeader.unTime = tTRAF_API_Header.unTimeLsb;
		tDspJob.tHeader.unMessageNumber = tTRAF_API_Header.unMessageNumber;

		tDspJob.tDspInfo.unJobList_lsb = info->unJobList_lsb;
		tDspJob.tDspInfo.unJobList_msb = info->unJobList_msb;

		memcpy(buff, (uint8_t*)&tDspJob, sizeof(TDSPjob));
		int Err = TLV_EncodeTLV(info->usTag, info->unDataLength, info->pucData, buff + sizeof(TDSPjob) - sizeOfCRC, 255, &ucTLVHeaderLength);
		if (!Err)
		{
			tDspJob.tHeader.unLength = info->unDataLength + ucTLVHeaderLength + sizeof(TDSPjob);
			memcpy(buff + sizeof(TDSPjob) - sizeOfCRC + info->unDataLength + ucTLVHeaderLength, (uint8_t*)&tDspJob.tDspInfo.ucCrc, sizeOfCRC);

			GetApiMailBox()->sendCmd(DspJob, (uint8_t*)&tDspJob, tDspJob.tHeader.unLength);
		}



	}

	void RAF_API_EXT_WriteData(TMemoryDataOpInfo* Info)
	{
		TMemoryDataOp tMemoryOperation;

		int _size = sizeof(TMemoryDataOp);

		memset(&tMemoryOperation, 0, _size);

		BuildHeader(MemoryOperation_Input, _size, &tMemoryOperation.tHeader);

		tMemoryOperation.tMemoryOperation.tMemoryOperationInfo.ePeripheralDevice = Info->tMemoryOperationInfo.ePeripheralDevice;

		tMemoryOperation.tMemoryOperation.tMemoryOperationInfo.eMemoryOperation = Info->tMemoryOperationInfo.eMemoryOperation;

		tMemoryOperation.tMemoryOperation.tMemoryOperationInfo.unChipID = Info->tMemoryOperationInfo.unChipID;

		tMemoryOperation.tMemoryOperation.tMemoryOperationInfo.unAddress = Info->tMemoryOperationInfo.unAddress;

		tMemoryOperation.tMemoryOperation.tMemoryOperationInfo.unValueOrTotalLength = Info->tMemoryOperationInfo.unValueOrTotalLength;

		tMemoryOperation.tMemoryOperation.tMemoryOperationInfo.eSpecialOpcode = Info->tMemoryOperationInfo.eSpecialOpcode;

		tMemoryOperation.tMemoryOperation.unCurrentDataLength = Info->unCurrentDataLength;

		tMemoryOperation.tMemoryOperation.unSliceNumber = Info->unSliceNumber;

		memcpy(tMemoryOperation.tMemoryOperation.pucData, Info->pucData,TCP_PACKET_LENGTH);


		GetApiMailBox()->sendCmd(MemoryOperation_Input, (uint8_t*)&tMemoryOperation, _size);
	}


#ifdef __cplusplus
}
#endif
