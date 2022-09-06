// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeFrameTypes = require('./arbeFrameTypes.js');

//-----------------------------------------------------------

class arbePcFrameHeader {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.time = null;
      this.frame_counter = null;
      this.is_4d = null;
      this.frame_type = null;
      this.udp_lost_packages = null;
      this.number_of_points = null;
      this.crd_count = null;
      this.frame_types = null;
    }
    else {
      if (initObj.hasOwnProperty('time')) {
        this.time = initObj.time
      }
      else {
        this.time = 0;
      }
      if (initObj.hasOwnProperty('frame_counter')) {
        this.frame_counter = initObj.frame_counter
      }
      else {
        this.frame_counter = 0;
      }
      if (initObj.hasOwnProperty('is_4d')) {
        this.is_4d = initObj.is_4d
      }
      else {
        this.is_4d = 0;
      }
      if (initObj.hasOwnProperty('frame_type')) {
        this.frame_type = initObj.frame_type
      }
      else {
        this.frame_type = 0;
      }
      if (initObj.hasOwnProperty('udp_lost_packages')) {
        this.udp_lost_packages = initObj.udp_lost_packages
      }
      else {
        this.udp_lost_packages = 0;
      }
      if (initObj.hasOwnProperty('number_of_points')) {
        this.number_of_points = initObj.number_of_points
      }
      else {
        this.number_of_points = 0;
      }
      if (initObj.hasOwnProperty('crd_count')) {
        this.crd_count = initObj.crd_count
      }
      else {
        this.crd_count = 0;
      }
      if (initObj.hasOwnProperty('frame_types')) {
        this.frame_types = initObj.frame_types
      }
      else {
        this.frame_types = new arbeFrameTypes();
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbePcFrameHeader
    // Serialize message field [time]
    bufferOffset = _serializer.uint64(obj.time, buffer, bufferOffset);
    // Serialize message field [frame_counter]
    bufferOffset = _serializer.uint16(obj.frame_counter, buffer, bufferOffset);
    // Serialize message field [is_4d]
    bufferOffset = _serializer.uint8(obj.is_4d, buffer, bufferOffset);
    // Serialize message field [frame_type]
    bufferOffset = _serializer.uint8(obj.frame_type, buffer, bufferOffset);
    // Serialize message field [udp_lost_packages]
    bufferOffset = _serializer.uint16(obj.udp_lost_packages, buffer, bufferOffset);
    // Serialize message field [number_of_points]
    bufferOffset = _serializer.uint32(obj.number_of_points, buffer, bufferOffset);
    // Serialize message field [crd_count]
    bufferOffset = _serializer.uint16(obj.crd_count, buffer, bufferOffset);
    // Serialize message field [frame_types]
    bufferOffset = arbeFrameTypes.serialize(obj.frame_types, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbePcFrameHeader
    let len;
    let data = new arbePcFrameHeader(null);
    // Deserialize message field [time]
    data.time = _deserializer.uint64(buffer, bufferOffset);
    // Deserialize message field [frame_counter]
    data.frame_counter = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [is_4d]
    data.is_4d = _deserializer.uint8(buffer, bufferOffset);
    // Deserialize message field [frame_type]
    data.frame_type = _deserializer.uint8(buffer, bufferOffset);
    // Deserialize message field [udp_lost_packages]
    data.udp_lost_packages = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [number_of_points]
    data.number_of_points = _deserializer.uint32(buffer, bufferOffset);
    // Deserialize message field [crd_count]
    data.crd_count = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [frame_types]
    data.frame_types = arbeFrameTypes.deserialize(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 20;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbePcFrameHeader';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'ea7e8a42429d20d4636665fbdb9eab60';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint64 time
    uint16 frame_counter
    uint8  is_4d
    uint8  frame_type
    uint16 udp_lost_packages
    uint32 number_of_points
    uint16 crd_count
    arbeFrameTypes frame_types
    
    ================================================================================
    MSG: arbe_msgs/arbeFrameTypes
    uint8 Frame_Type_3d_short = 1
    uint8 Frame_Type_3d_mid = 2
    uint8 Frame_Type_3d_long = 3
    uint8 Frame_Type_3d_ultra_long = 13
    uint8 Frame_Type_4d_short = 4
    uint8 Frame_Type_4d_mid = 5
    uint8 Frame_Type_4d_long = 6
    uint8 Frame_Type_4d_ultra_long = 16
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbePcFrameHeader(null);
    if (msg.time !== undefined) {
      resolved.time = msg.time;
    }
    else {
      resolved.time = 0
    }

    if (msg.frame_counter !== undefined) {
      resolved.frame_counter = msg.frame_counter;
    }
    else {
      resolved.frame_counter = 0
    }

    if (msg.is_4d !== undefined) {
      resolved.is_4d = msg.is_4d;
    }
    else {
      resolved.is_4d = 0
    }

    if (msg.frame_type !== undefined) {
      resolved.frame_type = msg.frame_type;
    }
    else {
      resolved.frame_type = 0
    }

    if (msg.udp_lost_packages !== undefined) {
      resolved.udp_lost_packages = msg.udp_lost_packages;
    }
    else {
      resolved.udp_lost_packages = 0
    }

    if (msg.number_of_points !== undefined) {
      resolved.number_of_points = msg.number_of_points;
    }
    else {
      resolved.number_of_points = 0
    }

    if (msg.crd_count !== undefined) {
      resolved.crd_count = msg.crd_count;
    }
    else {
      resolved.crd_count = 0
    }

    if (msg.frame_types !== undefined) {
      resolved.frame_types = arbeFrameTypes.Resolve(msg.frame_types)
    }
    else {
      resolved.frame_types = new arbeFrameTypes()
    }

    return resolved;
    }
};

module.exports = arbePcFrameHeader;
