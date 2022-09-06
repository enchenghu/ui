#ifndef ARBE_MAIL_BOX_H_
#define ARBE_MAIL_BOX_H_

#include <stdint.h>

#include "CommonBase.h"

#ifdef __cplusplus
extern "C"
{
#endif


	typedef struct TArbeApiMailBox* ptArbeApiMailBox;

	typedef void (*SendCmd)(int32_t unMsgID, uint8_t* pucBuffer, uint32_t unLen);

	typedef unsigned long (*SysCfg_GetSystemTimeMs)();



	struct TArbeApiMailBox
	{
		SendCmd					sendCmd;
		SysCfg_GetSystemTimeMs  sysCfg_GetSystemTimeMs;
		uint32_t			    unMessageNumber;			// Read Only , RAF_API increment this filed.

	};


#ifdef __cplusplus
}
#endif


#endif