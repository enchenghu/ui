// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class arbeFrameTypes {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
    }
    else {
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeFrameTypes
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeFrameTypes
    let len;
    let data = new arbeFrameTypes(null);
    return data;
  }

  static getMessageSize(object) {
    return 0;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeFrameTypes';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '60a899865eb222277c4a73fdc879e6e4';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint8 Frame_Type_3d_short = 1
    uint8 Frame_Type_3d_mid = 2
    uint8 Frame_Type_3d_long = 3
    uint8 Frame_Type_3d_ultra_long = 13
    uint8 Frame_Type_4d_short = 4
    uint8 Frame_Type_4d_mid = 5
    uint8 Frame_Type_4d_long = 6
    uint8 Frame_Type_4d_ultra_long = 16
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeFrameTypes(null);
    return resolved;
    }
};

// Constants for message
arbeFrameTypes.Constants = {
  FRAME_TYPE_3D_SHORT: 1,
  FRAME_TYPE_3D_MID: 2,
  FRAME_TYPE_3D_LONG: 3,
  FRAME_TYPE_3D_ULTRA_LONG: 13,
  FRAME_TYPE_4D_SHORT: 4,
  FRAME_TYPE_4D_MID: 5,
  FRAME_TYPE_4D_LONG: 6,
  FRAME_TYPE_4D_ULTRA_LONG: 16,
}

module.exports = arbeFrameTypes;
