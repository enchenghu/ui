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
let arbeFusionResponse = require('./arbeFusionResponse.js');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeClassificationTrainingPc {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.Header = null;
      this.Pc = null;
      this.fusionResponse = null;
      this.matching_com_indices = null;
    }
    else {
      if (initObj.hasOwnProperty('Header')) {
        this.Header = initObj.Header
      }
      else {
        this.Header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('Pc')) {
        this.Pc = initObj.Pc
      }
      else {
        this.Pc = new arbeNewPcMsg();
      }
      if (initObj.hasOwnProperty('fusionResponse')) {
        this.fusionResponse = initObj.fusionResponse
      }
      else {
        this.fusionResponse = new arbeFusionResponse();
      }
      if (initObj.hasOwnProperty('matching_com_indices')) {
        this.matching_com_indices = initObj.matching_com_indices
      }
      else {
        this.matching_com_indices = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeClassificationTrainingPc
    // Serialize message field [Header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.Header, buffer, bufferOffset);
    // Serialize message field [Pc]
    bufferOffset = arbeNewPcMsg.serialize(obj.Pc, buffer, bufferOffset);
    // Serialize message field [fusionResponse]
    bufferOffset = arbeFusionResponse.serialize(obj.fusionResponse, buffer, bufferOffset);
    // Serialize message field [matching_com_indices]
    bufferOffset = _arraySerializer.uint16(obj.matching_com_indices, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeClassificationTrainingPc
    let len;
    let data = new arbeClassificationTrainingPc(null);
    // Deserialize message field [Header]
    data.Header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [Pc]
    data.Pc = arbeNewPcMsg.deserialize(buffer, bufferOffset);
    // Deserialize message field [fusionResponse]
    data.fusionResponse = arbeFusionResponse.deserialize(buffer, bufferOffset);
    // Deserialize message field [matching_com_indices]
    data.matching_com_indices = _arrayDeserializer.uint16(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.Header);
    length += arbeNewPcMsg.getMessageSize(object.Pc);
    length += arbeFusionResponse.getMessageSize(object.fusionResponse);
    length += 2 * object.matching_com_indices.length;
    return length + 4;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeClassificationTrainingPc';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'e9b7fe5d9944489810bf1fbe1b2cac94';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header Header
    arbeNewPcMsg Pc
    arbeFusionResponse fusionResponse
    uint16[] matching_com_indices
    
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
    MSG: arbe_msgs/arbeNewPcMsg
    std_msgs/Header RosHeader
    arbePcFrameHeader PcHeader
    arbePcMetadata PcMetadata
    arbePcBins Points
    
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
    const resolved = new arbeClassificationTrainingPc(null);
    if (msg.Header !== undefined) {
      resolved.Header = std_msgs.msg.Header.Resolve(msg.Header)
    }
    else {
      resolved.Header = new std_msgs.msg.Header()
    }

    if (msg.Pc !== undefined) {
      resolved.Pc = arbeNewPcMsg.Resolve(msg.Pc)
    }
    else {
      resolved.Pc = new arbeNewPcMsg()
    }

    if (msg.fusionResponse !== undefined) {
      resolved.fusionResponse = arbeFusionResponse.Resolve(msg.fusionResponse)
    }
    else {
      resolved.fusionResponse = new arbeFusionResponse()
    }

    if (msg.matching_com_indices !== undefined) {
      resolved.matching_com_indices = msg.matching_com_indices;
    }
    else {
      resolved.matching_com_indices = []
    }

    return resolved;
    }
};

module.exports = arbeClassificationTrainingPc;
