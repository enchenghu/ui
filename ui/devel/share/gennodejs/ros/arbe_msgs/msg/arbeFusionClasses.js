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

class arbeFusionClasses {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
    }
    else {
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeFusionClasses
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeFusionClasses
    let len;
    let data = new arbeFusionClasses(null);
    return data;
  }

  static getMessageSize(object) {
    return 0;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeFusionClasses';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'c824281429998cf35dc2364ca411d089';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
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
    const resolved = new arbeFusionClasses(null);
    return resolved;
    }
};

// Constants for message
arbeFusionClasses.Constants = {
  FC_BIKE: 0,
  FC_TRUCK_BUS: 1,
  FC_VRU: 2,
  FC_CAR: 3,
  FC_NOT_CLASSIFIED: 4,
  NUM_CLASSES: 4,
}

module.exports = arbeFusionClasses;
