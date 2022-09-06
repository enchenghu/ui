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

class arbeTSlamVelocity {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.x_dot = null;
      this.y_dot = null;
      this.dx_dot = null;
      this.dy_dot = null;
    }
    else {
      if (initObj.hasOwnProperty('x_dot')) {
        this.x_dot = initObj.x_dot
      }
      else {
        this.x_dot = 0.0;
      }
      if (initObj.hasOwnProperty('y_dot')) {
        this.y_dot = initObj.y_dot
      }
      else {
        this.y_dot = 0.0;
      }
      if (initObj.hasOwnProperty('dx_dot')) {
        this.dx_dot = initObj.dx_dot
      }
      else {
        this.dx_dot = 0.0;
      }
      if (initObj.hasOwnProperty('dy_dot')) {
        this.dy_dot = initObj.dy_dot
      }
      else {
        this.dy_dot = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeTSlamVelocity
    // Serialize message field [x_dot]
    bufferOffset = _serializer.float32(obj.x_dot, buffer, bufferOffset);
    // Serialize message field [y_dot]
    bufferOffset = _serializer.float32(obj.y_dot, buffer, bufferOffset);
    // Serialize message field [dx_dot]
    bufferOffset = _serializer.float32(obj.dx_dot, buffer, bufferOffset);
    // Serialize message field [dy_dot]
    bufferOffset = _serializer.float32(obj.dy_dot, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeTSlamVelocity
    let len;
    let data = new arbeTSlamVelocity(null);
    // Deserialize message field [x_dot]
    data.x_dot = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [y_dot]
    data.y_dot = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [dx_dot]
    data.dx_dot = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [dy_dot]
    data.dy_dot = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 16;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeTSlamVelocity';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '01747869987f0c9150027780a275e95f';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    float32     x_dot 
    float32     y_dot 
    float32      dx_dot 
    float32      dy_dot 
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeTSlamVelocity(null);
    if (msg.x_dot !== undefined) {
      resolved.x_dot = msg.x_dot;
    }
    else {
      resolved.x_dot = 0.0
    }

    if (msg.y_dot !== undefined) {
      resolved.y_dot = msg.y_dot;
    }
    else {
      resolved.y_dot = 0.0
    }

    if (msg.dx_dot !== undefined) {
      resolved.dx_dot = msg.dx_dot;
    }
    else {
      resolved.dx_dot = 0.0
    }

    if (msg.dy_dot !== undefined) {
      resolved.dy_dot = msg.dy_dot;
    }
    else {
      resolved.dy_dot = 0.0
    }

    return resolved;
    }
};

module.exports = arbeTSlamVelocity;
