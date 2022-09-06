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

class arbePolarFreeSpacePoly {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.frameCounter = null;
      this.rPerAzimuth = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('frameCounter')) {
        this.frameCounter = initObj.frameCounter
      }
      else {
        this.frameCounter = 0;
      }
      if (initObj.hasOwnProperty('rPerAzimuth')) {
        this.rPerAzimuth = initObj.rPerAzimuth
      }
      else {
        this.rPerAzimuth = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbePolarFreeSpacePoly
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [frameCounter]
    bufferOffset = _serializer.uint16(obj.frameCounter, buffer, bufferOffset);
    // Serialize message field [rPerAzimuth]
    bufferOffset = _arraySerializer.float32(obj.rPerAzimuth, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbePolarFreeSpacePoly
    let len;
    let data = new arbePolarFreeSpacePoly(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [frameCounter]
    data.frameCounter = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [rPerAzimuth]
    data.rPerAzimuth = _arrayDeserializer.float32(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    length += 4 * object.rPerAzimuth.length;
    return length + 6;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbePolarFreeSpacePoly';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '3e4fe2f26c99bce1f836beb8bd1176f9';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header header
    uint16 frameCounter
    float32[] rPerAzimuth
    
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
    const resolved = new arbePolarFreeSpacePoly(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.frameCounter !== undefined) {
      resolved.frameCounter = msg.frameCounter;
    }
    else {
      resolved.frameCounter = 0
    }

    if (msg.rPerAzimuth !== undefined) {
      resolved.rPerAzimuth = msg.rPerAzimuth;
    }
    else {
      resolved.rPerAzimuth = []
    }

    return resolved;
    }
};

module.exports = arbePolarFreeSpacePoly;
