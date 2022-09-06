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

class arbeClassificationMetadata {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.enable_tta = null;
      this.post_factum_mode = null;
      this.average_mode = null;
      this.tta_decay = null;
      this.tta_momentum = null;
      this.conf_TH = null;
    }
    else {
      if (initObj.hasOwnProperty('enable_tta')) {
        this.enable_tta = initObj.enable_tta
      }
      else {
        this.enable_tta = false;
      }
      if (initObj.hasOwnProperty('post_factum_mode')) {
        this.post_factum_mode = initObj.post_factum_mode
      }
      else {
        this.post_factum_mode = false;
      }
      if (initObj.hasOwnProperty('average_mode')) {
        this.average_mode = initObj.average_mode
      }
      else {
        this.average_mode = false;
      }
      if (initObj.hasOwnProperty('tta_decay')) {
        this.tta_decay = initObj.tta_decay
      }
      else {
        this.tta_decay = false;
      }
      if (initObj.hasOwnProperty('tta_momentum')) {
        this.tta_momentum = initObj.tta_momentum
      }
      else {
        this.tta_momentum = 0.0;
      }
      if (initObj.hasOwnProperty('conf_TH')) {
        this.conf_TH = initObj.conf_TH
      }
      else {
        this.conf_TH = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeClassificationMetadata
    // Serialize message field [enable_tta]
    bufferOffset = _serializer.bool(obj.enable_tta, buffer, bufferOffset);
    // Serialize message field [post_factum_mode]
    bufferOffset = _serializer.bool(obj.post_factum_mode, buffer, bufferOffset);
    // Serialize message field [average_mode]
    bufferOffset = _serializer.bool(obj.average_mode, buffer, bufferOffset);
    // Serialize message field [tta_decay]
    bufferOffset = _serializer.bool(obj.tta_decay, buffer, bufferOffset);
    // Serialize message field [tta_momentum]
    bufferOffset = _serializer.float32(obj.tta_momentum, buffer, bufferOffset);
    // Serialize message field [conf_TH]
    bufferOffset = _serializer.float32(obj.conf_TH, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeClassificationMetadata
    let len;
    let data = new arbeClassificationMetadata(null);
    // Deserialize message field [enable_tta]
    data.enable_tta = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [post_factum_mode]
    data.post_factum_mode = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [average_mode]
    data.average_mode = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [tta_decay]
    data.tta_decay = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [tta_momentum]
    data.tta_momentum = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [conf_TH]
    data.conf_TH = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 12;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeClassificationMetadata';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '2def84262941647c9070003f42808b6a';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    bool enable_tta
    bool post_factum_mode
    bool average_mode
    bool tta_decay
    float32 tta_momentum
    float32 conf_TH
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeClassificationMetadata(null);
    if (msg.enable_tta !== undefined) {
      resolved.enable_tta = msg.enable_tta;
    }
    else {
      resolved.enable_tta = false
    }

    if (msg.post_factum_mode !== undefined) {
      resolved.post_factum_mode = msg.post_factum_mode;
    }
    else {
      resolved.post_factum_mode = false
    }

    if (msg.average_mode !== undefined) {
      resolved.average_mode = msg.average_mode;
    }
    else {
      resolved.average_mode = false
    }

    if (msg.tta_decay !== undefined) {
      resolved.tta_decay = msg.tta_decay;
    }
    else {
      resolved.tta_decay = false
    }

    if (msg.tta_momentum !== undefined) {
      resolved.tta_momentum = msg.tta_momentum;
    }
    else {
      resolved.tta_momentum = 0.0
    }

    if (msg.conf_TH !== undefined) {
      resolved.conf_TH = msg.conf_TH;
    }
    else {
      resolved.conf_TH = 0.0
    }

    return resolved;
    }
};

module.exports = arbeClassificationMetadata;
