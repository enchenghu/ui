// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeTAlgoDetectorThrInfo = require('./arbeTAlgoDetectorThrInfo.js');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeTAlgoDetectorThrOutput {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.RosHeader = null;
      this.tinfo = null;
    }
    else {
      if (initObj.hasOwnProperty('RosHeader')) {
        this.RosHeader = initObj.RosHeader
      }
      else {
        this.RosHeader = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('tinfo')) {
        this.tinfo = initObj.tinfo
      }
      else {
        this.tinfo = new arbeTAlgoDetectorThrInfo();
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeTAlgoDetectorThrOutput
    // Serialize message field [RosHeader]
    bufferOffset = std_msgs.msg.Header.serialize(obj.RosHeader, buffer, bufferOffset);
    // Serialize message field [tinfo]
    bufferOffset = arbeTAlgoDetectorThrInfo.serialize(obj.tinfo, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeTAlgoDetectorThrOutput
    let len;
    let data = new arbeTAlgoDetectorThrOutput(null);
    // Deserialize message field [RosHeader]
    data.RosHeader = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [tinfo]
    data.tinfo = arbeTAlgoDetectorThrInfo.deserialize(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.RosHeader);
    length += arbeTAlgoDetectorThrInfo.getMessageSize(object.tinfo);
    return length;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeTAlgoDetectorThrOutput';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '888d5fc086e6ad00dbaf413f9befa62b';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header RosHeader
    arbeTAlgoDetectorThrInfo tinfo
    
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
    MSG: arbe_msgs/arbeTAlgoDetectorThrInfo
    uint32 unFrameNumber
    uint8 eAlgoOutput
    uint8 eThresholdType
    uint8 eFrameType
    uint32[] Data
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeTAlgoDetectorThrOutput(null);
    if (msg.RosHeader !== undefined) {
      resolved.RosHeader = std_msgs.msg.Header.Resolve(msg.RosHeader)
    }
    else {
      resolved.RosHeader = new std_msgs.msg.Header()
    }

    if (msg.tinfo !== undefined) {
      resolved.tinfo = arbeTAlgoDetectorThrInfo.Resolve(msg.tinfo)
    }
    else {
      resolved.tinfo = new arbeTAlgoDetectorThrInfo()
    }

    return resolved;
    }
};

module.exports = arbeTAlgoDetectorThrOutput;
