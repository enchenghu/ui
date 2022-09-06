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

class arbeRdInclination {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.ant_height = null;
      this.ant_tilt = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('ant_height')) {
        this.ant_height = initObj.ant_height
      }
      else {
        this.ant_height = 0.0;
      }
      if (initObj.hasOwnProperty('ant_tilt')) {
        this.ant_tilt = initObj.ant_tilt
      }
      else {
        this.ant_tilt = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeRdInclination
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [ant_height]
    bufferOffset = _serializer.float32(obj.ant_height, buffer, bufferOffset);
    // Serialize message field [ant_tilt]
    bufferOffset = _serializer.float32(obj.ant_tilt, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeRdInclination
    let len;
    let data = new arbeRdInclination(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [ant_height]
    data.ant_height = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [ant_tilt]
    data.ant_tilt = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    return length + 8;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeRdInclination';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '401e82b1a597051854c2fb29fdf032ba';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header header
    float32 ant_height
    float32 ant_tilt
    
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
    const resolved = new arbeRdInclination(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.ant_height !== undefined) {
      resolved.ant_height = msg.ant_height;
    }
    else {
      resolved.ant_height = 0.0
    }

    if (msg.ant_tilt !== undefined) {
      resolved.ant_tilt = msg.ant_tilt;
    }
    else {
      resolved.ant_tilt = 0.0
    }

    return resolved;
    }
};

module.exports = arbeRdInclination;
