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

class arbeFusionQuery {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.obj_id = null;
      this.top = null;
      this.left = null;
      this.bottom = null;
      this.right = null;
      this.query_id = null;
      this.query_time = null;
      this.obj_index = null;
      this.tta_count = null;
      this.tta_scores = null;
    }
    else {
      if (initObj.hasOwnProperty('obj_id')) {
        this.obj_id = initObj.obj_id
      }
      else {
        this.obj_id = 0;
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
      if (initObj.hasOwnProperty('query_id')) {
        this.query_id = initObj.query_id
      }
      else {
        this.query_id = 0;
      }
      if (initObj.hasOwnProperty('query_time')) {
        this.query_time = initObj.query_time
      }
      else {
        this.query_time = 0;
      }
      if (initObj.hasOwnProperty('obj_index')) {
        this.obj_index = initObj.obj_index
      }
      else {
        this.obj_index = 0;
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
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeFusionQuery
    // Serialize message field [obj_id]
    bufferOffset = _serializer.uint16(obj.obj_id, buffer, bufferOffset);
    // Serialize message field [top]
    bufferOffset = _serializer.uint16(obj.top, buffer, bufferOffset);
    // Serialize message field [left]
    bufferOffset = _serializer.uint16(obj.left, buffer, bufferOffset);
    // Serialize message field [bottom]
    bufferOffset = _serializer.uint16(obj.bottom, buffer, bufferOffset);
    // Serialize message field [right]
    bufferOffset = _serializer.uint16(obj.right, buffer, bufferOffset);
    // Serialize message field [query_id]
    bufferOffset = _serializer.uint32(obj.query_id, buffer, bufferOffset);
    // Serialize message field [query_time]
    bufferOffset = _serializer.uint64(obj.query_time, buffer, bufferOffset);
    // Serialize message field [obj_index]
    bufferOffset = _serializer.uint16(obj.obj_index, buffer, bufferOffset);
    // Serialize message field [tta_count]
    bufferOffset = _serializer.uint8(obj.tta_count, buffer, bufferOffset);
    // Serialize message field [tta_scores]
    bufferOffset = _arraySerializer.float32(obj.tta_scores, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeFusionQuery
    let len;
    let data = new arbeFusionQuery(null);
    // Deserialize message field [obj_id]
    data.obj_id = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [top]
    data.top = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [left]
    data.left = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [bottom]
    data.bottom = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [right]
    data.right = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [query_id]
    data.query_id = _deserializer.uint32(buffer, bufferOffset);
    // Deserialize message field [query_time]
    data.query_time = _deserializer.uint64(buffer, bufferOffset);
    // Deserialize message field [obj_index]
    data.obj_index = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [tta_count]
    data.tta_count = _deserializer.uint8(buffer, bufferOffset);
    // Deserialize message field [tta_scores]
    data.tta_scores = _arrayDeserializer.float32(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 4 * object.tta_scores.length;
    return length + 29;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeFusionQuery';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'd6277f1c47cde1be60fd589e75618a8d';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint16  	obj_id
    uint16  	top
    uint16  	left
    uint16  	bottom
    uint16  	right
    uint32  	query_id
    uint64	  	query_time
    uint16  	obj_index
    uint8  	tta_count
    float32[] tta_scores
    
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeFusionQuery(null);
    if (msg.obj_id !== undefined) {
      resolved.obj_id = msg.obj_id;
    }
    else {
      resolved.obj_id = 0
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

    if (msg.query_id !== undefined) {
      resolved.query_id = msg.query_id;
    }
    else {
      resolved.query_id = 0
    }

    if (msg.query_time !== undefined) {
      resolved.query_time = msg.query_time;
    }
    else {
      resolved.query_time = 0
    }

    if (msg.obj_index !== undefined) {
      resolved.obj_index = msg.obj_index;
    }
    else {
      resolved.obj_index = 0
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

    return resolved;
    }
};

module.exports = arbeFusionQuery;
