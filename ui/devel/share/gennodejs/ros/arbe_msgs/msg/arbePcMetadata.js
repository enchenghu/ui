// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbePcRes = require('./arbePcRes.js');

//-----------------------------------------------------------

class arbePcMetadata {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.PcResolution = null;
      this.range_offset = null;
      this.range_res_in_db = null;
      this.azimuth_fft_size = null;
      this.elevation_fft_size = null;
      this.azimuth_fft_padding = null;
      this.elevation_fft_padding = null;
    }
    else {
      if (initObj.hasOwnProperty('PcResolution')) {
        this.PcResolution = initObj.PcResolution
      }
      else {
        this.PcResolution = new arbePcRes();
      }
      if (initObj.hasOwnProperty('range_offset')) {
        this.range_offset = initObj.range_offset
      }
      else {
        this.range_offset = 0.0;
      }
      if (initObj.hasOwnProperty('range_res_in_db')) {
        this.range_res_in_db = initObj.range_res_in_db
      }
      else {
        this.range_res_in_db = 0.0;
      }
      if (initObj.hasOwnProperty('azimuth_fft_size')) {
        this.azimuth_fft_size = initObj.azimuth_fft_size
      }
      else {
        this.azimuth_fft_size = 0;
      }
      if (initObj.hasOwnProperty('elevation_fft_size')) {
        this.elevation_fft_size = initObj.elevation_fft_size
      }
      else {
        this.elevation_fft_size = 0;
      }
      if (initObj.hasOwnProperty('azimuth_fft_padding')) {
        this.azimuth_fft_padding = initObj.azimuth_fft_padding
      }
      else {
        this.azimuth_fft_padding = 0;
      }
      if (initObj.hasOwnProperty('elevation_fft_padding')) {
        this.elevation_fft_padding = initObj.elevation_fft_padding
      }
      else {
        this.elevation_fft_padding = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbePcMetadata
    // Serialize message field [PcResolution]
    bufferOffset = arbePcRes.serialize(obj.PcResolution, buffer, bufferOffset);
    // Serialize message field [range_offset]
    bufferOffset = _serializer.float32(obj.range_offset, buffer, bufferOffset);
    // Serialize message field [range_res_in_db]
    bufferOffset = _serializer.float32(obj.range_res_in_db, buffer, bufferOffset);
    // Serialize message field [azimuth_fft_size]
    bufferOffset = _serializer.uint32(obj.azimuth_fft_size, buffer, bufferOffset);
    // Serialize message field [elevation_fft_size]
    bufferOffset = _serializer.uint32(obj.elevation_fft_size, buffer, bufferOffset);
    // Serialize message field [azimuth_fft_padding]
    bufferOffset = _serializer.uint32(obj.azimuth_fft_padding, buffer, bufferOffset);
    // Serialize message field [elevation_fft_padding]
    bufferOffset = _serializer.uint32(obj.elevation_fft_padding, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbePcMetadata
    let len;
    let data = new arbePcMetadata(null);
    // Deserialize message field [PcResolution]
    data.PcResolution = arbePcRes.deserialize(buffer, bufferOffset);
    // Deserialize message field [range_offset]
    data.range_offset = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [range_res_in_db]
    data.range_res_in_db = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [azimuth_fft_size]
    data.azimuth_fft_size = _deserializer.uint32(buffer, bufferOffset);
    // Deserialize message field [elevation_fft_size]
    data.elevation_fft_size = _deserializer.uint32(buffer, bufferOffset);
    // Deserialize message field [azimuth_fft_padding]
    data.azimuth_fft_padding = _deserializer.uint32(buffer, bufferOffset);
    // Deserialize message field [elevation_fft_padding]
    data.elevation_fft_padding = _deserializer.uint32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 40;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbePcMetadata';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '07014ddb83398e57a7856835fad2cfca';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    arbePcRes PcResolution
    float32 range_offset
    float32 range_res_in_db
    uint32 azimuth_fft_size
    uint32 elevation_fft_size
    uint32 azimuth_fft_padding
    uint32 elevation_fft_padding
    
    
    ================================================================================
    MSG: arbe_msgs/arbePcRes
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
    const resolved = new arbePcMetadata(null);
    if (msg.PcResolution !== undefined) {
      resolved.PcResolution = arbePcRes.Resolve(msg.PcResolution)
    }
    else {
      resolved.PcResolution = new arbePcRes()
    }

    if (msg.range_offset !== undefined) {
      resolved.range_offset = msg.range_offset;
    }
    else {
      resolved.range_offset = 0.0
    }

    if (msg.range_res_in_db !== undefined) {
      resolved.range_res_in_db = msg.range_res_in_db;
    }
    else {
      resolved.range_res_in_db = 0.0
    }

    if (msg.azimuth_fft_size !== undefined) {
      resolved.azimuth_fft_size = msg.azimuth_fft_size;
    }
    else {
      resolved.azimuth_fft_size = 0
    }

    if (msg.elevation_fft_size !== undefined) {
      resolved.elevation_fft_size = msg.elevation_fft_size;
    }
    else {
      resolved.elevation_fft_size = 0
    }

    if (msg.azimuth_fft_padding !== undefined) {
      resolved.azimuth_fft_padding = msg.azimuth_fft_padding;
    }
    else {
      resolved.azimuth_fft_padding = 0
    }

    if (msg.elevation_fft_padding !== undefined) {
      resolved.elevation_fft_padding = msg.elevation_fft_padding;
    }
    else {
      resolved.elevation_fft_padding = 0
    }

    return resolved;
    }
};

module.exports = arbePcMetadata;
