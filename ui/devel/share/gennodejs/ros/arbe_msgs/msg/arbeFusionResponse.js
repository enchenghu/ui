// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeFusionClasses = require('./arbeFusionClasses.js');

//-----------------------------------------------------------

class arbeFusionResponse {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.valid = null;
      this.obj_id = null;
      this.query_id = null;
      this.fusionClass = null;
      this.obj_index = null;
      this.queryTime = null;
      this.conf = null;
      this.tta_count = null;
      this.tta_scores = null;
      this.class_str = null;
      this.fusionClasses = null;
      this.top = null;
      this.left = null;
      this.bottom = null;
      this.right = null;
    }
    else {
      if (initObj.hasOwnProperty('valid')) {
        this.valid = initObj.valid
      }
      else {
        this.valid = false;
      }
      if (initObj.hasOwnProperty('obj_id')) {
        this.obj_id = initObj.obj_id
      }
      else {
        this.obj_id = 0;
      }
      if (initObj.hasOwnProperty('query_id')) {
        this.query_id = initObj.query_id
      }
      else {
        this.query_id = 0;
      }
      if (initObj.hasOwnProperty('fusionClass')) {
        this.fusionClass = initObj.fusionClass
      }
      else {
        this.fusionClass = 0;
      }
      if (initObj.hasOwnProperty('obj_index')) {
        this.obj_index = initObj.obj_index
      }
      else {
        this.obj_index = 0;
      }
      if (initObj.hasOwnProperty('queryTime')) {
        this.queryTime = initObj.queryTime
      }
      else {
        this.queryTime = 0;
      }
      if (initObj.hasOwnProperty('conf')) {
        this.conf = initObj.conf
      }
      else {
        this.conf = 0.0;
      }
      if (initObj.hasOwnProperty('tta_count')) {
        this.tta_count = initObj.tta_count
      }
      else {
        this.tta_count = 0;
      }
      if (initObj.hasOwnProperty('tta_scores')) {
        this.tta_scores = initObj.tta_scores
      }
      else {
        this.tta_scores = [];
      }
      if (initObj.hasOwnProperty('class_str')) {
        this.class_str = initObj.class_str
      }
      else {
        this.class_str = '';
      }
      if (initObj.hasOwnProperty('fusionClasses')) {
        this.fusionClasses = initObj.fusionClasses
      }
      else {
        this.fusionClasses = new arbeFusionClasses();
      }
      if (initObj.hasOwnProperty('top')) {
        this.top = initObj.top
      }
      else {
        this.top = 0;
      }
      if (initObj.hasOwnProperty('left')) {
        this.left = initObj.left
      }
      else {
        this.left = 0;
      }
      if (initObj.hasOwnProperty('bottom')) {
        this.bottom = initObj.bottom
      }
      else {
        this.bottom = 0;
      }
      if (initObj.hasOwnProperty('right')) {
        this.right = initObj.right
      }
      else {
        this.right = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeFusionResponse
    // Serialize message field [valid]
    bufferOffset = _serializer.bool(obj.valid, buffer, bufferOffset);
    // Serialize message field [obj_id]
    bufferOffset = _serializer.uint16(obj.obj_id, buffer, bufferOffset);
    // Serialize message field [query_id]
    bufferOffset = _serializer.uint32(obj.query_id, buffer, bufferOffset);
    // Serialize message field [fusionClass]
    bufferOffset = _serializer.uint8(obj.fusionClass, buffer, bufferOffset);
    // Serialize message field [obj_index]
    bufferOffset = _serializer.uint16(obj.obj_index, buffer, bufferOffset);
    // Serialize message field [queryTime]
    bufferOffset = _serializer.uint64(obj.queryTime, buffer, bufferOffset);
    // Serialize message field [conf]
    bufferOffset = _serializer.float32(obj.conf, buffer, bufferOffset);
    // Serialize message field [tta_count]
    bufferOffset = _serializer.uint8(obj.tta_count, buffer, bufferOffset);
    // Serialize message field [tta_scores]
    bufferOffset = _arraySerializer.float32(obj.tta_scores, buffer, bufferOffset, null);
    // Serialize message field [class_str]
    bufferOffset = _serializer.string(obj.class_str, buffer, bufferOffset);
    // Serialize message field [fusionClasses]
    bufferOffset = arbeFusionClasses.serialize(obj.fusionClasses, buffer, bufferOffset);
    // Serialize message field [top]
    bufferOffset = _serializer.uint16(obj.top, buffer, bufferOffset);
    // Serialize message field [left]
    bufferOffset = _serializer.uint16(obj.left, buffer, bufferOffset);
    // Serialize message field [bottom]
    bufferOffset = _serializer.uint16(obj.bottom, buffer, bufferOffset);
    // Serialize message field [right]
    bufferOffset = _serializer.uint16(obj.right, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeFusionResponse
    let len;
    let data = new arbeFusionResponse(null);
    // Deserialize message field [valid]
    data.valid = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [obj_id]
    data.obj_id = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [query_id]
    data.query_id = _deserializer.uint32(buffer, bufferOffset);
    // Deserialize message field [fusionClass]
    data.fusionClass = _deserializer.uint8(buffer, bufferOffset);
    // Deserialize message field [obj_index]
    data.obj_index = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [queryTime]
    data.queryTime = _deserializer.uint64(buffer, bufferOffset);
    // Deserialize message field [conf]
    data.conf = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [tta_count]
    data.tta_count = _deserializer.uint8(buffer, bufferOffset);
    // Deserialize message field [tta_scores]
    data.tta_scores = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [class_str]
    data.class_str = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [fusionClasses]
    data.fusionClasses = arbeFusionClasses.deserialize(buffer, bufferOffset);
    // Deserialize message field [top]
    data.top = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [left]
    data.left = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [bottom]
    data.bottom = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [right]
    data.right = _deserializer.uint16(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 4 * object.tta_scores.length;
    length += object.class_str.length;
    return length + 39;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeFusionResponse';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '9d16e7674f130b5c070b9bc414f29ed5';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    bool valid
    uint16 obj_id
    uint32 query_id
    uint8 fusionClass
    uint16 obj_index
    uint64 queryTime
    float32 conf
    uint8 tta_count
    float32[] tta_scores
    string class_str
    arbeFusionClasses fusionClasses
    
    uint16  top
    uint16  left
    uint16  bottom
    uint16  right
    
    ================================================================================
    MSG: arbe_msgs/arbeFusionClasses
    uint16 FC_BIKE = 0
    uint16 FC_TRUCK_BUS = 1
    uint16 FC_VRU = 2
    uint16 FC_CAR = 3
    uint16 FC_NOT_CLASSIFIED = 4
    uint16 NUM_CLASSES = 4
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeFusionResponse(null);
    if (msg.valid !== undefined) {
      resolved.valid = msg.valid;
    }
    else {
      resolved.valid = false
    }

    if (msg.obj_id !== undefined) {
      resolved.obj_id = msg.obj_id;
    }
    else {
      resolved.obj_id = 0
    }

    if (msg.query_id !== undefined) {
      resolved.query_id = msg.query_id;
    }
    else {
      resolved.query_id = 0
    }

    if (msg.fusionClass !== undefined) {
      resolved.fusionClass = msg.fusionClass;
    }
    else {
      resolved.fusionClass = 0
    }

    if (msg.obj_index !== undefined) {
      resolved.obj_index = msg.obj_index;
    }
    else {
      resolved.obj_index = 0
    }

    if (msg.queryTime !== undefined) {
      resolved.queryTime = msg.queryTime;
    }
    else {
      resolved.queryTime = 0
    }

    if (msg.conf !== undefined) {
      resolved.conf = msg.conf;
    }
    else {
      resolved.conf = 0.0
    }

    if (msg.tta_count !== undefined) {
      resolved.tta_count = msg.tta_count;
    }
    else {
      resolved.tta_count = 0
    }

    if (msg.tta_scores !== undefined) {
      resolved.tta_scores = msg.tta_scores;
    }
    else {
      resolved.tta_scores = []
    }

    if (msg.class_str !== undefined) {
      resolved.class_str = msg.class_str;
    }
    else {
      resolved.class_str = ''
    }

    if (msg.fusionClasses !== undefined) {
      resolved.fusionClasses = arbeFusionClasses.Resolve(msg.fusionClasses)
    }
    else {
      resolved.fusionClasses = new arbeFusionClasses()
    }

    if (msg.top !== undefined) {
      resolved.top = msg.top;
    }
    else {
      resolved.top = 0
    }

    if (msg.left !== undefined) {
      resolved.left = msg.left;
    }
    else {
      resolved.left = 0
    }

    if (msg.bottom !== undefined) {
      resolved.bottom = msg.bottom;
    }
    else {
      resolved.bottom = 0
    }

    if (msg.right !== undefined) {
      resolved.right = msg.right;
    }
    else {
      resolved.right = 0
    }

    return resolved;
    }
};

module.exports = arbeFusionResponse;
