// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbePcFrameHeader = require('./arbePcFrameHeader.js');
let arbePcMetadata = require('./arbePcMetadata.js');
let arbePcBins = require('./arbePcBins.js');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeNewPcMsg {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.RosHeader = null;
      this.PcHeader = null;
      this.PcMetadata = null;
      this.Points = null;
    }
    else {
      if (initObj.hasOwnProperty('RosHeader')) {
        this.RosHeader = initObj.RosHeader
      }
      else {
        this.RosHeader = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('PcHeader')) {
        this.PcHeader = initObj.PcHeader
      }
      else {
        this.PcHeader = new arbePcFrameHeader();
      }
      if (initObj.hasOwnProperty('PcMetadata')) {
        this.PcMetadata = initObj.PcMetadata
      }
      else {
        this.PcMetadata = new arbePcMetadata();
      }
      if (initObj.hasOwnProperty('Points')) {
        this.Points = initObj.Points
      }
      else {
        this.Points = new arbePcBins();
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeNewPcMsg
    // Serialize message field [RosHeader]
    bufferOffset = std_msgs.msg.Header.serialize(obj.RosHeader, buffer, bufferOffset);
    // Serialize message field [PcHeader]
    bufferOffset = arbePcFrameHeader.serialize(obj.PcHeader, buffer, bufferOffset);
    // Serialize message field [PcMetadata]
    bufferOffset = arbePcMetadata.serialize(obj.PcMetadata, buffer, bufferOffset);
    // Serialize message field [Points]
    bufferOffset = arbePcBins.serialize(obj.Points, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeNewPcMsg
    let len;
    let data = new arbeNewPcMsg(null);
    // Deserialize message field [RosHeader]
    data.RosHeader = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [PcHeader]
    data.PcHeader = arbePcFrameHeader.deserialize(buffer, bufferOffset);
    // Deserialize message field [PcMetadata]
    data.PcMetadata = arbePcMetadata.deserialize(buffer, bufferOffset);
    // Deserialize message field [Points]
    data.Points = arbePcBins.deserialize(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.RosHeader);
    length += arbePcBins.getMessageSize(object.Points);
    return length + 60;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeNewPcMsg';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '6537ae358990354b634606bcfb0fa7db';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
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
    const resolved = new arbeNewPcMsg(null);
    if (msg.RosHeader !== undefined) {
      resolved.RosHeader = std_msgs.msg.Header.Resolve(msg.RosHeader)
    }
    else {
      resolved.RosHeader = new std_msgs.msg.Header()
    }

    if (msg.PcHeader !== undefined) {
      resolved.PcHeader = arbePcFrameHeader.Resolve(msg.PcHeader)
    }
    else {
      resolved.PcHeader = new arbePcFrameHeader()
    }

    if (msg.PcMetadata !== undefined) {
      resolved.PcMetadata = arbePcMetadata.Resolve(msg.PcMetadata)
    }
    else {
      resolved.PcMetadata = new arbePcMetadata()
    }

    if (msg.Points !== undefined) {
      resolved.Points = arbePcBins.Resolve(msg.Points)
    }
    else {
      resolved.Points = new arbePcBins()
    }

    return resolved;
    }
};

module.exports = arbeNewPcMsg;
