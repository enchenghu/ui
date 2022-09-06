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

class arbePcRes {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.range_resolution = null;
      this.doppler_resolution = null;
      this.azimuth_coefficient = null;
      this.elevation_coefficient = null;
    }
    else {
      if (initObj.hasOwnProperty('range_resolution')) {
        this.range_resolution = initObj.range_resolution
      }
      else {
        this.range_resolution = 0.0;
      }
      if (initObj.hasOwnProperty('doppler_resolution')) {
        this.doppler_resolution = initObj.doppler_resolution
      }
      else {
        this.doppler_resolution = 0.0;
      }
      if (initObj.hasOwnProperty('azimuth_coefficient')) {
        this.azimuth_coefficient = initObj.azimuth_coefficient
      }
      else {
        this.azimuth_coefficient = 0.0;
      }
      if (initObj.hasOwnProperty('elevation_coefficient')) {
        this.elevation_coefficient = initObj.elevation_coefficient
      }
      else {
        this.elevation_coefficient = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbePcRes
    // Serialize message field [range_resolution]
    bufferOffset = _serializer.float32(obj.range_resolution, buffer, bufferOffset);
    // Serialize message field [doppler_resolution]
    bufferOffset = _serializer.float32(obj.doppler_resolution, buffer, bufferOffset);
    // Serialize message field [azimuth_coefficient]
    bufferOffset = _serializer.float32(obj.azimuth_coefficient, buffer, bufferOffset);
    // Serialize message field [elevation_coefficient]
    bufferOffset = _serializer.float32(obj.elevation_coefficient, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbePcRes
    let len;
    let data = new arbePcRes(null);
    // Deserialize message field [range_resolution]
    data.range_resolution = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [doppler_resolution]
    data.doppler_resolution = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [azimuth_coefficient]
    data.azimuth_coefficient = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [elevation_coefficient]
    data.elevation_coefficient = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 16;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbePcRes';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '84103936b409bd62d8ea8aada9826bd5';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    float32 range_resolution
    float32 doppler_resolution
    float32 azimuth_coefficient
    float32 elevation_coefficient
    
    
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbePcRes(null);
    if (msg.range_resolution !== undefined) {
      resolved.range_resolution = msg.range_resolution;
    }
    else {
      resolved.range_resolution = 0.0
    }

    if (msg.doppler_resolution !== undefined) {
      resolved.doppler_resolution = msg.doppler_resolution;
    }
    else {
      resolved.doppler_resolution = 0.0
    }

    if (msg.azimuth_coefficient !== undefined) {
      resolved.azimuth_coefficient = msg.azimuth_coefficient;
    }
    else {
      resolved.azimuth_coefficient = 0.0
    }

    if (msg.elevation_coefficient !== undefined) {
      resolved.elevation_coefficient = msg.elevation_coefficient;
    }
    else {
      resolved.elevation_coefficient = 0.0
    }

    return resolved;
    }
};

module.exports = arbePcRes;
