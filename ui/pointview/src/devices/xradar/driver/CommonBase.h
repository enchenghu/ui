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

#include <cstdint>
namespace autox {
namespace drivers {
namespace arbe {

const uint16_t PACKET_PREFIX = 0xA55A;
const uint8_t PACKET_PREFIX_SHIFT = 16;
const uint8_t POINT_CLOUD_METADATA_VER = 1;
const uint32_t MAX_FRAME_PER_SEQUENCE = 50;



/*######################### ENUMERATORS ###############################*/

typedef enum {
  PErr_OK = 0,
  PErr_CRC,
  PErr_LENGTH,
  PErr_TYPE,
  // In case there was a request that conflicts with the mode we
  // are working at this time : raw was requested while we are
  // running p.Cloude without asking for stop
  PErr_CONFLICT,
  PErr_OVERFLOW,
  PErr_DATALOSS,
  PErr_DONT_SEND_RESP,
  Perr_UNSUPPORTED
} EParseErrs;

typedef enum {
  SystemTime = 0,
  FwVersion,
  FpgaVersion,
  SequenceNumber,
  RadarErrorNum,
  TaskState,
  ApiInterfaceErrorNum,
  NucStatus,
  VerifyApiCommonVersion,
  GetThreshold  // wHAT???
} EStatusOpcode;

typedef enum {
  Raw_Output = 1,  // bypass
  PointCloud_Output,
  RegOutput,
  MemRdOutput,
  Status_Output = 6,
  Response_Output,
  Slam_Output = 10,
  DspJob_Output,
  PointCloudAndSlam,
  PointCloud_Output_V1_1,
  Slam_Output_V1_1,
  Range_Output,
  MemoryOperation_Output,
  RficOperation_Output,
  Histogram_Output,
  RangeThreshold_output,
  RdPairRec_Output,
  last_output_type,
  PointCloud_Output_xRadar_V1_1 = 30
} EOutputType;

typedef enum {
  IdleSeq = 0,
  CoarseShortSeq,
  CoarseMidSeq,
  CoarseLongSeq,
  FineShortSeq,
  FineMidSeq,
  FineLongSeq,
  CalibrationSeq,
  LabSeq,
  DelayCalibrationSeq,
  CoarseUltraLongSeq,
  FineUltraLongSeq,
  LastSeq,
} ESeuqenceType;

typedef enum {
  FrameType0 = 0,
  FrameType1,
  FrameType2,
  FrameType3,
  FrameType4,
  FrameType5,
  FrameType6,
  FrameType7,
  FrameType8,
  FrameType9,
  FrameType10,
  FrameType11,
  FrameType12,
  FrameType13,
  FrameType14,
  FrameType15,
  FrameType16,
  FrameType17,
  FrameType18,
  FrameType19,
  FrameType20,
  FrameType21,
  FrameType22,
  FrameType23,
  FrameTypeLast
} EFrameTypeUser;

/*###########################################################################*/

/*######################### BASE STRUCTURES ###############################*/

typedef struct {
  uint16_t usPrefix;
  uint16_t usType;
  uint32_t unLength;
  uint32_t unTimeLsb;
  uint32_t unTimeMsb;
  uint32_t unMessageNumber;
} TRAF_API_Header;

typedef struct {
  uint32_t usStatus;  // 0 - Success or error number
} TResponseInfo;
typedef struct {
  TRAF_API_Header tHeader;
  TResponseInfo tResponseInfo;
} TResponseMsg;

typedef struct {
  uint32_t fraction : 16;
  uint32_t integral : 10;
  uint32_t range_zoom : 6;
} TRangeFields;
typedef struct {
  union {
    TRangeFields tFields;
    uint32_t value;
  };
} TRangeCoef;

typedef struct {
  uint32_t fraction : 16;
  uint32_t integral : 14;
  uint32_t isMbcFix : 1;
  uint32_t isFine : 1;
} TDopplerFields;
typedef struct {
  union {
    TDopplerFields tFields;
    uint32_t value;
  };
} TDopplerCoef;

typedef struct {
  uint32_t fraction : 16;
  uint32_t padding : 6;
  uint32_t fftRange : 10;
} TAzimuthFields;
typedef struct {
  union {
    TAzimuthFields tFields;
    uint32_t value;
  };
} TAzimuthCoef;

typedef struct {
  uint32_t fraction : 16;
  uint32_t padding : 6;
  uint32_t fftRange : 10;
} TElevationFields;
typedef struct {
  union {
    TElevationFields tFields;
    uint32_t value;
  };
} TElevationCoef;

typedef struct {
  TRAF_API_Header tHeader;
  uint32_t usTotalSamples;
  uint32_t unSampleNumber;
  uint8_t ucIsLastPacket;
  uint8_t ucParams[3];
  uint32_t unCrc;
} TRawOutput;
/* RAW_METADATA_VERSION	above should be incremented if making changes in
 * following structs*/
typedef struct {
  uint32_t unMagicNum;
  uint32_t usHeaderVersion;
  uint32_t unSampleDataOffset;
  uint32_t unNumberOfSamples;
  uint32_t unSwVersion;
  uint32_t unBitstreamVersion;
  uint32_t unRadarNum;
  uint32_t unCalibrationVersion;
  uint32_t unTimeUtc;
  uint16_t usFramesInSeq;
  uint16_t reserverd;
  uint32_t unMetadataOffset[MAX_FRAME_PER_SEQUENCE];
  uint32_t unMetadataSize[MAX_FRAME_PER_SEQUENCE];
  float afPtatResults[2 * 3 + 4 * 2 + 16];  // NEED TO USE A DEFINE FOR SIZE!!
} TRawMetadataHeader;

typedef struct {
  uint32_t unFrameTypeNumber;
  uint32_t unChirpBandwidth;
  uint32_t unRepetition;
  uint32_t unNumOfChirps;
  uint32_t unNumOfRxChannels;
  uint32_t unTChirp;
  uint32_t unDelayBetweenChirps;
} TRawMetadataFrame;
/* RAW_METADATA_VERSION	above should be incremented if making changes in
 * structs above*/

typedef struct {
  TRangeCoef range;
  TDopplerCoef doppler;
  TAzimuthCoef azimuth;
  TElevationCoef elevation;
} TPcMetadata;

typedef struct {
  uint16_t usPrefix;
  uint16_t usType;
  uint16_t usLength;
  uint16_t usTime_lsb;
  uint16_t usTime_msb;
  uint16_t usFrameCounter;
  uint16_t usMessageNumber;
  uint8_t ucLastPacket;
  uint8_t ucTrgtFmtFrmType;
  TPcMetadata tMetadata;
  uint16_t usHeaderCrc;
  uint16_t usPayloadCrc;
} TPointCloud;


typedef struct {
  uint16_t usPrefix;
  uint16_t usType;
} MandatoryHeader;

typedef struct {
  uint16_t usPrefix;
  uint16_t usType;
  uint32_t unLength;
  uint32_t unTimeLsb;
  uint32_t unTimeMsb;
  uint16_t usFrameCounter;
  // packet counter
  uint16_t usMessageNumber;
  uint8_t ucLastPacket;
  uint8_t ucTrgtFmtFrmType;
  uint16_t usCrdCount;
  TPcMetadata tMetadata;
  uint16_t usHeaderCrc;
  uint16_t usPayloadCrc;
} TPointCloud_V1_1;

typedef struct FrameHandlerInfo_s {
  uint32_t unFrameNumber;
  uint32_t unNumberOfPoints;
  uint32_t unFrameType;
} TFrameHandlerInfo;

typedef struct {
  TRAF_API_Header tHeader;
  TFrameHandlerInfo tFrameInfo;
  uint32_t ucIsLastPacket;
  uint32_t unCrc;
} THistogramOutput;

typedef struct TRangeThresholdInfo {
  uint32_t unFrameNumber;
  uint32_t unRangePartition;
} TRangeThresholdInfo;

typedef struct {
  TRAF_API_Header tHeader;
  TRangeThresholdInfo tFrameInfo;
  uint32_t unIsLastPacket;
  uint32_t unCrc;
} TRangeThreseholdOutput;

typedef struct {
  uint16_t usFrameNumber;
  uint8_t RFU1[10];
  uint16_t usHostHeading;
  uint16_t usHostHeadingConf;
  uint16_t usHostVelocity;
  uint16_t usHostVelocityConf;
  uint16_t usHost_w;
  uint8_t RFU2[4];
} TSlamMetadata;

typedef struct {
  uint16_t prefix;
  uint16_t type;
  uint32_t length;
  uint32_t time;
  uint16_t frame_counter;
  uint16_t packet_counter;  // in frame
  uint8_t last_packet;
  uint8_t format;
  TSlamMetadata meta_data;
  uint16_t header_crc;
  uint16_t payload_crc;
} TSlamHeader;

typedef struct {
  uint16_t prefix;
  uint16_t type;
  uint32_t length;
  uint32_t unTimeLsb;
  uint32_t unTimeMsb;
  uint16_t frame_counter;
  uint16_t packet_counter;  // in frame
  uint8_t last_packet;
  uint8_t format;
  TSlamMetadata meta_data;
  uint16_t header_crc;
  uint16_t payload_crc;
} TSlamHeader_V1_1;

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t z;
  uint8_t dx;
  uint8_t dy;
  uint8_t dz;
  uint8_t rfu1;
} TSlamPos;

typedef struct {
  uint16_t x_dot;
  uint16_t y_dot;
  uint8_t dx_dot;
  uint8_t dy_dot;
} TSlamVelocity;

typedef struct {
  // Cartesian size
  uint16_t scale_x;
  uint16_t scale_y;
  uint16_t scale_z;
  uint8_t scale_x_conf;
  uint8_t scale_y_conf;
  uint8_t scale_z_conf;
  uint8_t orientation_conf;
  uint16_t orientation;
} TSlamBox;

typedef struct {
  uint16_t ID;
  uint16_t obj_class;
  uint16_t class_conf;
  TSlamPos position;
  TSlamVelocity velocity;
  TSlamBox bounding_box;
  uint8_t age;
  uint8_t status;
  uint8_t last_frame_update;
  uint8_t side_lobe_conf;
  uint8_t rfu2;
  uint8_t rfu3;
} TSlamObj;

typedef struct {
  uint32_t unRangeBin;
  int32_t unDopplerBin;
  uint32_t unZoom;
  uint32_t unFrameTypeNumber;
} TRdPairBinding;

typedef struct {
  TRAF_API_Header tRafHeader;
  TRdPairBinding tRdBindingConfig;
  uint32_t unRawHeaderOffset;
  uint32_t unRdDataOffset;
  TRawMetadataHeader tRawMetadata;
  TRawMetadataFrame tRawFrameMetada;  // 1 frame only in this case
} TRdPairHeader;
}  // namespace arbe
}  // namespace drivers
}  // namespace autox
