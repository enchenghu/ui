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

class arbeClassSingleQuery {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.obj_id = null;
      this.obj_index = null;
      this.n_points = null;
      this.xyzvp = null;
      this.xyzvp_center = null;
      this.tta_scores = null;
      this.tta_count = null;
    }
    else {
      if (initObj.hasOwnProperty('obj_id')) {
        this.obj_id = initObj.obj_id
      }
      else {
        this.obj_id = 0;
      }
      if (initObj.hasOwnProperty('obj_index')) {
        this.obj_index = initObj.obj_index
      }
      else {
        this.obj_index = 0;
      }
      if (initObj.hasOwnProperty('n_points')) {
        this.n_points = initObj.n_points
      }
      else {
        this.n_points = 0;
      }
      if (initObj.hasOwnProperty('xyzvp')) {
        this.xyzvp = initObj.xyzvp
      }
      else {
        this.xyzvp = [];
      }
      if (initObj.hasOwnProperty('xyzvp_center')) {
        this.xyzvp_center = initObj.xyzvp_center
      }
      else {
        this.xyzvp_center = [];
      }
      if (initObj.hasOwnProperty('tta_scores')) {
        this.tta_scores = initObj.tta_scores
      }
      else {
        this.tta_scores = [];
      }
      if (initObj.hasOwnProperty('tta_count')) {
        this.tta_count = initObj.tta_count
      }
      else {
        this.tta_count = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeClassSingleQuery
    // Serialize message field [obj_id]
    bufferOffset = _serializer.uint16(obj.obj_id, buffer, bufferOffset);
    // Serialize message field [obj_index]
    bufferOffset = _serializer.uint16(obj.obj_index, buffer, bufferOffset);
    // Serialize message field [n_points]
    bufferOffset = _serializer.uint16(obj.n_points, buffer, bufferOffset);
    // Serialize message field [xyzvp]
    bufferOffset = _arraySerializer.float32(obj.xyzvp, buffer, bufferOffset, null);
    // Serialize message field [xyzvp_center]
    bufferOffset = _arraySerializer.float32(obj.xyzvp_center, buffer, bufferOffset, null);
    // Serialize message field [tta_scores]
    bufferOffset = _arraySerializer.float32(obj.tta_scores, buffer, bufferOffset, null);
    // Serialize message field [tta_count]
    bufferOffset = _serializer.uint32(obj.tta_count, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeClassSingleQuery
    let len;
    let data = new arbeClassSingleQuery(null);
    // Deserialize message field [obj_id]
    data.obj_id = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [obj_index]
    data.obj_index = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [n_points]
    data.n_points = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [xyzvp]
    data.xyzvp = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [xyzvp_center]
    data.xyzvp_center = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [tta_scores]
    data.tta_scores = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [tta_count]
    data.tta_count = _deserializer.uint32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 4 * object.xyzvp.length;
    length += 4 * object.xyzvp_center.length;
    length += 4 * object.tta_scores.length;
    return length + 22;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeClassSingleQuery';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '64f8fe3f54997e174c21d53624f859dd';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint16 obj_id
    uint16 obj_index
    uint16 n_points
    float32[] xyzvp
    float32[] xyzvp_center
    float32[] tta_scores
    uint32 tta_count
    
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeClassSingleQuery(null);
    if (msg.obj_id !== undefined) {
      resolved.obj_id = msg.obj_id;
    }
    else {
      resolved.obj_id = 0
    }

    if (msg.obj_index !== undefined) {
      resolved.obj_index = msg.obj_index;
    }
    else {
      resolved.obj_index = 0
    }

    if (msg.n_points !== undefined) {
      resolved.n_points = msg.n_points;
    }
    else {
      resolved.n_points = 0
    }

    if (msg.xyzvp !== undefined) {
      resolved.xyzvp = msg.xyzvp;
    }
    else {
      resolved.xyzvp = []
    }

    if (msg.xyzvp_center !== undefined) {
      resolved.xyzvp_center = msg.xyzvp_center;
    }
    else {
      resolved.xyzvp_center = []
    }

    if (msg.tta_scores !== undefined) {
      resolved.tta_scores = msg.tta_scores;
    }
    else {
      resolved.tta_scores = []
    }

    if (msg.tta_count !== undefined) {
      resolved.tta_count = msg.tta_count;
    }
    else {
      resolved.tta_count = 0
    }

    return resolved;
    }
};

module.exports = arbeClassSingleQuery;
