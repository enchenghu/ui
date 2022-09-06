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

class arbeBookmarkMsg {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.frame_num = null;
      this.egoVel = null;
    }
    else {
      if (initObj.hasOwnProperty('frame_num')) {
        this.frame_num = initObj.frame_num
      }
      else {
        this.frame_num = 0;
      }
      if (initObj.hasOwnProperty('egoVel')) {
        this.egoVel = initObj.egoVel
      }
      else {
        this.egoVel = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeBookmarkMsg
    // Serialize message field [frame_num]
    bufferOffset = _serializer.uint16(obj.frame_num, buffer, bufferOffset);
    // Serialize message field [egoVel]
    bufferOffset = _serializer.float32(obj.egoVel, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeBookmarkMsg
    let len;
    let data = new arbeBookmarkMsg(null);
    // Deserialize message field [frame_num]
    data.frame_num = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [egoVel]
    data.egoVel = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 6;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeBookmarkMsg';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '968e0faad3956fde6fdb61e0a1f108a6';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint16 frame_num
    float32 egoVel
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeBookmarkMsg(null);
    if (msg.frame_num !== undefined) {
      resolved.frame_num = msg.frame_num;
    }
    else {
      resolved.frame_num = 0
    }

    if (msg.egoVel !== undefined) {
      resolved.egoVel = msg.egoVel;
    }
    else {
      resolved.egoVel = 0.0
    }

    return resolved;
    }
};

module.exports = arbeBookmarkMsg;
