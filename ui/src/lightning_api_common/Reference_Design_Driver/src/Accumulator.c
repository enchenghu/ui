#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include <ctype.h>

#include "Accumulator.h"


#ifdef __cplusplus
extern "C"
{
#endif

	TAccumulator_Handler tAccumulatorHndler;


	/************ LOCAL FUNCTION *************/

	bool SearchForHeader(uint8_t** pucReceivedBuffer, uint32_t* punReceivedBufferLength, bool* pbIsMiddleOfHeaderReceived);

	bool IsRequstedType(EOutputType unType);

	bool ValidateHeader(TRAF_API_Header* pucHeaderMessage);

	void AccumulateInformation(CyclicBuffer* tCyclicBuffer, uint8_t** pucStartMessage, uint32_t* unBufferLength);

	void ArbeApiParseMessage(uint8_t* buffer, int32_t type, uint32_t len);

	void CopyHeader(CyclicBuffer* tCyclicBuffer, TRAF_API_Header* pucStartMessage);

	/*****************************************/




	void RAF_API_EXT_CalUtil_RecordFrameRawData(ptArbeApiMailBox ptMailboxHandler,TRecordFrameRawDataInfo* info)
	{

		tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferStartPoint = info->pucDestinationAddress;

		tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation = info->pucDestinationAddress;

		tAccumulatorHndler.tCyclicBufferRequstedInformation.unBufferSize = info->unDestinationBufferSize;

		RAF_API_CalUtil_RecordFrameRawData(ptMailboxHandler,info);

	}

	void RAF_API_EXT_SysCfg_ConfigurePcOutput(ptArbeApiMailBox ptMailboxHandler,TConfigurePointCloudsInfo* info)
	{
		tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferStartPoint = info->pucDestinationAddress;

		tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation = info->pucDestinationAddress;

		tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unTotalMsgLen = 0;

		tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unMessageTail = 0;

		tAccumulatorHndler.tCyclicBufferRequstedInformation.unBufferSize = info->unDestinationBufferSize;

		RAF_API_SysCfg_ConfigurePcOutput(ptMailboxHandler,info);
	}



    void RAF_API_EXT_SysCfg_ConfigPcBufOutput(ptArbeApiMailBox ptMailboxHandler,TConfigPCInfo* info)
    {
#if __x86_64__
    /* 64-bit */
        tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferStartPoint             = (uint64_t)(info->unDestAdrLsb) | (uint64_t)(info->unDestAdrMsb) <<32;
        tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation         = (uint64_t)(info->unDestAdrLsb) | (uint64_t)(info->unDestAdrMsb) <<32;
#else
        tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferStartPoint             = (uint8_t*)info->unDestAdrLsb;
        tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation         = (uint8_t*)info->unDestAdrLsb;
#endif
        tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unTotalMsgLen   = 0;
        tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unMessageTail   = 0;
        tAccumulatorHndler.tCyclicBufferRequstedInformation.unBufferSize                    = info->unDestBufSize;
    }


	/************ACCUMULATOR INIT AND LOGIC *************/

	void RAF_DRV_CallbackInit(uint8_t* pucBufferPointer, uint32_t unLength, Callback ptCallback)
	{
		tAccumulatorHndler.tCyclicBufferAckMessage.tMessageHandler.type = 0;

		tAccumulatorHndler.tCyclicBufferAckMessage.tMessageHandler.unRemainMessageLength = 0;

		tAccumulatorHndler.tCyclicBufferAckMessage.unBufferSize = unLength;

		tAccumulatorHndler.tCyclicBufferAckMessage.pucBufferStartPoint = pucBufferPointer;

		tAccumulatorHndler.tCyclicBufferAckMessage.pucBufferCyclicLocation = pucBufferPointer;

		tAccumulatorHndler.tCyclicBufferAckMessage.tMessageHandler.msgLen = 0;

		tAccumulatorHndler.tCurrentBufferCyclic = &tAccumulatorHndler.tCyclicBufferAckMessage;

		tAccumulatorHndler.bNeedMoreData = false;

		tAccumulatorHndler.bIsMiddleOfHeaderState = false;

		tAccumulatorHndler.callback = ptCallback;

	}

	void RAF_DRV_AccumulateRxData(uint8_t* pucReceivedBuffer, uint32_t unReceivedBufferLength)
	{
		bool bIsMiddleOfHeaderComplete;
		while (unReceivedBufferLength > 0)
		{

			if (tAccumulatorHndler.bNeedMoreData)
			{
				if (IsRequstedType(tAccumulatorHndler.usLastReceivedtype))
				{
					AccumulateInformation(&tAccumulatorHndler.tCyclicBufferRequstedInformation, &pucReceivedBuffer, &unReceivedBufferLength);
				}
				else
				{
					AccumulateInformation(&tAccumulatorHndler.tCyclicBufferAckMessage, &pucReceivedBuffer, &unReceivedBufferLength);
				}
			}
			else if (SearchForHeader(&pucReceivedBuffer, &unReceivedBufferLength, &bIsMiddleOfHeaderComplete))
			{
				TRAF_API_Header tMessageHeader;
				if (bIsMiddleOfHeaderComplete)
				{
					tMessageHeader = tAccumulatorHndler.tHeader;
				}
				else
				{
					tMessageHeader = *(TRAF_API_Header*)pucReceivedBuffer;
				}


				if (ValidateHeader(&tMessageHeader))
				{
					tAccumulatorHndler.usLastReceivedtype = tMessageHeader.usType;

					tAccumulatorHndler.tCurrentBufferCyclic->tMessageHandler.msgLen = tMessageHeader.unLength;

					tAccumulatorHndler.tCurrentBufferCyclic->tMessageHandler.type = tMessageHeader.usType;


					if (tAccumulatorHndler.tCurrentBufferCyclic->pucBufferStartPoint == NULL)
					{
						unReceivedBufferLength += tMessageHeader.unLength;
						continue;
					}

					if (bIsMiddleOfHeaderComplete)
					{
						CopyHeader(tAccumulatorHndler.tCurrentBufferCyclic, &tAccumulatorHndler.tHeader);
					}

					AccumulateInformation(tAccumulatorHndler.tCurrentBufferCyclic, &pucReceivedBuffer, &unReceivedBufferLength);
				}
				else
				{
					unReceivedBufferLength = 0; // Maybe there is better way to handle this case
				}
			}
		}
	}




	bool SearchForHeader(uint8_t** pucReceivedBuffer, uint32_t* punReceivedBufferLength, bool* pbIsMiddleOfHeaderReceived)
	{
		uint32_t unBufferIndexer = 0;
		bool bHeaderFound = false;
		*pbIsMiddleOfHeaderReceived = false;
		if (tAccumulatorHndler.bHeaderMybeCutoff)
		{
			if (!((uint8_t)(PACKET_PREFIX >> 8) == *(uint8_t*)(*pucReceivedBuffer + unBufferIndexer)))
			{
				tAccumulatorHndler.bIsMiddleOfHeaderState = false;
			}
			tAccumulatorHndler.bHeaderMybeCutoff = false;
		}
		if (tAccumulatorHndler.bIsMiddleOfHeaderState)
		{

			if ((sizeof(TRAF_API_Header) - tAccumulatorHndler.unMiddleHeaderLength) > * punReceivedBufferLength)
			{
				memcpy((uint8_t*)(((uint8_t*)&tAccumulatorHndler.tHeader) + tAccumulatorHndler.unMiddleHeaderLength), (uint8_t*)(*pucReceivedBuffer + unBufferIndexer), *punReceivedBufferLength);

				tAccumulatorHndler.unMiddleHeaderLength += *punReceivedBufferLength;

				*pucReceivedBuffer += *punReceivedBufferLength;

				*punReceivedBufferLength -= *punReceivedBufferLength;

				bHeaderFound = false;
			}
			else
			{
				memcpy((uint8_t*)(((uint8_t*)&tAccumulatorHndler.tHeader) + tAccumulatorHndler.unMiddleHeaderLength), (uint8_t*)(*pucReceivedBuffer + unBufferIndexer), sizeof(TRAF_API_Header) - tAccumulatorHndler.unMiddleHeaderLength);

				*pucReceivedBuffer += sizeof(TRAF_API_Header) - tAccumulatorHndler.unMiddleHeaderLength;

				*punReceivedBufferLength -= sizeof(TRAF_API_Header) - tAccumulatorHndler.unMiddleHeaderLength;


				tAccumulatorHndler.unMiddleHeaderLength = 0;

				*pbIsMiddleOfHeaderReceived = true;


				bHeaderFound = true;

			}


		}
		else
		{

			while (unBufferIndexer < *punReceivedBufferLength)
			{
				if ((uint8_t)PACKET_PREFIX == *(uint8_t*)(*pucReceivedBuffer + unBufferIndexer))
				{
					if ((unBufferIndexer + 1) >= *punReceivedBufferLength)
					{
						memcpy((uint8_t*)&tAccumulatorHndler.tHeader, (uint8_t*)(*pucReceivedBuffer + unBufferIndexer), *punReceivedBufferLength - unBufferIndexer);

						tAccumulatorHndler.unMiddleHeaderLength = *punReceivedBufferLength - unBufferIndexer;

						*pucReceivedBuffer += *punReceivedBufferLength;

						*punReceivedBufferLength -= *punReceivedBufferLength;

						tAccumulatorHndler.bHeaderMybeCutoff = true;

						tAccumulatorHndler.bIsMiddleOfHeaderState = true;

						bHeaderFound = false;
						break;

					}
					else if ((uint8_t)(PACKET_PREFIX >> 8) == *(uint8_t*)(*pucReceivedBuffer + unBufferIndexer + 1))
					{

						if (sizeof(TRAF_API_Header) > * punReceivedBufferLength)
						{

							memcpy((uint8_t*)&tAccumulatorHndler.tHeader, (uint8_t*)(*pucReceivedBuffer), *punReceivedBufferLength);

							tAccumulatorHndler.unMiddleHeaderLength = *punReceivedBufferLength;

							*pucReceivedBuffer += *punReceivedBufferLength;

							*punReceivedBufferLength -= *punReceivedBufferLength;


							tAccumulatorHndler.bIsMiddleOfHeaderState = true;
							bHeaderFound = false;
							break;
						}

						*pucReceivedBuffer += unBufferIndexer;

						*punReceivedBufferLength -= unBufferIndexer;

						tAccumulatorHndler.bIsMiddleOfHeaderState = false;


						

						bHeaderFound = true;
						break;

					}

				}

				unBufferIndexer += 1;

			}
			if (!bHeaderFound && unBufferIndexer == *punReceivedBufferLength)
			{
				*pucReceivedBuffer += unBufferIndexer;

				*punReceivedBufferLength -= unBufferIndexer;
			}

		}



		return bHeaderFound;

	}

	bool ValidateHeader(TRAF_API_Header* pucHeaderMessage)
	{
		bool bIsValid = true;

		if (pucHeaderMessage->usType >= last_output_type)
		{
			bIsValid = false;
		}
		else
		{
			if (IsRequstedType(pucHeaderMessage->usType))
			{
				tAccumulatorHndler.tCurrentBufferCyclic = &tAccumulatorHndler.tCyclicBufferRequstedInformation;
			}
			else
			{
				tAccumulatorHndler.tCurrentBufferCyclic = &tAccumulatorHndler.tCyclicBufferAckMessage;
			}

			if (pucHeaderMessage->unLength > tAccumulatorHndler.tCurrentBufferCyclic->unBufferSize)
			{
				bIsValid = false;
			}
		}

		return bIsValid;
	}

	bool IsRequstedType(EOutputType unType)
	{
		switch (unType)
		{
		case Raw_Output:
			return true;
		case Range_Output:
			return true;
		case PointCloud_Output:
			return true;
		case PointCloud_Output_V1_1:
			return false;
		case Slam_Output:
			return true;
		case Slam_Output_V1_1:
			return true;
		default:
			return false;
		}
	}

	void AccumulateInformation(CyclicBuffer* tCyclicBuffer, uint8_t** pucStartMessage, uint32_t* unBufferLength)
	{
		if (tAccumulatorHndler.bNeedMoreData) // middle of data
		{

			// copy received data to memory
			if (tCyclicBuffer->tMessageHandler.unRemainMessageLength > * unBufferLength) // not all the data
			{
				memcpy(tCyclicBuffer->pucBufferCyclicLocation, *pucStartMessage, *unBufferLength);

				tCyclicBuffer->pucBufferCyclicLocation = tCyclicBuffer->pucBufferCyclicLocation + *unBufferLength;

				tCyclicBuffer->tMessageHandler.unRemainMessageLength -= *unBufferLength;

				*pucStartMessage += *unBufferLength;

				*unBufferLength -= *unBufferLength;

				tAccumulatorHndler.bNeedMoreData = true;
#ifdef EXTERNAL_USER
				OutputDebugString("1,true,true\n");
#endif

			}
			else if (tCyclicBuffer->tMessageHandler.unRemainMessageLength <= *unBufferLength)	// full message
			{

				memcpy(tCyclicBuffer->pucBufferCyclicLocation, *pucStartMessage, tCyclicBuffer->tMessageHandler.unRemainMessageLength);

				uint8_t* _pucStartMessage = (uint8_t*)(tCyclicBuffer->pucBufferCyclicLocation - tCyclicBuffer->tMessageHandler.msgLen + tCyclicBuffer->tMessageHandler.unRemainMessageLength);


				ArbeApiParseMessage(_pucStartMessage, tCyclicBuffer->tMessageHandler.type, tCyclicBuffer->tMessageHandler.msgLen);

				tCyclicBuffer->pucBufferCyclicLocation = tCyclicBuffer->pucBufferCyclicLocation + tCyclicBuffer->tMessageHandler.unRemainMessageLength;

				tAccumulatorHndler.bNeedMoreData = false;

				*pucStartMessage += tCyclicBuffer->tMessageHandler.unRemainMessageLength;

				*unBufferLength -= tCyclicBuffer->tMessageHandler.unRemainMessageLength;

				tCyclicBuffer->tMessageHandler.unRemainMessageLength = 0;


#ifdef EXTERNAL_USER
				OutputDebugString("2,true,false\n");
#endif

			}

		}
		else
		{

			// Set cyclic pointer to start location
			if ((tCyclicBuffer->pucBufferCyclicLocation + tCyclicBuffer->tMessageHandler.msgLen) >= (tCyclicBuffer->pucBufferStartPoint + tCyclicBuffer->unBufferSize))
			{
				tCyclicBuffer->pucBufferCyclicLocation = tCyclicBuffer->pucBufferStartPoint;
			}

			// copy received data to memory
			if (tCyclicBuffer->tMessageHandler.msgLen > * unBufferLength) // not all the data
			{
				memcpy(tCyclicBuffer->pucBufferCyclicLocation, *pucStartMessage, *unBufferLength);

				if (tCyclicBuffer->tMessageHandler.unHeaderIsCopy == 1)
				{
					tCyclicBuffer->tMessageHandler.unRemainMessageLength = tCyclicBuffer->tMessageHandler.msgLen - *unBufferLength - sizeof(TRAF_API_Header);

				}
				else
				{
					tCyclicBuffer->tMessageHandler.unRemainMessageLength = tCyclicBuffer->tMessageHandler.msgLen - *unBufferLength;

				}

				tCyclicBuffer->pucBufferCyclicLocation = tCyclicBuffer->pucBufferCyclicLocation + *unBufferLength;

				*pucStartMessage += *unBufferLength;

				*unBufferLength -= *unBufferLength;

				tCyclicBuffer->tMessageHandler.unHeaderIsCopy = 0;

				tAccumulatorHndler.bNeedMoreData = true;

#ifdef EXTERNAL_USER
				OutputDebugString(" 3,false,true\n");
#endif


			}
			else if (tCyclicBuffer->tMessageHandler.msgLen <= *unBufferLength)	// full message
			{
				memcpy(tCyclicBuffer->pucBufferCyclicLocation, *pucStartMessage, tCyclicBuffer->tMessageHandler.msgLen);

				if (tCyclicBuffer->tMessageHandler.unHeaderIsCopy == 1)
				{
					ArbeApiParseMessage((uint8_t*)(tCyclicBuffer->pucBufferCyclicLocation - sizeof(TRAF_API_Header)), tCyclicBuffer->tMessageHandler.type, tCyclicBuffer->tMessageHandler.msgLen);

					tCyclicBuffer->pucBufferCyclicLocation = tCyclicBuffer->pucBufferCyclicLocation + tCyclicBuffer->tMessageHandler.msgLen - sizeof(TRAF_API_Header);

					*pucStartMessage += tCyclicBuffer->tMessageHandler.msgLen - sizeof(TRAF_API_Header);

					*unBufferLength -= tCyclicBuffer->tMessageHandler.msgLen - sizeof(TRAF_API_Header);
				}
				else
				{
					ArbeApiParseMessage(tCyclicBuffer->pucBufferCyclicLocation, tCyclicBuffer->tMessageHandler.type, tCyclicBuffer->tMessageHandler.msgLen);

					tCyclicBuffer->pucBufferCyclicLocation = tCyclicBuffer->pucBufferCyclicLocation + tCyclicBuffer->tMessageHandler.msgLen;

					*pucStartMessage += tCyclicBuffer->tMessageHandler.msgLen;

					*unBufferLength -= tCyclicBuffer->tMessageHandler.msgLen;

				}



				tCyclicBuffer->tMessageHandler.unRemainMessageLength = 0;

				tCyclicBuffer->tMessageHandler.unHeaderIsCopy = 0;

				tAccumulatorHndler.bNeedMoreData = false;




#ifdef EXTERNAL_USER
				OutputDebugString(" 4,false,false\n");
#endif


			}

		}


	}

	void ArbeApiParseMessage(uint8_t* buffer, int32_t type, uint32_t len)
	{
		switch (type)
		{
			case PointCloud_Output_V1_1:
			{
				// Release frame if last pacakge is missing
				if ((((TPointCloud_V1_1*)buffer)->usMessageNumber < tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unMessageTail) && tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation != tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferStartPoint)
				{
					tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation = tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferStartPoint;
					tAccumulatorHndler.callback(type, (uint8_t*)tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation, tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unTotalMsgLen);
					tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unTotalMsgLen = 0;
					tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unMessageTail = 0;
				}

				// Accumulate pc 
				memcpy(tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation, buffer, len);
				tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unTotalMsgLen += len;
				tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation += len;
				tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unMessageTail++;

				// if buffer is full
				if (tAccumulatorHndler.tCyclicBufferRequstedInformation.unBufferSize < tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unTotalMsgLen)
				{
					tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unTotalMsgLen = 0;
					tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation = tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferStartPoint;
				}
				if (((TPointCloud_V1_1*)buffer)->ucLastPacket == 1)
				{
				
					tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation = tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferStartPoint;
					tAccumulatorHndler.callback(type, (uint8_t*)tAccumulatorHndler.tCyclicBufferRequstedInformation.pucBufferCyclicLocation, tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unTotalMsgLen);

					tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unTotalMsgLen = 0;
					tAccumulatorHndler.tCyclicBufferRequstedInformation.tMessageHandler.unMessageTail = 0;

				}
				break;
			}
		default:
			tAccumulatorHndler.callback(type, (uint8_t*)buffer, len);
			break;

		}
	}

	void CopyHeader(CyclicBuffer* tCyclicBuffer, TRAF_API_Header* pucStartMessage)
	{
		// Set cyclic pointer to start location
		if ((tCyclicBuffer->pucBufferCyclicLocation + pucStartMessage->unLength) >= (tCyclicBuffer->pucBufferStartPoint + tCyclicBuffer->unBufferSize))
		{
			tCyclicBuffer->pucBufferCyclicLocation = tCyclicBuffer->pucBufferStartPoint;
		}

		memcpy(tCyclicBuffer->pucBufferCyclicLocation, pucStartMessage, sizeof(TRAF_API_Header));

		tCyclicBuffer->pucBufferCyclicLocation = tCyclicBuffer->pucBufferCyclicLocation + sizeof(TRAF_API_Header);

		tCyclicBuffer->tMessageHandler.unHeaderIsCopy = 1;

		tAccumulatorHndler.bIsMiddleOfHeaderState = false;

	}

	/****************************************************/







#ifdef __cplusplus
}
#endif
