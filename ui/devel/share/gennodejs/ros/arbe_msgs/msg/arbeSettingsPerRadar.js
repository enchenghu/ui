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

class arbeSettingsPerRadar {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.n_radars = null;
      this.radar_for_text = null;
      this.ant_height = null;
      this.ant_pitch = null;
      this.cam_euler_a = null;
      this.cam_euler_b = null;
      this.cam_euler_g = null;
    }
    else {
      if (initObj.hasOwnProperty('n_radars')) {
        this.n_radars = initObj.n_radars
      }
      else {
        this.n_radars = 0;
      }
      if (initObj.hasOwnProperty('radar_for_text')) {
        this.radar_for_text = initObj.radar_for_text
      }
      else {
        this.radar_for_text = 0;
      }
      if (initObj.hasOwnProperty('ant_height')) {
        this.ant_height = initObj.ant_height
      }
      else {
        this.ant_height = [];
      }
      if (initObj.hasOwnProperty('ant_pitch')) {
        this.ant_pitch = initObj.ant_pitch
      }
      else {
        this.ant_pitch = [];
      }
      if (initObj.hasOwnProperty('cam_euler_a')) {
        this.cam_euler_a = initObj.cam_euler_a
      }
      else {
        this.cam_euler_a = [];
      }
      if (initObj.hasOwnProperty('cam_euler_b')) {
        this.cam_euler_b = initObj.cam_euler_b
      }
      else {
        this.cam_euler_b = [];
      }
      if (initObj.hasOwnProperty('cam_euler_g')) {
        this.cam_euler_g = initObj.cam_euler_g
      }
      else {
        this.cam_euler_g = [];
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeSettingsPerRadar
    // Serialize message field [n_radars]
    bufferOffset = _serializer.uint8(obj.n_radars, buffer, bufferOffset);
    // Serialize message field [radar_for_text]
    bufferOffset = _serializer.int8(obj.radar_for_text, buffer, bufferOffset);
    // Serialize message field [ant_height]
    bufferOffset = _arraySerializer.float32(obj.ant_height, buffer, bufferOffset, null);
    // Serialize message field [ant_pitch]
    bufferOffset = _arraySerializer.float32(obj.ant_pitch, buffer, bufferOffset, null);
    // Serialize message field [cam_euler_a]
    bufferOffset = _arraySerializer.float32(obj.cam_euler_a, buffer, bufferOffset, null);
    // Serialize message field [cam_euler_b]
    bufferOffset = _arraySerializer.float32(obj.cam_euler_b, buffer, bufferOffset, null);
    // Serialize message field [cam_euler_g]
    bufferOffset = _arraySerializer.float32(obj.cam_euler_g, buffer, bufferOffset, null);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeSettingsPerRadar
    let len;
    let data = new arbeSettingsPerRadar(null);
    // Deserialize message field [n_radars]
    data.n_radars = _deserializer.uint8(buffer, bufferOffset);
    // Deserialize message field [radar_for_text]
    data.radar_for_text = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [ant_height]
    data.ant_height = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [ant_pitch]
    data.ant_pitch = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [cam_euler_a]
    data.cam_euler_a = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [cam_euler_b]
    data.cam_euler_b = _arrayDeserializer.float32(buffer, bufferOffset, null)
    // Deserialize message field [cam_euler_g]
    data.cam_euler_g = _arrayDeserializer.float32(buffer, bufferOffset, null)
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += 4 * object.ant_height.length;
    length += 4 * object.ant_pitch.length;
    length += 4 * object.cam_euler_a.length;
    length += 4 * object.cam_euler_b.length;
    length += 4 * object.cam_euler_g.length;
    return length + 22;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeSettingsPerRadar';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '0ee305f5b1c77996631bbad5b0e5663d';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    uint8 n_radars
    int8  radar_for_text
    float32[] ant_height
    float32[] ant_pitch
    float32[] cam_euler_a
    float32[] cam_euler_b
    float32[] cam_euler_g
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeSettingsPerRadar(null);
    if (msg.n_radars !== undefined) {
      resolved.n_radars = msg.n_radars;
    }
    else {
      resolved.n_radars = 0
    }

    if (msg.radar_for_text !== undefined) {
      resolved.radar_for_text = msg.radar_for_text;
    }
    else {
      resolved.radar_for_text = 0
    }

    if (msg.ant_height !== undefined) {
      resolved.ant_height = msg.ant_height;
    }
    else {
      resolved.ant_height = []
    }

    if (msg.ant_pitch !== undefined) {
      resolved.ant_pitch = msg.ant_pitch;
    }
    else {
      resolved.ant_pitch = []
    }

    if (msg.cam_euler_a !== undefined) {
      resolved.cam_euler_a = msg.cam_euler_a;
    }
    else {
      resolved.cam_euler_a = []
    }

    if (msg.cam_euler_b !== undefined) {
      resolved.cam_euler_b = msg.cam_euler_b;
    }
    else {
      resolved.cam_euler_b = []
    }

    if (msg.cam_euler_g !== undefined) {
      resolved.cam_euler_g = msg.cam_euler_g;
    }
    else {
      resolved.cam_euler_g = []
    }

    return resolved;
    }
};

module.exports = arbeSettingsPerRadar;
