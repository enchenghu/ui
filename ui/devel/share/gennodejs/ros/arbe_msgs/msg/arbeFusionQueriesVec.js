// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeFusionQuery = require('./arbeFusionQuery.js');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeFusionQueriesVec {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.queries = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('queries')) {
        this.queries = initObj.queries
      }
      else {
        this.queries = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeFusionQueriesVec
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [queries]
    // Serialize the length for message field [queries]
    bufferOffset = _serializer.uint32(obj.queries.length, buffer, bufferOffset);
    obj.queries.forEach((val) => {
      bufferOffset = arbeFusionQuery.serialize(val, buffer, bufferOffset);
    });
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeFusionQueriesVec
    let len;
    let data = new arbeFusionQueriesVec(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [queries]
    // Deserialize array length for message field [queries]
    len = _deserializer.uint32(buffer, bufferOffset);
    data.queries = new Array(len);
    for (let i = 0; i < len; ++i) {
      data.queries[i] = arbeFusionQuery.deserialize(buffer, bufferOffset)
    }
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    object.queries.forEach((val) => {
      length += arbeFusionQuery.getMessageSize(val);
    });
    return length + 4;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeFusionQueriesVec';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'ba44955926ecbdeed3c3d177166ac4a8';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header header
    arbeFusionQuery[] queries
    
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
    MSG: arbe_msgs/arbeFusionQuery
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
    const resolved = new arbeFusionQueriesVec(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.queries !== undefined) {
      resolved.queries = new Array(msg.queries.length);
      for (let i = 0; i < resolved.queries.length; ++i) {
        resolved.queries[i] = arbeFusionQuery.Resolve(msg.queries[i]);
      }
    }
    else {
      resolved.queries = []
    }

    return resolved;
    }
};

module.exports = arbeFusionQueriesVec;
