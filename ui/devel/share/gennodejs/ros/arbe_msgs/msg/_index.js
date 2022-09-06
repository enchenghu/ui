
"use strict";

let arbeEgoVelParams = require('./arbeEgoVelParams.js');
let arbeTAlgoDetectorThrOutput = require('./arbeTAlgoDetectorThrOutput.js');
let arbeThresholds = require('./arbeThresholds.js');
let arbeRdInclination = require('./arbeRdInclination.js');
let arbeClassFrameResponses = require('./arbeClassFrameResponses.js');
let arbeDtExceedTh = require('./arbeDtExceedTh.js');
let arbeFrameTypes = require('./arbeFrameTypes.js');
let arbeSlamDisplaySettings = require('./arbeSlamDisplaySettings.js');
let arbeBoolWithTime = require('./arbeBoolWithTime.js');
let arbeFusionQuery = require('./arbeFusionQuery.js');
let arbeTAlgoDetectorThrInfo = require('./arbeTAlgoDetectorThrInfo.js');
let arbePolarFreeSpacePoly = require('./arbePolarFreeSpacePoly.js');
let arbePcMetadata = require('./arbePcMetadata.js');
let arbeTElevationCoef = require('./arbeTElevationCoef.js');
let arbeFusionClasses = require('./arbeFusionClasses.js');
let arbeNewPcMsg = require('./arbeNewPcMsg.js');
let arbeBuildNumber = require('./arbeBuildNumber.js');
let arbeClassificationTrainingPc = require('./arbeClassificationTrainingPc.js');
let arbeTSlamMetadata = require('./arbeTSlamMetadata.js');
let arbeObjTrackParams = require('./arbeObjTrackParams.js');
let arbeFusionQueriesVec = require('./arbeFusionQueriesVec.js');
let arbeTSlamPos = require('./arbeTSlamPos.js');
let arbeSlamMsg = require('./arbeSlamMsg.js');
let arbeClassSingleQuery = require('./arbeClassSingleQuery.js');
let arbeTSlamVelocity = require('./arbeTSlamVelocity.js');
let arbeHornData = require('./arbeHornData.js');
let arbeVerbosityParams = require('./arbeVerbosityParams.js');
let arbeFusionResponse = require('./arbeFusionResponse.js');
let arbePcFrameHeader = require('./arbePcFrameHeader.js');
let arbeClassificationMetadata = require('./arbeClassificationMetadata.js');
let arbeTDopplerCoef = require('./arbeTDopplerCoef.js');
let arbeMagCalParams = require('./arbeMagCalParams.js');
let arbeClusteringParams = require('./arbeClusteringParams.js');
let arbePcBins = require('./arbePcBins.js');
let arbeBookmarkMsg = require('./arbeBookmarkMsg.js');
let arbeFusionResponsesVec = require('./arbeFusionResponsesVec.js');
let arbeClassFrameQueries = require('./arbeClassFrameQueries.js');
let arbeSlamParamsmsg = require('./arbeSlamParamsmsg.js');
let arbeMatchParams = require('./arbeMatchParams.js');
let arbePcWithValid = require('./arbePcWithValid.js');
let arbeSlamObjectmsg = require('./arbeSlamObjectmsg.js');
let arbeCameraInstallationParams = require('./arbeCameraInstallationParams.js');
let arbePcRes = require('./arbePcRes.js');
let arbeVisParams = require('./arbeVisParams.js');
let arbeDspJobmsg = require('./arbeDspJobmsg.js');
let arbeRangeCoef = require('./arbeRangeCoef.js');
let arbeImuData = require('./arbeImuData.js');
let arbeTAzimuthCoef = require('./arbeTAzimuthCoef.js');
let arbeMultipathParams = require('./arbeMultipathParams.js');
let arbeClassSingleResponse = require('./arbeClassSingleResponse.js');
let arbeTSlamBox = require('./arbeTSlamBox.js');
let arbePCmsg = require('./arbePCmsg.js');
let arbeNewClassFrameQueries = require('./arbeNewClassFrameQueries.js');
let arbeTPcMetadata = require('./arbeTPcMetadata.js');
let arbeGUIsettings = require('./arbeGUIsettings.js');
let arbeSettingsPerRadar = require('./arbeSettingsPerRadar.js');
let arbeTSlamHeader_V1_1 = require('./arbeTSlamHeader_V1_1.js');
let arbeTSlamObj = require('./arbeTSlamObj.js');
let arbeTPointCloud_V1_1Header = require('./arbeTPointCloud_V1_1Header.js');
let arbeGpsData = require('./arbeGpsData.js');

module.exports = {
  arbeEgoVelParams: arbeEgoVelParams,
  arbeTAlgoDetectorThrOutput: arbeTAlgoDetectorThrOutput,
  arbeThresholds: arbeThresholds,
  arbeRdInclination: arbeRdInclination,
  arbeClassFrameResponses: arbeClassFrameResponses,
  arbeDtExceedTh: arbeDtExceedTh,
  arbeFrameTypes: arbeFrameTypes,
  arbeSlamDisplaySettings: arbeSlamDisplaySettings,
  arbeBoolWithTime: arbeBoolWithTime,
  arbeFusionQuery: arbeFusionQuery,
  arbeTAlgoDetectorThrInfo: arbeTAlgoDetectorThrInfo,
  arbePolarFreeSpacePoly: arbePolarFreeSpacePoly,
  arbePcMetadata: arbePcMetadata,
  arbeTElevationCoef: arbeTElevationCoef,
  arbeFusionClasses: arbeFusionClasses,
  arbeNewPcMsg: arbeNewPcMsg,
  arbeBuildNumber: arbeBuildNumber,
  arbeClassificationTrainingPc: arbeClassificationTrainingPc,
  arbeTSlamMetadata: arbeTSlamMetadata,
  arbeObjTrackParams: arbeObjTrackParams,
  arbeFusionQueriesVec: arbeFusionQueriesVec,
  arbeTSlamPos: arbeTSlamPos,
  arbeSlamMsg: arbeSlamMsg,
  arbeClassSingleQuery: arbeClassSingleQuery,
  arbeTSlamVelocity: arbeTSlamVelocity,
  arbeHornData: arbeHornData,
  arbeVerbosityParams: arbeVerbosityParams,
  arbeFusionResponse: arbeFusionResponse,
  arbePcFrameHeader: arbePcFrameHeader,
  arbeClassificationMetadata: arbeClassificationMetadata,
  arbeTDopplerCoef: arbeTDopplerCoef,
  arbeMagCalParams: arbeMagCalParams,
  arbeClusteringParams: arbeClusteringParams,
  arbePcBins: arbePcBins,
  arbeBookmarkMsg: arbeBookmarkMsg,
  arbeFusionResponsesVec: arbeFusionResponsesVec,
  arbeClassFrameQueries: arbeClassFrameQueries,
  arbeSlamParamsmsg: arbeSlamParamsmsg,
  arbeMatchParams: arbeMatchParams,
  arbePcWithValid: arbePcWithValid,
  arbeSlamObjectmsg: arbeSlamObjectmsg,
  arbeCameraInstallationParams: arbeCameraInstallationParams,
  arbePcRes: arbePcRes,
  arbeVisParams: arbeVisParams,
  arbeDspJobmsg: arbeDspJobmsg,
  arbeRangeCoef: arbeRangeCoef,
  arbeImuData: arbeImuData,
  arbeTAzimuthCoef: arbeTAzimuthCoef,
  arbeMultipathParams: arbeMultipathParams,
  arbeClassSingleResponse: arbeClassSingleResponse,
  arbeTSlamBox: arbeTSlamBox,
  arbePCmsg: arbePCmsg,
  arbeNewClassFrameQueries: arbeNewClassFrameQueries,
  arbeTPcMetadata: arbeTPcMetadata,
  arbeGUIsettings: arbeGUIsettings,
  arbeSettingsPerRadar: arbeSettingsPerRadar,
  arbeTSlamHeader_V1_1: arbeTSlamHeader_V1_1,
  arbeTSlamObj: arbeTSlamObj,
  arbeTPointCloud_V1_1Header: arbeTPointCloud_V1_1Header,
  arbeGpsData: arbeGpsData,
};
