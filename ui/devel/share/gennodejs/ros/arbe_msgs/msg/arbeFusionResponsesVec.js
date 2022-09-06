// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeFusionResponse = require('./arbeFusionResponse.js');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeFusionResponsesVec {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.dt = null;
      this.responses = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('dt')) {
        this.dt = initObj.dt
      }
      else {
        this.dt = 0;
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
    // Serializes a message object of type arbeFusionResponsesVec
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [dt]
    bufferOffset = _serializer.uint64(obj.dt, buffer, bufferOffset);
    // Serialize message field [responses]
    // Serialize the length for message field [responses]
    bufferOffset = _serializer.uint32(obj.responses.length, buffer, bufferOffset);
    obj.responses.forEach((val) => {
      bufferOffset = arbeFusionResponse.serialize(val, buffer, bufferOffset);
    });
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeFusionResponsesVec
    let len;
    let data = new arbeFusionResponsesVec(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [dt]
    data.dt = _deserializer.uint64(buffer, bufferOffset);
    // Deserialize message field [responses]
    // Deserialize array length for message field [responses]
    len = _deserializer.uint32(buffer, bufferOffset);
    data.responses = new Array(len);
    for (let i = 0; i < len; ++i) {
      data.responses[i] = arbeFusionResponse.deserialize(buffer, bufferOffset)
    }
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    object.responses.forEach((val) => {
      length += arbeFusionResponse.getMessageSize(val);
    });
    return length + 12;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeFusionResponsesVec';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'e85897c08185dc25f773baf17831351f';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header header
    uint64 dt
    arbeFusionResponse[] responses
    
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
    MSG: arbe_msgs/arbeFusionResponse
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
    const resolved = new arbeFusionResponsesVec(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.dt !== undefined) {
      resolved.dt = msg.dt;
    }
    else {
      resolved.dt = 0
    }

    if (msg.responses !== undefined) {
      resolved.responses = new Array(msg.responses.length);
      for (let i = 0; i < resolved.responses.length; ++i) {
        resolved.responses[i] = arbeFusionResponse.Resolve(msg.responses[i]);
      }
    }
    else {
      resolved.responses = []
    }

    return resolved;
    }
};

module.exports = arbeFusionResponsesVec;
