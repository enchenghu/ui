/*
 * Copyright (C) 2019 Arbe - http://www.arberobotics.com/
 *
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Arbe nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
#pragma once
#include <ctime>

#include "driver/CommonBase.h"
namespace autox {
namespace drivers {
namespace arbe {

const uint16_t CRC_POLY_16 = 0xD175;
const uint16_t CRC_START_16 = 0x0000;
const uint32_t BIN_DATA_WITH_PHASE_SIZE_BYTES = 12;
const uint32_t BIN_DATA_NO_PHASE_SIZE_BYTES = 10;
const uint32_t BIN_DATA_COMPRESSED_WITH_PHASE_SIZE_BYTES = 7;
const uint32_t BIN_DATA_COMPRESSED_NO_PHASE_SIZE_BYTES = 6;
const float POW_TO_DB_RATIO = 3 / 16.0;

/*
 * Prototype list of global functions
 */

typedef struct {
  union {
    uint8_t data[BIN_DATA_WITH_PHASE_SIZE_BYTES];
    struct {
      uint64_t range : 16;
      uint64_t doppler : 16;
      uint64_t azimuth : 16;
      uint64_t elevation : 16;
      uint64_t power : 16;
      uint64_t phase : 16;
    } Fields;
  };
} Target96bit;

typedef struct {
  union {
    uint8_t data[BIN_DATA_NO_PHASE_SIZE_BYTES];
    struct {
      uint64_t range : 16;
      uint64_t doppler : 16;
      uint64_t azimuth : 16;
      uint64_t elevation : 16;
      uint64_t power : 16;
    } Fields;
  };
} Target80bit;

typedef struct {
  union {
    uint8_t data[BIN_DATA_COMPRESSED_WITH_PHASE_SIZE_BYTES];
    struct {
      uint64_t phase : 11;
      uint64_t power : 10;
      uint64_t elevation : 6;
      uint64_t azimuth : 8;
      uint64_t doppler : 12;
      uint64_t range : 9;
    } Fields;
  };
} Target56bit;

typedef struct {
  union {
    uint8_t data[BIN_DATA_COMPRESSED_NO_PHASE_SIZE_BYTES];
    struct {
      uint64_t power : 10;
      uint64_t elevation : 6;
      uint64_t azimuth : 8;
      uint64_t doppler : 12;
      uint64_t range : 9;
      uint64_t dummy : 3;
    } Fields;
  };
} Target48bit;

typedef struct {
  float range;
  float azimuthParam;
  int azimuthNumOfBins;
  float elevationParam;
  int elevationNumOfBins;
} TargetGenericResolution;

typedef struct {
  float phase;
  float power;
  float elevation;
  float azimuth;
  float doppler;
  float range;
} TargetGeneric;

typedef struct {
  double x;
  double y;
  double z;
} TaregtCartesian;

inline uint32_t TARGET_DATA_PAHSE_BIT_MASK(uint32_t x) {
  return ((x) >> 7) & 0x1;
}
inline uint32_t TARGET_DATA_COMPRESSED_BIT_MASK(uint32_t x) {
  return ((x) >> 6) & 0x1;
}

/*****************************************************************************
*	RAF_COM_CALC_Bins2Phy - Convert binary point cloud package to list of
phy points with resolution
** @params
*		Input
*			pointCloudPackage - pointer on begin of the point cloud
pacakge (include header)
*			TargetsBuffer - Allocate memory for physical results,
Max size is 160 * 24 + 24 bytes (Maximum points in pacakge * TargetGenric size)
+ resulotion struct size

*
*		Output
*			TargetsBuffer - Contain resolution and all the physical
info , each point convert to TargetGeneric struct
                                * First:  Resolution represented by
TargetGenericResolution struct
                                * Second: All physical point clouds points
chained , represented by TargetGeneric struct
*			TargetsLength - TargetsBuffer return size
*
* @note		None.
*
******************************************************************************/
void RAF_COM_CALC_Binary2Phy(uint8_t* pointCloudPackage, uint8_t* TargetsBuffer,
                             uint32_t* TargetsLength);

/*****************************************************************************
*	RAF_COM_CALC_Binary2Bins - Convert binary point cloud package to list
of bins points
** @params
*		Input
*			BinaryData - pointer on begin of the point cloud data
(not include header)
*			BinaryDataLength - Length of BinaryData buffer
                        bytesPerTarget - Number of bytes to represent one point
                        BinsBuffer -  Allocate memory for physical results, Max
size is 160 * 24 bytes (Maximum points in pacakge * TargetGenric size)
*
*		Output
*			BinsBuffer - All bins point clouds points chained ,
represented by TargetGeneric struct
*			BinsLength - BinsBuffer return size
*
* @note		None.
*
******************************************************************************/
void RAF_COM_CALC_Binary2Bins(uint8_t* pointCloudPackage,
                              uint8_t* TargetsBuffer, uint32_t* TargetsLength);

void RAF_COM_Bin2cart(uint16_t range_bin, uint16_t azimuth_bin,
                      uint16_t elevation_bin, TPcMetadata* tMetadata,
                      TaregtCartesian* taregtCartesian);

void RAF_COM_CALC_Binary2Cart(uint8_t* pointCloudPackage,
                              uint8_t* TargetsBuffer, uint32_t* TargetsLength);

float RAF_COM_CALC_CalcRangeCoefficient(TRangeCoef range);

float RAF_COM_CALC_CalcRange(int bin, TPcMetadata* meta);

float RAF_COM_CALC_CalcDopplerCoefficient(TDopplerCoef doppler);

float RAF_COM_CALC_CalcDopplerSignBin(int bin, TDopplerCoef doppler);

float RAF_COM_CALC_CalcDoppler(int bin, TPcMetadata* meta);

float RAF_COM_CALC_CalcAzimuthCoefficient(TAzimuthCoef azimuth);

int32_t RAF_COM_CALC_CalcAzimuthSignBin(int bin, TAzimuthCoef azimuth);

float RAF_COM_CALC_CalcSinAlpha(int bin, TPcMetadata* meta);

float RAF_COM_CALC_CalcAlpha(int bin, TPcMetadata* meta);

float RAF_COM_CALC_CalcAzimuth(int Azbin, int Elbin, TPcMetadata* meta);

int32_t RAF_COM_CALC_CalcElevationSignBin(int bin, TElevationCoef elevation);

float RAF_COM_CALC_CalcElevationCoefficient(TElevationCoef elevation);

float RAF_COM_CALC_CalcSinTheta(uint16_t el_bin, TPcMetadata* metadata);

float RAF_COM_CALC_CalcElevation(double bin, TPcMetadata* meta);

float RAF_COM_CALC_CalcPhase(int bin);

float RAF_COM_CALC_CalcPower(int power);

float RAF_COM_CALC_CalcPowerEqulizer(float power, int rangeBin,
                                     float rangeResInDb);

float RAF_COM_CALC_ClacRangeResInDb(TPcMetadata* meta);

uint8_t RAF_COM_CLAC_Is4d(TPcMetadata* ptMetadata);

uint8_t RAF_COM_CLAC_GetFrameNumber(uint8_t* pointCloudPackage);

uint32_t RAF_COM_CALC_GetBytesLengthOfTraget(uint32_t unTargetFormat);

uint16_t Float2Fixed(float floatingNum, uint8_t fractionalBits);

float Fixed2Float(uint16_t fixedNum, uint8_t fractionalBits);

float Fixed32toFloat(int32_t fixedNum, uint8_t fractionalBits);

float Fixed64toFloat(int64_t fixedNum, uint8_t fractionalBits);
uint64_t GetSystemTime();

}  // namespace arbe
}  // namespace drivers
}  // namespace autox
