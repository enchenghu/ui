// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeNewPcMsg = require('./arbeNewPcMsg.js');

//-----------------------------------------------------------

class arbePcWithValid {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.pc = null;
      this.valid = null;
    }
    else {
      if (initObj.hasOwnProperty('pc')) {
        this.pc = initObj.pc
      }
      else {
        this.pc = new arbeNewPcMsg();
      }
      if (initObj.hasOwnProperty('valid')) {
        this.valid = initObj.valid
      }
      else {
        this.valid = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbePcWithValid
    // Serialize message field [pc]
    bufferOffset = arbeNewPcMsg.serialize(obj.pc, buffer, bufferOffset);
    // Serialize message field [valid]
    bufferOffset = _arraySerializer.bool(obj.valid, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbePcWithValid
    let len;
    let data = new arbePcWithValid(null);
    // Deserialize message field [pc]
    data.pc = arbeNewPcMsg.deserialize(buffer, bufferOffset);
    // Deserialize message field [valid]
    data.valid = _arrayDeserializer.bool(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += arbeNewPcMsg.getMessageSize(object.pc);
    length += object.valid.length;
    return length + 4;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbePcWithValid';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'e4ea8e0ece0df084b29db472b924eebf';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    arbeNewPcMsg pc
    bool[] valid
    
    ================================================================================
    MSG: arbe_msgs/arbeNewPcMsg
    std_msgs/Header RosHeader
    arbePcFrameHeader PcHeader
    arbePcMetadata PcMetadata
    arbePcBins Points
    
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
    MSG: arbe_msgs/arbePcFrameHeader
    uint64 time
    uint16 frame_counter
    uint8  is_4d
    uint8  frame_type
    uint16 udp_lost_packages
    uint32 number_of_points
    uint16 crd_count
    arbeFrameTypes frame_types
    
    ================================================================================
    MSG: arbe_msgs/arbeFrameTypes
    uint8 Frame_Type_3d_short = 1
    uint8 Frame_Type_3d_mid = 2
    uint8 Frame_Type_3d_long = 3
    uint8 Frame_Type_3d_ultra_long = 13
    uint8 Frame_Type_4d_short = 4
    uint8 Frame_Type_4d_mid = 5
    uint8 Frame_Type_4d_long = 6
    uint8 Frame_Type_4d_ultra_long = 16
    
    ================================================================================
    MSG: arbe_msgs/arbePcMetadata
    arbePcRes PcResolution
    float32 range_offset
    float32 range_res_in_db
    uint32 azimuth_fft_size
    uint32 elevation_fft_size
    uint32 azimuth_fft_padding
    uint32 elevation_fft_padding
    
    
    ================================================================================
    MSG: arbe_msgs/arbePcRes
    float32 range_resolution
    float32 doppler_resolution
    float32 azimuth_coefficient
    float32 elevation_coefficient
    
    
    
    ================================================================================
    MSG: arbe_msgs/arbePcBins
    uint16[] range_bin
    int16[] doppler_signed_bin
    int8[] azimuth_signed_bin
    int8[] elevation_signed_bin
    int16[] phase
    int16[] power
    
    
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbePcWithValid(null);
    if (msg.pc !== undefined) {
      resolved.pc = arbeNewPcMsg.Resolve(msg.pc)
    }
    else {
      resolved.pc = new arbeNewPcMsg()
    }

    if (msg.valid !== undefined) {
      resolved.valid = msg.valid;
    }
    else {
      resolved.valid = []
    }

    return resolved;
    }
};

module.exports = arbePcWithValid;
