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
#include "driver/Utils.h"

#include <cmath>
#include <cstdarg>
#include <cstdbool>
#include <cstdint>
#include <cstdio>
#include <cstring>

#include "driver/CommonBase.h"
namespace autox {
namespace drivers {
namespace arbe {

/************ STATIC FILEDS AND FUNCTION *************/

static const uint8_t rangeBinToDb[128] = {
    0x0,  0xC,  0x13, 0x18, 0x1C, 0x1F, 0x22, 0x24, 0x26, 0x28, 0x2A, 0x2B,
    0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x36, 0x37,
    0x38, 0x39, 0x39, 0x3A, 0x3A, 0x3B, 0x3C, 0x3C, 0x3D, 0x3D, 0x3E, 0x3E,
    0x3F, 0x3F, 0x40, 0x40, 0x41, 0x41, 0x41, 0x42, 0x42, 0x43, 0x43, 0x43,
    0x44, 0x44, 0x44, 0x45, 0x45, 0x45, 0x46, 0x46, 0x46, 0x47, 0x47, 0x47,
    0x47, 0x48, 0x48, 0x48, 0x49, 0x49, 0x49, 0x49, 0x4A, 0x4A, 0x4A, 0x4A,
    0x4B, 0x4B, 0x4B, 0x4B, 0x4B, 0x4C, 0x4C, 0x4C, 0x4C, 0x4D, 0x4D, 0x4D,
    0x4D, 0x4D, 0x4E, 0x4E, 0x4E, 0x4E, 0x4E, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F,
    0x4F, 0x50, 0x50, 0x50, 0x50, 0x50, 0x51, 0x51, 0x51, 0x51, 0x51, 0x51,
    0x51, 0x52, 0x52, 0x52, 0x52, 0x52, 0x52, 0x53, 0x53, 0x53, 0x53, 0x53,
    0x53, 0x53, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54,
};

/*****************************************/

/************ LOCAL FUNCTION *************/

void InnerGetBinsFromPointClouds_1_1(uint8_t* pointCloudBinData,
                                     uint8_t* Results, uint32_t* ResultsLength);

void InnerGetTargetsFromPointClouds_1_1(uint8_t* pointCloudBinData,
                                        uint8_t* Results,
                                        uint32_t* ResultsLength);

TargetGeneric GetTarget(uint8_t* data, uint32_t NumberOfbytesTarget,
                        TPcMetadata* pcMetadata);

TargetGeneric GetBin(uint8_t* data, uint32_t NumberOfbytesTarget);

/*****************************************/
float RAF_COM_CALC_CalcRangeCoefficient(TRangeCoef range) {
  uint16_t complete = range.tFields.integral;

  uint16_t fraction = range.tFields.fraction;

  return complete + fraction / static_cast<float>(1 << 16);
}

uint32_t RAF_COM_CALC_CalcPhysicalRangeZoom(TRangeCoef range) {
  if (range.tFields.range_zoom == 0x0) {
    return 0;
  } else if (range.tFields.range_zoom == 0x0) {
    return 0;
  } else if (range.tFields.range_zoom == 0x1) {
    return 2;
  } else if (range.tFields.range_zoom == 0x3) {
    return 4;
  } else if (range.tFields.range_zoom == 0x7) {
    return 8;
  } else if (range.tFields.range_zoom == 0xF) {
    return 16;
  } else {
    return 0;
  }
}

float RAF_COM_CALC_CalcRange(int bin, TPcMetadata* meta) {
  float coef = RAF_COM_CALC_CalcRangeCoefficient(meta->range);

  if (meta->doppler.tFields.isFine == 1) {
    if (bin > 1) {
      return (bin -
              ((RAF_COM_CALC_CalcPhysicalRangeZoom(meta->range) - 1) / 2)) *
             coef;
    } else {
      return 0;
    }
  }

  return bin * coef;
}

float RAF_COM_CALC_CalcDopplerCoefficient(TDopplerCoef doppler) {
  return doppler.tFields.integral +
         (doppler.tFields.fraction / static_cast<float>(1 << 16));
}

float RAF_COM_CALC_CalcDopplerSignBin(int bin, TDopplerCoef doppler) {
  float _doppler;

  if (doppler.tFields.isFine) {
    _doppler = static_cast<float>((bin + 0x800) % 0x1000);

    _doppler -= 0x800;
  } else {
    _doppler = static_cast<float>((bin + 0x200) % 0x400);

    _doppler -= 0x200;
  }

  return (-1) * _doppler;
}

float RAF_COM_CALC_CalcDoppler(int bin, TPcMetadata* meta) {
  float doppler = RAF_COM_CALC_CalcDopplerSignBin(bin, meta->doppler);

  doppler *= RAF_COM_CALC_CalcDopplerCoefficient(meta->doppler);

  return doppler;
}

float RAF_COM_CALC_CalcAzimuthCoefficient(TAzimuthCoef azimuth) {
  return azimuth.tFields.fraction / static_cast<float>(1 << 16);
}

int32_t RAF_COM_CALC_CalcAzimuthSignBin(int bin, TAzimuthCoef azimuth) {
  return (-1) *
         (bin - (azimuth.tFields.fftRange / 2) + azimuth.tFields.padding);
}

float RAF_COM_CALC_CalcSinAlpha(int bin, TPcMetadata* meta) {
  int32_t signedBin = RAF_COM_CALC_CalcAzimuthSignBin(bin, meta->azimuth);
  float coef = RAF_COM_CALC_CalcAzimuthCoefficient(meta->azimuth);
  return signedBin * coef;
}

float RAF_COM_CALC_CalcAzimuth(int Azbin, int Elbin, TPcMetadata* metadata) {
  float azimuth = 0;

  float SinAlpha = RAF_COM_CALC_CalcSinAlpha(Azbin, metadata);

  float sinEl = RAF_COM_CALC_CalcSinTheta(Elbin, metadata);
  azimuth = asin(SinAlpha / sqrt(1 - sinEl * sinEl));

  return azimuth;
}

float RAF_COM_CALC_CalcAlpha(int bin, TPcMetadata* meta) {
  float SinAlpha = RAF_COM_CALC_CalcSinAlpha(bin, meta);

  return asin(SinAlpha);
}

int32_t RAF_COM_CALC_CalcElevationSignBin(int bin, TElevationCoef elevation) {
  return (-1) *
         (bin - (elevation.tFields.fftRange) / 2 + elevation.tFields.padding);
}

float RAF_COM_CALC_CalcElevationCoefficient(TElevationCoef elevation) {
  return (elevation.tFields.fraction / static_cast<float>(1 << 16));
}

float RAF_COM_CALC_CalcSinTheta(uint16_t el_bin, TPcMetadata* metadata) {
  int32_t elfixedBin =
      RAF_COM_CALC_CalcElevationSignBin(el_bin, metadata->elevation);
  float SinEl = RAF_COM_CALC_CalcElevationCoefficient(metadata->elevation);
  return SinEl * elfixedBin;
}

float RAF_COM_CALC_CalcElevation(double bin, TPcMetadata* meta) {
  float SinEl = RAF_COM_CALC_CalcSinTheta(bin, meta);
  return asin(SinEl);
}

float RAF_COM_CALC_CalcPhase(int bin) { return static_cast<float>(bin); }

float RAF_COM_CALC_CalcPower(int power) {
  return static_cast<float>(power * POW_TO_DB_RATIO);
}

float RAF_COM_CALC_CalcPowerEqulizer(float power, int rangeBin,
                                     float rangeResInDb) {
  return power + rangeResInDb + rangeBinToDb[rangeBin];
}

float RAF_COM_CALC_ClacRangeResInDb(TPcMetadata* meta) {
  uint16_t complete = meta->range.tFields.integral;

  uint16_t fraction = meta->range.tFields.fraction;

  float coef = complete + fraction / static_cast<float>(1 << 16);

  return 40 * log10(coef);
}

uint8_t RAF_COM_CLAC_Is4d(TPcMetadata* ptMetadata) {
  return (ptMetadata->doppler.tFields.isFine);
}

uint8_t RAF_COM_CLAC_GetFrameNumber(uint8_t* pointCloudPackage) {
  return (
      reinterpret_cast<TPointCloud_V1_1*>(pointCloudPackage)->ucTrgtFmtFrmType &
      0x3f);
}

void RAF_COM_CALC_Binary2Phy(uint8_t* pointCloudPackage, uint8_t* TargetsBuffer,
                             uint32_t* TargetsLength) {
  *TargetsLength = 0;
  if (((reinterpret_cast<TRAF_API_Header*>(pointCloudPackage))->usType) ==
      PointCloud_Output_V1_1) {
    InnerGetTargetsFromPointClouds_1_1(pointCloudPackage, TargetsBuffer,
                                       TargetsLength);
  }
}

void RAF_COM_CALC_Binary2Bins(uint8_t* pointCloudPackage,
                              uint8_t* TargetsBuffer, uint32_t* TargetsLength) {
  *TargetsLength = 0;
  if (((reinterpret_cast<TRAF_API_Header*>(pointCloudPackage))->usType) ==
      PointCloud_Output_V1_1) {
    InnerGetBinsFromPointClouds_1_1(pointCloudPackage, TargetsBuffer,
                                    TargetsLength);
  }
}

void RAF_COM_Bin2cart(uint16_t range_bin, uint16_t azimuth_bin,
                      uint16_t elevation_bin, TPcMetadata* tMetadata,
                      TaregtCartesian* taregtCartesian) {
  float range = RAF_COM_CALC_CalcRange(range_bin, tMetadata);

  taregtCartesian->x =
      range * RAF_COM_CALC_CalcSinAlpha(azimuth_bin, tMetadata);
  taregtCartesian->z =
      range * RAF_COM_CALC_CalcSinTheta(elevation_bin, tMetadata);
  taregtCartesian->y =
      sqrt(range * range - taregtCartesian->z * taregtCartesian->z -
           taregtCartesian->x * taregtCartesian->x);
}

void RAF_COM_CALC_Binary2Cart(uint8_t* pointCloudPackage,
                              uint8_t* TargetsBuffer, uint32_t* TargetsLength) {
  *TargetsLength = 0;

  uint32_t i = 0;
  TargetGeneric targetGeneric;
  TaregtCartesian taregtCartesian;

  uint32_t unTargetFormat =
      (reinterpret_cast<TPointCloud_V1_1*>(pointCloudPackage))
          ->ucTrgtFmtFrmType;

  uint32_t bytesPerTarget = RAF_COM_CALC_GetBytesLengthOfTraget(unTargetFormat);

  for (i = sizeof(TPointCloud_V1_1);
       i < (reinterpret_cast<TPointCloud_V1_1*>(pointCloudPackage))->unLength;
       i += bytesPerTarget) {
    targetGeneric = GetBin(pointCloudPackage + i, bytesPerTarget);
    RAF_COM_Bin2cart(
        targetGeneric.range, targetGeneric.azimuth, targetGeneric.elevation,
        &(reinterpret_cast<TPointCloud_V1_1*>(pointCloudPackage))->tMetadata,
        &taregtCartesian);

    memcpy(TargetsBuffer, &taregtCartesian, sizeof(taregtCartesian));
    TargetsBuffer = TargetsBuffer + sizeof(taregtCartesian);
    *TargetsLength += sizeof(taregtCartesian);
  }
}

void InnerGetBinsFromPointClouds_1_1(uint8_t* pointCloudBinData,
                                     uint8_t* Results,
                                     uint32_t* ResultsLength) {
  uint32_t i = 0;
  TargetGeneric targetGeneric;

  uint32_t unTargetFormat =
      (reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData))
          ->ucTrgtFmtFrmType;

  uint32_t bytesPerTarget = RAF_COM_CALC_GetBytesLengthOfTraget(unTargetFormat);

  for (i = sizeof(TPointCloud_V1_1);
       i < (reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData))->unLength;
       i += bytesPerTarget) {
    targetGeneric = GetBin(pointCloudBinData + i, bytesPerTarget);
    memcpy(Results, &targetGeneric, sizeof(targetGeneric));

    Results = Results + sizeof(targetGeneric);
    *ResultsLength += sizeof(targetGeneric);
  }
}

void InnerGetTargetsFromPointClouds_1_1(uint8_t* pointCloudBinData,
                                        uint8_t* Results,
                                        uint32_t* ResultsLength) {
  TargetGenericResolution resolution;

  uint32_t unTargetFormat =
      (reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData))
          ->ucTrgtFmtFrmType;

  uint32_t bytesPerTarget = RAF_COM_CALC_GetBytesLengthOfTraget(unTargetFormat);

  // calculate resolution
  resolution.range =
      RAF_COM_CALC_CalcRange(
          1, &(reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData)
                   ->tMetadata)) -
      RAF_COM_CALC_CalcRange(
          0,
          &(reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData)->tMetadata));
  resolution.azimuthParam =
      (reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData)
           ->tMetadata.azimuth.tFields.fraction /
       static_cast<float>(1 << 16));
  resolution.azimuthNumOfBins =
      (reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData)
           ->tMetadata.azimuth.tFields.fftRange);
  resolution.elevationParam =
      (reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData)
           ->tMetadata.elevation.tFields.fraction /
       static_cast<float>(1 << 16));
  resolution.elevationNumOfBins =
      (reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData)
           ->tMetadata.elevation.tFields.fftRange);

  // insert resolution struct before physical points
  memcpy(Results, reinterpret_cast<uint8_t*>(&resolution),
         sizeof(TargetGenericResolution));
  Results += sizeof(resolution);
  *ResultsLength += sizeof(resolution);

  uint32_t i = 0;
  TargetGeneric targetGeneric;

  for (i = 0;
       i < (reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData)->unLength -
            sizeof(TPointCloud_V1_1));
       i += bytesPerTarget) {
    targetGeneric = GetTarget(
        pointCloudBinData + sizeof(TPointCloud_V1_1) + i, bytesPerTarget,
        &(reinterpret_cast<TPointCloud_V1_1*>(pointCloudBinData)->tMetadata));
    memcpy(Results, &targetGeneric, sizeof(targetGeneric));

    Results = Results + sizeof(targetGeneric);
    *ResultsLength = *ResultsLength + sizeof(targetGeneric);
  }
}

TargetGeneric GetTarget(uint8_t* data, uint32_t NumberOfbytesTarget,
                        TPcMetadata* pcMetadata) {
  TargetGeneric targetGeneric;

  if (NumberOfbytesTarget == BIN_DATA_COMPRESSED_WITH_PHASE_SIZE_BYTES) {
    Target56bit* target56bit = reinterpret_cast<Target56bit*>(data);
    targetGeneric.azimuth = RAF_COM_CALC_CalcAzimuth(
        static_cast<double>(target56bit->Fields.azimuth),
        static_cast<double>(target56bit->Fields.elevation), pcMetadata);
    targetGeneric.doppler = RAF_COM_CALC_CalcDoppler(
        static_cast<int>(target56bit->Fields.doppler), pcMetadata);
    targetGeneric.elevation = RAF_COM_CALC_CalcElevation(
        static_cast<double>(target56bit->Fields.elevation), pcMetadata);
    targetGeneric.phase =
        RAF_COM_CALC_CalcPhase(static_cast<int>(target56bit->Fields.phase));
    targetGeneric.power =
        RAF_COM_CALC_CalcPower(static_cast<int>(target56bit->Fields.power));
    targetGeneric.range = RAF_COM_CALC_CalcRange(
        static_cast<int>(target56bit->Fields.range), pcMetadata);
  } else if (NumberOfbytesTarget == BIN_DATA_COMPRESSED_NO_PHASE_SIZE_BYTES) {
    Target48bit* target48bit = reinterpret_cast<Target48bit*>(data);
    targetGeneric.azimuth = RAF_COM_CALC_CalcAzimuth(
        static_cast<double>(target48bit->Fields.azimuth),
        static_cast<double>(target48bit->Fields.elevation), pcMetadata);
    targetGeneric.doppler = RAF_COM_CALC_CalcDoppler(
        static_cast<int>(target48bit->Fields.doppler), pcMetadata);
    targetGeneric.elevation = RAF_COM_CALC_CalcElevation(
        static_cast<double>(target48bit->Fields.elevation), pcMetadata);
    targetGeneric.phase = 0;
    targetGeneric.power =
        RAF_COM_CALC_CalcPower(static_cast<int>(target48bit->Fields.power));
    targetGeneric.range = RAF_COM_CALC_CalcRange(
        static_cast<int>(target48bit->Fields.range), pcMetadata);
  } else if (NumberOfbytesTarget == BIN_DATA_WITH_PHASE_SIZE_BYTES) {
    Target96bit* target96bit = reinterpret_cast<Target96bit*>(data);
    targetGeneric.azimuth = RAF_COM_CALC_CalcAzimuth(
        static_cast<double>(target96bit->Fields.azimuth),
        static_cast<double>(target96bit->Fields.elevation), pcMetadata);
    targetGeneric.doppler = RAF_COM_CALC_CalcDoppler(
        static_cast<int>(target96bit->Fields.doppler), pcMetadata);
    targetGeneric.elevation = RAF_COM_CALC_CalcElevation(
        static_cast<double>(target96bit->Fields.elevation), pcMetadata);
    targetGeneric.phase =
        RAF_COM_CALC_CalcPhase(static_cast<int>(target96bit->Fields.phase));
    targetGeneric.power =
        RAF_COM_CALC_CalcPower(static_cast<int>(target96bit->Fields.power));
    targetGeneric.range = RAF_COM_CALC_CalcRange(
        static_cast<int>(target96bit->Fields.range), pcMetadata);
  } else if (NumberOfbytesTarget == BIN_DATA_NO_PHASE_SIZE_BYTES) {
    Target80bit* target80bit = reinterpret_cast<Target80bit*>(data);
    targetGeneric.azimuth = RAF_COM_CALC_CalcAzimuth(
        static_cast<double>(target80bit->Fields.azimuth),
        static_cast<double>(target80bit->Fields.elevation), pcMetadata);
    targetGeneric.doppler = RAF_COM_CALC_CalcDoppler(
        static_cast<int>(target80bit->Fields.doppler), pcMetadata);
    targetGeneric.elevation = RAF_COM_CALC_CalcElevation(
        static_cast<double>(target80bit->Fields.elevation), pcMetadata);
    targetGeneric.phase = 0;
    targetGeneric.power =
        RAF_COM_CALC_CalcPower(static_cast<int>(target80bit->Fields.power));
    targetGeneric.range = RAF_COM_CALC_CalcRange(
        static_cast<int>(target80bit->Fields.range), pcMetadata);
  }
  return targetGeneric;
}

TargetGeneric GetBin(uint8_t* data, uint32_t NumberOfbytesTarget) {
  TargetGeneric targetGeneric;

  if (NumberOfbytesTarget == BIN_DATA_COMPRESSED_WITH_PHASE_SIZE_BYTES) {
    Target56bit* target56bit = reinterpret_cast<Target56bit*>(data);
    targetGeneric.azimuth = static_cast<float>(target56bit->Fields.azimuth);
    targetGeneric.doppler = static_cast<float>(target56bit->Fields.doppler);
    targetGeneric.elevation = static_cast<float>(target56bit->Fields.elevation);
    targetGeneric.phase = static_cast<float>(target56bit->Fields.phase);
    targetGeneric.power = static_cast<float>(target56bit->Fields.power);
    targetGeneric.range = static_cast<float>(target56bit->Fields.range);
  } else if (NumberOfbytesTarget == BIN_DATA_COMPRESSED_NO_PHASE_SIZE_BYTES) {
    Target48bit* target48bit = reinterpret_cast<Target48bit*>(data);
    targetGeneric.azimuth = static_cast<float>(target48bit->Fields.azimuth);
    targetGeneric.doppler = static_cast<float>(target48bit->Fields.doppler);
    targetGeneric.elevation = static_cast<float>(target48bit->Fields.elevation);
    targetGeneric.phase = 0;
    targetGeneric.power = static_cast<float>(target48bit->Fields.power);
    targetGeneric.range = static_cast<float>(target48bit->Fields.range);
  } else if (NumberOfbytesTarget == BIN_DATA_WITH_PHASE_SIZE_BYTES) {
    Target96bit* target96bit = reinterpret_cast<Target96bit*>(data);
    targetGeneric.azimuth = static_cast<float>(target96bit->Fields.azimuth);
    targetGeneric.doppler = static_cast<float>(target96bit->Fields.doppler);
    targetGeneric.elevation = static_cast<float>(target96bit->Fields.elevation);
    targetGeneric.phase = static_cast<float>(target96bit->Fields.phase);
    targetGeneric.power = static_cast<float>(target96bit->Fields.power);
    targetGeneric.range = static_cast<float>(target96bit->Fields.range);
  } else if (NumberOfbytesTarget == BIN_DATA_NO_PHASE_SIZE_BYTES) {
    Target80bit* target80bit = reinterpret_cast<Target80bit*>(data);
    targetGeneric.azimuth = static_cast<float>(target80bit->Fields.azimuth);
    targetGeneric.doppler = static_cast<float>(target80bit->Fields.doppler);
    targetGeneric.elevation = static_cast<float>(target80bit->Fields.elevation);
    targetGeneric.phase = 0;
    targetGeneric.power = static_cast<float>(target80bit->Fields.power);
    targetGeneric.range = static_cast<float>(target80bit->Fields.range);
  }

  return targetGeneric;
}

uint32_t RAF_COM_CALC_GetBytesLengthOfTraget(uint32_t unTargetFormat) {
  uint32_t bytesPerTarget = 0;

  uint32_t unIsCompressed = TARGET_DATA_COMPRESSED_BIT_MASK(unTargetFormat);

  uint32_t unIncludePhase = TARGET_DATA_PAHSE_BIT_MASK(unTargetFormat);

  if (unIsCompressed == 0) {
    if (unIncludePhase == 1) {
      return BIN_DATA_WITH_PHASE_SIZE_BYTES;
    } else {
      return BIN_DATA_NO_PHASE_SIZE_BYTES;
    }
  } else if (unIsCompressed == 1) {
    if (unIncludePhase == 1) {
      return BIN_DATA_COMPRESSED_WITH_PHASE_SIZE_BYTES;
    } else {
      return BIN_DATA_COMPRESSED_NO_PHASE_SIZE_BYTES;
    }
  }

  return bytesPerTarget;
}

/*****************************************************************************
 *	Float2Fixed - convert floating to fixed point
 ** @params
 *		Input
 *			floatingNum
 *			fractionalBits - num of bits right of the fixed point
 *
 *		Output
 *			None
 *
 * @return		 fixed point value
 *matchedPCList
 * @note		None.
 *
 ******************************************************************************/
uint16_t Float2Fixed(float floatingNum, uint8_t fractionalBits) {
  float temp =
      floatingNum *
      (1
       << fractionalBits);  // floating num * 2^(num of bits on the right side)
  return (uint16_t)temp;
}

/*****************************************************************************
 *	Float2Fixed - convert floating to fixed point
 ** @params
 *		Input
 *			floatingNum
 *			fractionalBits - num of bits right of the fixed point
 *
 *		Output
 *			None
 *
 * @return		 fixed point value
 *matchedPCList
 * @note		None.
 *
 ******************************************************************************/
float Fixed2Float(uint16_t fixedNum, uint8_t fractionalBits) {
  uint8_t isNeg = 0;
  float result;
  if (fixedNum & 0x8000) {
    isNeg = 1;
    fixedNum = -fixedNum;
  }
  result = (static_cast<float>(fixedNum)) / (1 << fractionalBits);
  result = (isNeg) ? -result : result;
  return result;
}

/*****************************************************************************
 *	Float2Fixed - convert 32-bit fixed point to float
 ** @params
 *		Input
 *			fixedNum
 *			fractionalBits - num of bits right of the fixed point
 *
 *		Output
 *			None
 *
 * @return		 floating point value
 *matchedPCList
 * @note		None.
 *
 ******************************************************************************/
float Fixed32toFloat(int32_t fixedNum, uint8_t fractionalBits) {
  uint8_t isNeg = 0;
  float result;
  if (fixedNum & 0x80000000) {
    isNeg = 1;
    fixedNum = -fixedNum;
  }
  result = (static_cast<float>(fixedNum)) / (1 << fractionalBits);
  result = (isNeg) ? -result : result;
  return result;
}

/*****************************************************************************
 *	Float2Fixed - convert 64-bit fixed point to float
 ** @params
 *		Input
 *			fixedNum
 *			fractionalBits - num of bits right of the fixed point
 *
 *		Output
 *			None
 *
 * @return		 floating point value
 *matchedPCList
 * @note		None.
 *
 ******************************************************************************/
float Fixed64toFloat(int64_t fixedNum, uint8_t fractionalBits) {
  uint8_t isNeg = 0;
  float result;
  if (fixedNum & 0x8000000000000000) {
    isNeg = 1;
    fixedNum = -fixedNum;
  }
  result = (static_cast<float>(fixedNum)) / (1 << fractionalBits);
  result = (isNeg) ? -result : result;
  return result;
}

uint64_t GetSystemTime() {
  time_t tt;
  time(&tt);
  tt = tt + 8 * 3600;  // transform the time zone
  gmtime(&tt);
  return tt;
}
}  // namespace arbe
}  // namespace drivers
}  // namespace autox
