// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeTSlamPos = require('./arbeTSlamPos.js');
let arbeTSlamVelocity = require('./arbeTSlamVelocity.js');
let arbeTSlamBox = require('./arbeTSlamBox.js');

//-----------------------------------------------------------

class arbeTSlamObj {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.ID = null;
      this.obj_class = null;
      this.class_conf = null;
      this.position = null;
      this.velocity = null;
      this.bounding_box = null;
      this.age = null;
      this.last_frame_update = null;
    }
    else {
      if (initObj.hasOwnProperty('ID')) {
        this.ID = initObj.ID
      }
      else {
        this.ID = 0;
      }
      if (initObj.hasOwnProperty('obj_class')) {
        this.obj_class = initObj.obj_class
      }
      else {
        this.obj_class = 0;
      }
      if (initObj.hasOwnProperty('class_conf')) {
        this.class_conf = initObj.class_conf
      }
      else {
        this.class_conf = 0.0;
      }
      if (initObj.hasOwnProperty('position')) {
        this.position = initObj.position
      }
      else {
        this.position = new arbeTSlamPos();
      }
      if (initObj.hasOwnProperty('velocity')) {
        this.velocity = initObj.velocity
      }
      else {
        this.velocity = new arbeTSlamVelocity();
      }
      if (initObj.hasOwnProperty('bounding_box')) {
        this.bounding_box = initObj.bounding_box
      }
      else {
        this.bounding_box = new arbeTSlamBox();
      }
      if (initObj.hasOwnProperty('age')) {
        this.age = initObj.age
      }
      else {
        this.age = 0;
      }
      if (initObj.hasOwnProperty('last_frame_update')) {
        this.last_frame_update = initObj.last_frame_update
      }
      else {
        this.last_frame_update = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeTSlamObj
    // Serialize message field [ID]
    bufferOffset = _serializer.uint16(obj.ID, buffer, bufferOffset);
    // Serialize message field [obj_class]
    bufferOffset = _serializer.uint16(obj.obj_class, buffer, bufferOffset);
    // Serialize message field [class_conf]
    bufferOffset = _serializer.float32(obj.class_conf, buffer, bufferOffset);
    // Serialize message field [position]
    bufferOffset = arbeTSlamPos.serialize(obj.position, buffer, bufferOffset);
    // Serialize message field [velocity]
    bufferOffset = arbeTSlamVelocity.serialize(obj.velocity, buffer, bufferOffset);
    // Serialize message field [bounding_box]
    bufferOffset = arbeTSlamBox.serialize(obj.bounding_box, buffer, bufferOffset);
    // Serialize message field [age]
    bufferOffset = _serializer.uint32(obj.age, buffer, bufferOffset);
    // Serialize message field [last_frame_update]
    bufferOffset = _serializer.uint16(obj.last_frame_update, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeTSlamObj
    let len;
    let data = new arbeTSlamObj(null);
    // Deserialize message field [ID]
    data.ID = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [obj_class]
    data.obj_class = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [class_conf]
    data.class_conf = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [position]
    data.position = arbeTSlamPos.deserialize(buffer, bufferOffset);
    // Deserialize message field [velocity]
    data.velocity = arbeTSlamVelocity.deserialize(buffer, bufferOffset);
    // Deserialize message field [bounding_box]
    data.bounding_box = arbeTSlamBox.deserialize(buffer, bufferOffset);
    // Deserialize message field [age]
    data.age = _deserializer.uint32(buffer, bufferOffset);
    // Deserialize message field [last_frame_update]
    data.last_frame_update = _deserializer.uint16(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 86;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeTSlamObj';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '1eb101416f9437d15058033f4585fe65';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint16              ID
    uint16              obj_class
    float32             class_conf
    arbeTSlamPos        position
    arbeTSlamVelocity   velocity 
    arbeTSlamBox        bounding_box
    uint32              age
    uint16              last_frame_update
    
    
    
    ================================================================================
    MSG: arbe_msgs/arbeTSlamPos
    float32 x 
    float32 y 
    float32 z 
    float32  dx 
    float32  dy 
    float32  dz
    
    ================================================================================
    MSG: arbe_msgs/arbeTSlamVelocity
    float32     x_dot 
    float32     y_dot 
    float32      dx_dot 
    float32      dy_dot 
    
    ================================================================================
    MSG: arbe_msgs/arbeTSlamBox
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
    const resolved = new arbeTSlamObj(null);
    if (msg.ID !== undefined) {
      resolved.ID = msg.ID;
    }
    else {
      resolved.ID = 0
    }

    if (msg.obj_class !== undefined) {
      resolved.obj_class = msg.obj_class;
    }
    else {
      resolved.obj_class = 0
    }

    if (msg.class_conf !== undefined) {
      resolved.class_conf = msg.class_conf;
    }
    else {
      resolved.class_conf = 0.0
    }

    if (msg.position !== undefined) {
      resolved.position = arbeTSlamPos.Resolve(msg.position)
    }
    else {
      resolved.position = new arbeTSlamPos()
    }

    if (msg.velocity !== undefined) {
      resolved.velocity = arbeTSlamVelocity.Resolve(msg.velocity)
    }
    else {
      resolved.velocity = new arbeTSlamVelocity()
    }

    if (msg.bounding_box !== undefined) {
      resolved.bounding_box = arbeTSlamBox.Resolve(msg.bounding_box)
    }
    else {
      resolved.bounding_box = new arbeTSlamBox()
    }

    if (msg.age !== undefined) {
      resolved.age = msg.age;
    }
    else {
      resolved.age = 0
    }

    if (msg.last_frame_update !== undefined) {
      resolved.last_frame_update = msg.last_frame_update;
    }
    else {
      resolved.last_frame_update = 0
    }

    return resolved;
    }
};

module.exports = arbeTSlamObj;
