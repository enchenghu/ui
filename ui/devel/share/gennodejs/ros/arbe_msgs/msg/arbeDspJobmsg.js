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

class arbeDspJobmsg {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.Header = null;
      this.Info = null;
    }
    else {
      if (initObj.hasOwnProperty('Header')) {
        this.Header = initObj.Header
      }
      else {
        this.Header = [];
      }
      if (initObj.hasOwnProperty('Info')) {
        this.Info = initObj.Info
      }
      else {
        this.Info = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeDspJobmsg
    // Serialize message field [Header]
    bufferOffset = _arraySerializer.uint8(obj.Header, buffer, bufferOffset, null);
    // Serialize message field [Info]
    bufferOffset = _arraySerializer.uint8(obj.Info, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeDspJobmsg
    let len;
    let data = new arbeDspJobmsg(null);
    // Deserialize message field [Header]
    data.Header = _arrayDeserializer.uint8(buffer, bufferOffset, null)
    // Deserialize message field [Info]
    data.Info = _arrayDeserializer.uint8(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += object.Header.length;
    length += object.Info.length;
    return length + 8;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeDspJobmsg';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '8d95b90f43b0c5ea15e47b0302160dcc';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint8[] Header
    uint8[] Info
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeDspJobmsg(null);
    if (msg.Header !== undefined) {
      resolved.Header = msg.Header;
    }
    else {
      resolved.Header = []
    }

    if (msg.Info !== undefined) {
      resolved.Info = msg.Info;
    }
    else {
      resolved.Info = []
    }

    return resolved;
    }
};

module.exports = arbeDspJobmsg;
