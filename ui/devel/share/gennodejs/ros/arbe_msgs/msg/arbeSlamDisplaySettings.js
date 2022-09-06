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

class arbeSlamDisplaySettings {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.slam_on_camera = null;
      this.disp_dist_on_cam = null;
      this.color_by_class = null;
      this.show_classification = null;
      this.disp_funnel = null;
      this.disp_FS_on_cam = null;
      this.disp_slam_eco_mode = null;
    }
    else {
      if (initObj.hasOwnProperty('slam_on_camera')) {
        this.slam_on_camera = initObj.slam_on_camera
      }
      else {
        this.slam_on_camera = false;
      }
      if (initObj.hasOwnProperty('disp_dist_on_cam')) {
        this.disp_dist_on_cam = initObj.disp_dist_on_cam
      }
      else {
        this.disp_dist_on_cam = false;
      }
      if (initObj.hasOwnProperty('color_by_class')) {
        this.color_by_class = initObj.color_by_class
      }
      else {
        this.color_by_class = false;
      }
      if (initObj.hasOwnProperty('show_classification')) {
        this.show_classification = initObj.show_classification
      }
      else {
        this.show_classification = false;
      }
      if (initObj.hasOwnProperty('disp_funnel')) {
        this.disp_funnel = initObj.disp_funnel
      }
      else {
        this.disp_funnel = false;
      }
      if (initObj.hasOwnProperty('disp_FS_on_cam')) {
        this.disp_FS_on_cam = initObj.disp_FS_on_cam
      }
      else {
        this.disp_FS_on_cam = false;
      }
      if (initObj.hasOwnProperty('disp_slam_eco_mode')) {
        this.disp_slam_eco_mode = initObj.disp_slam_eco_mode
      }
      else {
        this.disp_slam_eco_mode = false;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeSlamDisplaySettings
    // Serialize message field [slam_on_camera]
    bufferOffset = _serializer.bool(obj.slam_on_camera, buffer, bufferOffset);
    // Serialize message field [disp_dist_on_cam]
    bufferOffset = _serializer.bool(obj.disp_dist_on_cam, buffer, bufferOffset);
    // Serialize message field [color_by_class]
    bufferOffset = _serializer.bool(obj.color_by_class, buffer, bufferOffset);
    // Serialize message field [show_classification]
    bufferOffset = _serializer.bool(obj.show_classification, buffer, bufferOffset);
    // Serialize message field [disp_funnel]
    bufferOffset = _serializer.bool(obj.disp_funnel, buffer, bufferOffset);
    // Serialize message field [disp_FS_on_cam]
    bufferOffset = _serializer.bool(obj.disp_FS_on_cam, buffer, bufferOffset);
    // Serialize message field [disp_slam_eco_mode]
    bufferOffset = _serializer.bool(obj.disp_slam_eco_mode, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeSlamDisplaySettings
    let len;
    let data = new arbeSlamDisplaySettings(null);
    // Deserialize message field [slam_on_camera]
    data.slam_on_camera = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [disp_dist_on_cam]
    data.disp_dist_on_cam = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [color_by_class]
    data.color_by_class = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [show_classification]
    data.show_classification = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [disp_funnel]
    data.disp_funnel = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [disp_FS_on_cam]
    data.disp_FS_on_cam = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [disp_slam_eco_mode]
    data.disp_slam_eco_mode = _deserializer.bool(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 7;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeSlamDisplaySettings';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '137b1eba43f6932882c483d9b1c16de3';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    bool slam_on_camera
    bool disp_dist_on_cam
    bool color_by_class
    bool show_classification
    bool disp_funnel
    bool disp_FS_on_cam
    bool disp_slam_eco_mode
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeSlamDisplaySettings(null);
    if (msg.slam_on_camera !== undefined) {
      resolved.slam_on_camera = msg.slam_on_camera;
    }
    else {
      resolved.slam_on_camera = false
    }

    if (msg.disp_dist_on_cam !== undefined) {
      resolved.disp_dist_on_cam = msg.disp_dist_on_cam;
    }
    else {
      resolved.disp_dist_on_cam = false
    }

    if (msg.color_by_class !== undefined) {
      resolved.color_by_class = msg.color_by_class;
    }
    else {
      resolved.color_by_class = false
    }

    if (msg.show_classification !== undefined) {
      resolved.show_classification = msg.show_classification;
    }
    else {
      resolved.show_classification = false
    }

    if (msg.disp_funnel !== undefined) {
      resolved.disp_funnel = msg.disp_funnel;
    }
    else {
      resolved.disp_funnel = false
    }

    if (msg.disp_FS_on_cam !== undefined) {
      resolved.disp_FS_on_cam = msg.disp_FS_on_cam;
    }
    else {
      resolved.disp_FS_on_cam = false
    }

    if (msg.disp_slam_eco_mode !== undefined) {
      resolved.disp_slam_eco_mode = msg.disp_slam_eco_mode;
    }
    else {
      resolved.disp_slam_eco_mode = false
    }

    return resolved;
    }
};

module.exports = arbeSlamDisplaySettings;
