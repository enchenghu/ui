// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class arbeTAlgoDetectorThrInfo {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.unFrameNumber = null;
      this.eAlgoOutput = null;
      this.eThresholdType = null;
      this.eFrameType = null;
      this.Data = null;
    }
    else {
      if (initObj.hasOwnProperty('unFrameNumber')) {
        this.unFrameNumber = initObj.unFrameNumber
      }
      else {
        this.unFrameNumber = 0;
      }
      if (initObj.hasOwnProperty('eAlgoOutput')) {
        this.eAlgoOutput = initObj.eAlgoOutput
      }
      else {
        this.eAlgoOutput = 0;
      }
      if (initObj.hasOwnProperty('eThresholdType')) {
        this.eThresholdType = initObj.eThresholdType
      }
      else {
        this.eThresholdType = 0;
      }
      if (initObj.hasOwnProperty('eFrameType')) {
        this.eFrameType = initObj.eFrameType
      }
      else {
        this.eFrameType = 0;
      }
      if (initObj.hasOwnProperty('Data')) {
        this.Data = initObj.Data
      }
      else {
        this.Data = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeTAlgoDetectorThrInfo
    // Serialize message field [unFrameNumber]
    bufferOffset = _serializer.uint32(obj.unFrameNumber, buffer, bufferOffset);
    // Serialize message field [eAlgoOutput]
    bufferOffset = _serializer.uint8(obj.eAlgoOutput, buffer, bufferOffset);
    // Serialize message field [eThresholdType]
    bufferOffset = _serializer.uint8(obj.eThresholdType, buffer, bufferOffset);
    // Serialize message field [eFrameType]
    bufferOffset = _serializer.uint8(obj.eFrameType, buffer, bufferOffset);
    // Serialize message field [Data]
    bufferOffset = _arraySerializer.uint32(obj.Data, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeTAlgoDetectorThrInfo
    let len;
    let data = new arbeTAlgoDetectorThrInfo(null);
    // Deserialize message field [unFrameNumber]
    data.unFrameNumber = _deserializer.uint32(buffer, bufferOffset);
    // Deserialize message field [eAlgoOutput]
    data.eAlgoOutput = _deserializer.uint8(buffer, bufferOffset);
    // Deserialize message field [eThresholdType]
    data.eThresholdType = _deserializer.uint8(buffer, bufferOffset);
    // Deserialize message field [eFrameType]
    data.eFrameType = _deserializer.uint8(buffer, bufferOffset);
    // Deserialize message field [Data]
    data.Data = _arrayDeserializer.uint32(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 4 * object.Data.length;
    return length + 11;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeTAlgoDetectorThrInfo';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '5f9f71c2214552c0e7f4ddc765c4621d';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint32 unFrameNumber
    uint8 eAlgoOutput
    uint8 eThresholdType
    uint8 eFrameType
    uint32[] Data
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeTAlgoDetectorThrInfo(null);
    if (msg.unFrameNumber !== undefined) {
      resolved.unFrameNumber = msg.unFrameNumber;
    }
    else {
      resolved.unFrameNumber = 0
    }

    if (msg.eAlgoOutput !== undefined) {
      resolved.eAlgoOutput = msg.eAlgoOutput;
    }
    else {
      resolved.eAlgoOutput = 0
    }

    if (msg.eThresholdType !== undefined) {
      resolved.eThresholdType = msg.eThresholdType;
    }
    else {
      resolved.eThresholdType = 0
    }

    if (msg.eFrameType !== undefined) {
      resolved.eFrameType = msg.eFrameType;
    }
    else {
      resolved.eFrameType = 0
    }

    if (msg.Data !== undefined) {
      resolved.Data = msg.Data;
    }
    else {
      resolved.Data = []
    }

    return resolved;
    }
};

module.exports = arbeTAlgoDetectorThrInfo;
