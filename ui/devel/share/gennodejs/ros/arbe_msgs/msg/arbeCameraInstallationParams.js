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

class arbeCameraInstallationParams {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.intrinsic = null;
      this.extrinsic_trans = null;
      this.euler_a_b_g = null;
    }
    else {
      if (initObj.hasOwnProperty('intrinsic')) {
        this.intrinsic = initObj.intrinsic
      }
      else {
        this.intrinsic = [];
      }
      if (initObj.hasOwnProperty('extrinsic_trans')) {
        this.extrinsic_trans = initObj.extrinsic_trans
      }
      else {
        this.extrinsic_trans = [];
      }
      if (initObj.hasOwnProperty('euler_a_b_g')) {
        this.euler_a_b_g = initObj.euler_a_b_g
      }
      else {
        this.euler_a_b_g = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeCameraInstallationParams
    // Serialize message field [intrinsic]
    bufferOffset = _arraySerializer.float32(obj.intrinsic, buffer, bufferOffset, null);
    // Serialize message field [extrinsic_trans]
    bufferOffset = _arraySerializer.float32(obj.extrinsic_trans, buffer, bufferOffset, null);
    // Serialize message field [euler_a_b_g]
    bufferOffset = _arraySerializer.float32(obj.euler_a_b_g, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeCameraInstallationParams
    let len;
    let data = new arbeCameraInstallationParams(null);
    // Deserialize message field [intrinsic]
    data.intrinsic = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [extrinsic_trans]
    data.extrinsic_trans = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [euler_a_b_g]
    data.euler_a_b_g = _arrayDeserializer.float32(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 4 * object.intrinsic.length;
    length += 4 * object.extrinsic_trans.length;
    length += 4 * object.euler_a_b_g.length;
    return length + 12;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeCameraInstallationParams';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '5d1e488e854ebf395d5c87a2670d2622';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    float32[] intrinsic
    float32[] extrinsic_trans
    float32[] euler_a_b_g
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeCameraInstallationParams(null);
    if (msg.intrinsic !== undefined) {
      resolved.intrinsic = msg.intrinsic;
    }
    else {
      resolved.intrinsic = []
    }

    if (msg.extrinsic_trans !== undefined) {
      resolved.extrinsic_trans = msg.extrinsic_trans;
    }
    else {
      resolved.extrinsic_trans = []
    }

    if (msg.euler_a_b_g !== undefined) {
      resolved.euler_a_b_g = msg.euler_a_b_g;
    }
    else {
      resolved.euler_a_b_g = []
    }

    return resolved;
    }
};

module.exports = arbeCameraInstallationParams;
