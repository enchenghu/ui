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

class arbeTSlamBox {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.scale_x = null;
      this.scale_y = null;
      this.scale_z = null;
      this.scale_x_unc = null;
      this.scale_y_unc = null;
      this.scale_z_unc = null;
      this.orientation_unc = null;
      this.orientation = null;
    }
    else {
      if (initObj.hasOwnProperty('scale_x')) {
        this.scale_x = initObj.scale_x
      }
      else {
        this.scale_x = 0.0;
      }
      if (initObj.hasOwnProperty('scale_y')) {
        this.scale_y = initObj.scale_y
      }
      else {
        this.scale_y = 0.0;
      }
      if (initObj.hasOwnProperty('scale_z')) {
        this.scale_z = initObj.scale_z
      }
      else {
        this.scale_z = 0.0;
      }
      if (initObj.hasOwnProperty('scale_x_unc')) {
        this.scale_x_unc = initObj.scale_x_unc
      }
      else {
        this.scale_x_unc = 0.0;
      }
      if (initObj.hasOwnProperty('scale_y_unc')) {
        this.scale_y_unc = initObj.scale_y_unc
      }
      else {
        this.scale_y_unc = 0.0;
      }
      if (initObj.hasOwnProperty('scale_z_unc')) {
        this.scale_z_unc = initObj.scale_z_unc
      }
      else {
        this.scale_z_unc = 0.0;
      }
      if (initObj.hasOwnProperty('orientation_unc')) {
        this.orientation_unc = initObj.orientation_unc
      }
      else {
        this.orientation_unc = 0.0;
      }
      if (initObj.hasOwnProperty('orientation')) {
        this.orientation = initObj.orientation
      }
      else {
        this.orientation = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeTSlamBox
    // Serialize message field [scale_x]
    bufferOffset = _serializer.float32(obj.scale_x, buffer, bufferOffset);
    // Serialize message field [scale_y]
    bufferOffset = _serializer.float32(obj.scale_y, buffer, bufferOffset);
    // Serialize message field [scale_z]
    bufferOffset = _serializer.float32(obj.scale_z, buffer, bufferOffset);
    // Serialize message field [scale_x_unc]
    bufferOffset = _serializer.float32(obj.scale_x_unc, buffer, bufferOffset);
    // Serialize message field [scale_y_unc]
    bufferOffset = _serializer.float32(obj.scale_y_unc, buffer, bufferOffset);
    // Serialize message field [scale_z_unc]
    bufferOffset = _serializer.float32(obj.scale_z_unc, buffer, bufferOffset);
    // Serialize message field [orientation_unc]
    bufferOffset = _serializer.float32(obj.orientation_unc, buffer, bufferOffset);
    // Serialize message field [orientation]
    bufferOffset = _serializer.float32(obj.orientation, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeTSlamBox
    let len;
    let data = new arbeTSlamBox(null);
    // Deserialize message field [scale_x]
    data.scale_x = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [scale_y]
    data.scale_y = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [scale_z]
    data.scale_z = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [scale_x_unc]
    data.scale_x_unc = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [scale_y_unc]
    data.scale_y_unc = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [scale_z_unc]
    data.scale_z_unc = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [orientation_unc]
    data.orientation_unc = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [orientation]
    data.orientation = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 32;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeTSlamBox';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'e36bb424db9b07f679bc8512896c93f7';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    float32 scale_x     
    float32 scale_y     
    float32 scale_z     
    float32 scale_x_unc 
    float32 scale_y_unc 
    float32 scale_z_unc 
    float32 orientation_unc  
    float32 orientation  
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeTSlamBox(null);
    if (msg.scale_x !== undefined) {
      resolved.scale_x = msg.scale_x;
    }
    else {
      resolved.scale_x = 0.0
    }

    if (msg.scale_y !== undefined) {
      resolved.scale_y = msg.scale_y;
    }
    else {
      resolved.scale_y = 0.0
    }

    if (msg.scale_z !== undefined) {
      resolved.scale_z = msg.scale_z;
    }
    else {
      resolved.scale_z = 0.0
    }

    if (msg.scale_x_unc !== undefined) {
      resolved.scale_x_unc = msg.scale_x_unc;
    }
    else {
      resolved.scale_x_unc = 0.0
    }

    if (msg.scale_y_unc !== undefined) {
      resolved.scale_y_unc = msg.scale_y_unc;
    }
    else {
      resolved.scale_y_unc = 0.0
    }

    if (msg.scale_z_unc !== undefined) {
      resolved.scale_z_unc = msg.scale_z_unc;
    }
    else {
      resolved.scale_z_unc = 0.0
    }

    if (msg.orientation_unc !== undefined) {
      resolved.orientation_unc = msg.orientation_unc;
    }
    else {
      resolved.orientation_unc = 0.0
    }

    if (msg.orientation !== undefined) {
      resolved.orientation = msg.orientation;
    }
    else {
      resolved.orientation = 0.0
    }

    return resolved;
    }
};

module.exports = arbeTSlamBox;
