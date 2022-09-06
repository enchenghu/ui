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

class arbeDtExceedTh {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.frame_counter = null;
      this.dt = null;
    }
    else {
      if (initObj.hasOwnProperty('frame_counter')) {
        this.frame_counter = initObj.frame_counter
      }
      else {
        this.frame_counter = 0;
      }
      if (initObj.hasOwnProperty('dt')) {
        this.dt = initObj.dt
      }
      else {
        this.dt = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeDtExceedTh
    // Serialize message field [frame_counter]
    bufferOffset = _serializer.uint16(obj.frame_counter, buffer, bufferOffset);
    // Serialize message field [dt]
    bufferOffset = _serializer.uint64(obj.dt, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeDtExceedTh
    let len;
    let data = new arbeDtExceedTh(null);
    // Deserialize message field [frame_counter]
    data.frame_counter = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [dt]
    data.dt = _deserializer.uint64(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 10;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeDtExceedTh';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'b3c2831d3d6ab52b0524ebb559a84899';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint16 frame_counter
    uint64 dt
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeDtExceedTh(null);
    if (msg.frame_counter !== undefined) {
      resolved.frame_counter = msg.frame_counter;
    }
    else {
      resolved.frame_counter = 0
    }

    if (msg.dt !== undefined) {
      resolved.dt = msg.dt;
    }
    else {
      resolved.dt = 0
    }

    return resolved;
    }
};

module.exports = arbeDtExceedTh;
