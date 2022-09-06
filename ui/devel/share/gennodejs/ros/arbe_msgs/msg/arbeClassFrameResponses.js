// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeClassSingleResponse = require('./arbeClassSingleResponse.js');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeClassFrameResponses {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.frame_counter = null;
      this.process_time_ms = null;
      this.responses = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('frame_counter')) {
        this.frame_counter = initObj.frame_counter
      }
      else {
        this.frame_counter = 0;
      }
      if (initObj.hasOwnProperty('process_time_ms')) {
        this.process_time_ms = initObj.process_time_ms
      }
      else {
        this.process_time_ms = 0;
      }
      if (initObj.hasOwnProperty('responses')) {
        this.responses = initObj.responses
      }
      else {
        this.responses = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeClassFrameResponses
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [frame_counter]
    bufferOffset = _serializer.uint16(obj.frame_counter, buffer, bufferOffset);
    // Serialize message field [process_time_ms]
    bufferOffset = _serializer.uint64(obj.process_time_ms, buffer, bufferOffset);
    // Serialize message field [responses]
    // Serialize the length for message field [responses]
    bufferOffset = _serializer.uint32(obj.responses.length, buffer, bufferOffset);
    obj.responses.forEach((val) => {
      bufferOffset = arbeClassSingleResponse.serialize(val, buffer, bufferOffset);
    });
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeClassFrameResponses
    let len;
    let data = new arbeClassFrameResponses(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [frame_counter]
    data.frame_counter = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [process_time_ms]
    data.process_time_ms = _deserializer.uint64(buffer, bufferOffset);
    // Deserialize message field [responses]
    // Deserialize array length for message field [responses]
    len = _deserializer.uint32(buffer, bufferOffset);
    data.responses = new Array(len);
    for (let i = 0; i < len; ++i) {
      data.responses[i] = arbeClassSingleResponse.deserialize(buffer, bufferOffset)
    }
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    object.responses.forEach((val) => {
      length += arbeClassSingleResponse.getMessageSize(val);
    });
    return length + 14;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeClassFrameResponses';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '5abc2de3f3441b5705fbb97c192204d4';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header header
    uint16 frame_counter
    uint64 process_time_ms
    arbeClassSingleResponse[] responses
    
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
    MSG: arbe_msgs/arbeClassSingleResponse
    bool valid
    uint16 obj_id
    uint16 obj_index
    uint8 inferredClass
    float32 conf
    string class_str
    arbeFusionClasses fusionClasses
    float32[] tta_scores
    uint32 tta_count
    
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
    const resolved = new arbeClassFrameResponses(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.frame_counter !== undefined) {
      resolved.frame_counter = msg.frame_counter;
    }
    else {
      resolved.frame_counter = 0
    }

    if (msg.process_time_ms !== undefined) {
      resolved.process_time_ms = msg.process_time_ms;
    }
    else {
      resolved.process_time_ms = 0
    }

    if (msg.responses !== undefined) {
      resolved.responses = new Array(msg.responses.length);
      for (let i = 0; i < resolved.responses.length; ++i) {
        resolved.responses[i] = arbeClassSingleResponse.Resolve(msg.responses[i]);
      }
    }
    else {
      resolved.responses = []
    }

    return resolved;
    }
};

module.exports = arbeClassFrameResponses;
