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

class arbePcBins {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.range_bin = null;
      this.doppler_signed_bin = null;
      this.azimuth_signed_bin = null;
      this.elevation_signed_bin = null;
      this.phase = null;
      this.power = null;
    }
    else {
      if (initObj.hasOwnProperty('range_bin')) {
        this.range_bin = initObj.range_bin
      }
      else {
        this.range_bin = [];
      }
      if (initObj.hasOwnProperty('doppler_signed_bin')) {
        this.doppler_signed_bin = initObj.doppler_signed_bin
      }
      else {
        this.doppler_signed_bin = [];
      }
      if (initObj.hasOwnProperty('azimuth_signed_bin')) {
        this.azimuth_signed_bin = initObj.azimuth_signed_bin
      }
      else {
        this.azimuth_signed_bin = [];
      }
      if (initObj.hasOwnProperty('elevation_signed_bin')) {
        this.elevation_signed_bin = initObj.elevation_signed_bin
      }
      else {
        this.elevation_signed_bin = [];
      }
      if (initObj.hasOwnProperty('phase')) {
        this.phase = initObj.phase
      }
      else {
        this.phase = [];
      }
      if (initObj.hasOwnProperty('power')) {
        this.power = initObj.power
      }
      else {
        this.power = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbePcBins
    // Serialize message field [range_bin]
    bufferOffset = _arraySerializer.uint16(obj.range_bin, buffer, bufferOffset, null);
    // Serialize message field [doppler_signed_bin]
    bufferOffset = _arraySerializer.int16(obj.doppler_signed_bin, buffer, bufferOffset, null);
    // Serialize message field [azimuth_signed_bin]
    bufferOffset = _arraySerializer.int8(obj.azimuth_signed_bin, buffer, bufferOffset, null);
    // Serialize message field [elevation_signed_bin]
    bufferOffset = _arraySerializer.int8(obj.elevation_signed_bin, buffer, bufferOffset, null);
    // Serialize message field [phase]
    bufferOffset = _arraySerializer.int16(obj.phase, buffer, bufferOffset, null);
    // Serialize message field [power]
    bufferOffset = _arraySerializer.int16(obj.power, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbePcBins
    let len;
    let data = new arbePcBins(null);
    // Deserialize message field [range_bin]
    data.range_bin = _arrayDeserializer.uint16(buffer, bufferOffset, null)
    // Deserialize message field [doppler_signed_bin]
    data.doppler_signed_bin = _arrayDeserializer.int16(buffer, bufferOffset, null)
    // Deserialize message field [azimuth_signed_bin]
    data.azimuth_signed_bin = _arrayDeserializer.int8(buffer, bufferOffset, null)
    // Deserialize message field [elevation_signed_bin]
    data.elevation_signed_bin = _arrayDeserializer.int8(buffer, bufferOffset, null)
    // Deserialize message field [phase]
    data.phase = _arrayDeserializer.int16(buffer, bufferOffset, null)
    // Deserialize message field [power]
    data.power = _arrayDeserializer.int16(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 2 * object.range_bin.length;
    length += 2 * object.doppler_signed_bin.length;
    length += object.azimuth_signed_bin.length;
    length += object.elevation_signed_bin.length;
    length += 2 * object.phase.length;
    length += 2 * object.power.length;
    return length + 24;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbePcBins';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '1053c45295755db3ee1eadc1a2005cf0';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint16[] range_bin
    int16[] doppler_signed_bin
    int8[] azimuth_signed_bin
    int8[] elevation_signed_bin
    int16[] phase
    int16[] power
    
    
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbePcBins(null);
    if (msg.range_bin !== undefined) {
      resolved.range_bin = msg.range_bin;
    }
    else {
      resolved.range_bin = []
    }

    if (msg.doppler_signed_bin !== undefined) {
      resolved.doppler_signed_bin = msg.doppler_signed_bin;
    }
    else {
      resolved.doppler_signed_bin = []
    }

    if (msg.azimuth_signed_bin !== undefined) {
      resolved.azimuth_signed_bin = msg.azimuth_signed_bin;
    }
    else {
      resolved.azimuth_signed_bin = []
    }

    if (msg.elevation_signed_bin !== undefined) {
      resolved.elevation_signed_bin = msg.elevation_signed_bin;
    }
    else {
      resolved.elevation_signed_bin = []
    }

    if (msg.phase !== undefined) {
      resolved.phase = msg.phase;
    }
    else {
      resolved.phase = []
    }

    if (msg.power !== undefined) {
      resolved.power = msg.power;
    }
    else {
      resolved.power = []
    }

    return resolved;
    }
};

module.exports = arbePcBins;
