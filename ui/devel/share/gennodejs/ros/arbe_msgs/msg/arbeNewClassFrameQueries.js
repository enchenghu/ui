// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeClassificationMetadata = require('./arbeClassificationMetadata.js');
let arbeClassSingleQuery = require('./arbeClassSingleQuery.js');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeNewClassFrameQueries {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.Header = null;
      this.class_metadata = null;
      this.frame_counter = null;
      this.queries = null;
      this.mutual_len = null;
    }
    else {
      if (initObj.hasOwnProperty('Header')) {
        this.Header = initObj.Header
      }
      else {
        this.Header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('class_metadata')) {
        this.class_metadata = initObj.class_metadata
      }
      else {
        this.class_metadata = new arbeClassificationMetadata();
      }
      if (initObj.hasOwnProperty('frame_counter')) {
        this.frame_counter = initObj.frame_counter
      }
      else {
        this.frame_counter = 0;
      }
      if (initObj.hasOwnProperty('queries')) {
        this.queries = initObj.queries
      }
      else {
        this.queries = [];
      }
      if (initObj.hasOwnProperty('mutual_len')) {
        this.mutual_len = initObj.mutual_len
      }
      else {
        this.mutual_len = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeNewClassFrameQueries
    // Serialize message field [Header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.Header, buffer, bufferOffset);
    // Serialize message field [class_metadata]
    bufferOffset = arbeClassificationMetadata.serialize(obj.class_metadata, buffer, bufferOffset);
    // Serialize message field [frame_counter]
    bufferOffset = _serializer.uint16(obj.frame_counter, buffer, bufferOffset);
    // Serialize message field [queries]
    // Serialize the length for message field [queries]
    bufferOffset = _serializer.uint32(obj.queries.length, buffer, bufferOffset);
    obj.queries.forEach((val) => {
      bufferOffset = arbeClassSingleQuery.serialize(val, buffer, bufferOffset);
    });
    // Serialize message field [mutual_len]
    bufferOffset = _serializer.uint16(obj.mutual_len, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeNewClassFrameQueries
    let len;
    let data = new arbeNewClassFrameQueries(null);
    // Deserialize message field [Header]
    data.Header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [class_metadata]
    data.class_metadata = arbeClassificationMetadata.deserialize(buffer, bufferOffset);
    // Deserialize message field [frame_counter]
    data.frame_counter = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [queries]
    // Deserialize array length for message field [queries]
    len = _deserializer.uint32(buffer, bufferOffset);
    data.queries = new Array(len);
    for (let i = 0; i < len; ++i) {
      data.queries[i] = arbeClassSingleQuery.deserialize(buffer, bufferOffset)
    }
    // Deserialize message field [mutual_len]
    data.mutual_len = _deserializer.uint16(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.Header);
    object.queries.forEach((val) => {
      length += arbeClassSingleQuery.getMessageSize(val);
    });
    return length + 20;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeNewClassFrameQueries';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '01f7dd27368405062b7c22b947b01851';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header Header
    arbeClassificationMetadata class_metadata
    uint16 frame_counter
    arbeClassSingleQuery[] queries
    uint16 mutual_len
    
    
    ================================================================================
    MSG: std_msgs/Header
    # Standard metadata for higher-level stamped data types.
    # This is generally used to communicate timestamped data 
    # in a particular coordinate frame.
    # 
    # sequence ID: consecutively increasing ID 
    uint32 seq
    #Two-integer timestamp that is expressed as:
    # * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
    # * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
    # time-handling sugar is provided by the client library
    time stamp
    #Frame this data is associated with
    string frame_id
    
    ================================================================================
    MSG: arbe_msgs/arbeClassificationMetadata
    bool enable_tta
    bool post_factum_mode
    bool average_mode
    bool tta_decay
    float32 tta_momentum
    float32 conf_TH
    
    ================================================================================
    MSG: arbe_msgs/arbeClassSingleQuery
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
    const resolved = new arbeNewClassFrameQueries(null);
    if (msg.Header !== undefined) {
      resolved.Header = std_msgs.msg.Header.Resolve(msg.Header)
    }
    else {
      resolved.Header = new std_msgs.msg.Header()
    }

    if (msg.class_metadata !== undefined) {
      resolved.class_metadata = arbeClassificationMetadata.Resolve(msg.class_metadata)
    }
    else {
      resolved.class_metadata = new arbeClassificationMetadata()
    }

    if (msg.frame_counter !== undefined) {
      resolved.frame_counter = msg.frame_counter;
    }
    else {
      resolved.frame_counter = 0
    }

    if (msg.queries !== undefined) {
      resolved.queries = new Array(msg.queries.length);
      for (let i = 0; i < resolved.queries.length; ++i) {
        resolved.queries[i] = arbeClassSingleQuery.Resolve(msg.queries[i]);
      }
    }
    else {
      resolved.queries = []
    }

    if (msg.mutual_len !== undefined) {
      resolved.mutual_len = msg.mutual_len;
    }
    else {
      resolved.mutual_len = 0
    }

    return resolved;
    }
};

module.exports = arbeNewClassFrameQueries;
