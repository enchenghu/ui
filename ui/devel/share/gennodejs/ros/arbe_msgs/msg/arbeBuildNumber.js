// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeBuildNumber {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.slam = null;
      this.arbe_gui = null;
      this.arbe_api_common = null;
      this.arbe_msgs = null;
      this.radar = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('slam')) {
        this.slam = initObj.slam
      }
      else {
        this.slam = 0;
      }
      if (initObj.hasOwnProperty('arbe_gui')) {
        this.arbe_gui = initObj.arbe_gui
      }
      else {
        this.arbe_gui = 0;
      }
      if (initObj.hasOwnProperty('arbe_api_common')) {
        this.arbe_api_common = initObj.arbe_api_common
      }
      else {
        this.arbe_api_common = 0;
      }
      if (initObj.hasOwnProperty('arbe_msgs')) {
        this.arbe_msgs = initObj.arbe_msgs
      }
      else {
        this.arbe_msgs = 0;
      }
      if (initObj.hasOwnProperty('radar')) {
        this.radar = initObj.radar
      }
      else {
        this.radar = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeBuildNumber
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [slam]
    bufferOffset = _serializer.int32(obj.slam, buffer, bufferOffset);
    // Serialize message field [arbe_gui]
    bufferOffset = _serializer.int32(obj.arbe_gui, buffer, bufferOffset);
    // Serialize message field [arbe_api_common]
    bufferOffset = _serializer.int32(obj.arbe_api_common, buffer, bufferOffset);
    // Serialize message field [arbe_msgs]
    bufferOffset = _serializer.int32(obj.arbe_msgs, buffer, bufferOffset);
    // Serialize message field [radar]
    bufferOffset = _serializer.int32(obj.radar, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeBuildNumber
    let len;
    let data = new arbeBuildNumber(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [slam]
    data.slam = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [arbe_gui]
    data.arbe_gui = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [arbe_api_common]
    data.arbe_api_common = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [arbe_msgs]
    data.arbe_msgs = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [radar]
    data.radar = _deserializer.int32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    return length + 20;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeBuildNumber';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '45c45703c45e662d3433cd39dca68964';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header header
    int32 slam
    int32 arbe_gui
    int32 arbe_api_common
    int32 arbe_msgs
    int32 radar
    
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
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeBuildNumber(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.slam !== undefined) {
      resolved.slam = msg.slam;
    }
    else {
      resolved.slam = 0
    }

    if (msg.arbe_gui !== undefined) {
      resolved.arbe_gui = msg.arbe_gui;
    }
    else {
      resolved.arbe_gui = 0
    }

    if (msg.arbe_api_common !== undefined) {
      resolved.arbe_api_common = msg.arbe_api_common;
    }
    else {
      resolved.arbe_api_common = 0
    }

    if (msg.arbe_msgs !== undefined) {
      resolved.arbe_msgs = msg.arbe_msgs;
    }
    else {
      resolved.arbe_msgs = 0
    }

    if (msg.radar !== undefined) {
      resolved.radar = msg.radar;
    }
    else {
      resolved.radar = 0
    }

    return resolved;
    }
};

module.exports = arbeBuildNumber;
