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

class arbeTSlamPos {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.x = null;
      this.y = null;
      this.z = null;
      this.dx = null;
      this.dy = null;
      this.dz = null;
    }
    else {
      if (initObj.hasOwnProperty('x')) {
        this.x = initObj.x
      }
      else {
        this.x = 0.0;
      }
      if (initObj.hasOwnProperty('y')) {
        this.y = initObj.y
      }
      else {
        this.y = 0.0;
      }
      if (initObj.hasOwnProperty('z')) {
        this.z = initObj.z
      }
      else {
        this.z = 0.0;
      }
      if (initObj.hasOwnProperty('dx')) {
        this.dx = initObj.dx
      }
      else {
        this.dx = 0.0;
      }
      if (initObj.hasOwnProperty('dy')) {
        this.dy = initObj.dy
      }
      else {
        this.dy = 0.0;
      }
      if (initObj.hasOwnProperty('dz')) {
        this.dz = initObj.dz
      }
      else {
        this.dz = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeTSlamPos
    // Serialize message field [x]
    bufferOffset = _serializer.float32(obj.x, buffer, bufferOffset);
    // Serialize message field [y]
    bufferOffset = _serializer.float32(obj.y, buffer, bufferOffset);
    // Serialize message field [z]
    bufferOffset = _serializer.float32(obj.z, buffer, bufferOffset);
    // Serialize message field [dx]
    bufferOffset = _serializer.float32(obj.dx, buffer, bufferOffset);
    // Serialize message field [dy]
    bufferOffset = _serializer.float32(obj.dy, buffer, bufferOffset);
    // Serialize message field [dz]
    bufferOffset = _serializer.float32(obj.dz, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeTSlamPos
    let len;
    let data = new arbeTSlamPos(null);
    // Deserialize message field [x]
    data.x = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [y]
    data.y = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [z]
    data.z = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [dx]
    data.dx = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [dy]
    data.dy = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [dz]
    data.dz = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 24;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeTSlamPos';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '7ebdf4fc16c563585927be0d949cdc5a';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    float32 x 
    float32 y 
    float32 z 
    float32  dx 
    float32  dy 
    float32  dz
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeTSlamPos(null);
    if (msg.x !== undefined) {
      resolved.x = msg.x;
    }
    else {
      resolved.x = 0.0
    }

    if (msg.y !== undefined) {
      resolved.y = msg.y;
    }
    else {
      resolved.y = 0.0
    }

    if (msg.z !== undefined) {
      resolved.z = msg.z;
    }
    else {
      resolved.z = 0.0
    }

    if (msg.dx !== undefined) {
      resolved.dx = msg.dx;
    }
    else {
      resolved.dx = 0.0
    }

    if (msg.dy !== undefined) {
      resolved.dy = msg.dy;
    }
    else {
      resolved.dy = 0.0
    }

    if (msg.dz !== undefined) {
      resolved.dz = msg.dz;
    }
    else {
      resolved.dz = 0.0
    }

    return resolved;
    }
};

module.exports = arbeTSlamPos;
