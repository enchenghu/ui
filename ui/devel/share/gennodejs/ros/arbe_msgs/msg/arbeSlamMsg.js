// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeTSlamMetadata = require('./arbeTSlamMetadata.js');
let arbeTSlamObj = require('./arbeTSlamObj.js');
let arbeFusionClasses = require('./arbeFusionClasses.js');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeSlamMsg {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.meta_data = null;
      this.ObjectsBuffer = null;
      this.fusionClasses = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('meta_data')) {
        this.meta_data = initObj.meta_data
      }
      else {
        this.meta_data = new arbeTSlamMetadata();
      }
      if (initObj.hasOwnProperty('ObjectsBuffer')) {
        this.ObjectsBuffer = initObj.ObjectsBuffer
      }
      else {
        this.ObjectsBuffer = [];
      }
      if (initObj.hasOwnProperty('fusionClasses')) {
        this.fusionClasses = initObj.fusionClasses
      }
      else {
        this.fusionClasses = new arbeFusionClasses();
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeSlamMsg
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [meta_data]
    bufferOffset = arbeTSlamMetadata.serialize(obj.meta_data, buffer, bufferOffset);
    // Serialize message field [ObjectsBuffer]
    // Serialize the length for message field [ObjectsBuffer]
    bufferOffset = _serializer.uint32(obj.ObjectsBuffer.length, buffer, bufferOffset);
    obj.ObjectsBuffer.forEach((val) => {
      bufferOffset = arbeTSlamObj.serialize(val, buffer, bufferOffset);
    });
    // Serialize message field [fusionClasses]
    bufferOffset = arbeFusionClasses.serialize(obj.fusionClasses, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeSlamMsg
    let len;
    let data = new arbeSlamMsg(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [meta_data]
    data.meta_data = arbeTSlamMetadata.deserialize(buffer, bufferOffset);
    // Deserialize message field [ObjectsBuffer]
    // Deserialize array length for message field [ObjectsBuffer]
    len = _deserializer.uint32(buffer, bufferOffset);
    data.ObjectsBuffer = new Array(len);
    for (let i = 0; i < len; ++i) {
      data.ObjectsBuffer[i] = arbeTSlamObj.deserialize(buffer, bufferOffset)
    }
    // Deserialize message field [fusionClasses]
    data.fusionClasses = arbeFusionClasses.deserialize(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    length += 86 * object.ObjectsBuffer.length;
    return length + 50;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeSlamMsg';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'ad8727f4b769ffeff212c8a8f681cd8a';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header header
    arbeTSlamMetadata  meta_data
    arbeTSlamObj[] ObjectsBuffer
    
    arbeFusionClasses fusionClasses
    uint16 FC_BIKE = 0
    uint16 FC_TRUCK_BUS = 1
    uint16 FC_VRU = 2
    uint16 FC_CAR = 3
    uint16 FC_NOT_CLASSIFIED = 4
    
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
    MSG: arbe_msgs/arbeTSlamMetadata
    uint64  CorresPcTime
    uint16  FrameNumber
    float32 HostHeading
    float32 HostHeadingUnc
    float32 HostVelocity
    float32 HostVelocityUnc
    float32 HostOmega		
    float32 HostOmegaUnc
    uint16  NumberOfObjects
    float32 local_catr_x
    float32 local_catr_y
    bool    location_valid
    bool    is_reset_geo_location
    
    
    ================================================================================
    MSG: arbe_msgs/arbeTSlamObj
    uint16              ID
    uint16              obj_class
    float32             class_conf
    arbeTSlamPos        position
    arbeTSlamVelocity   velocity 
    arbeTSlamBox        bounding_box
    uint32              age
    uint16              last_frame_update
    
    
    
    ================================================================================
    MSG: arbe_msgs/arbeTSlamPos
    float32 x 
    float32 y 
    float32 z 
    float32  dx 
    float32  dy 
    float32  dz
    
    ================================================================================
    MSG: arbe_msgs/arbeTSlamVelocity
    float32     x_dot 
    float32     y_dot 
    float32      dx_dot 
    float32      dy_dot 
    
    ================================================================================
    MSG: arbe_msgs/arbeTSlamBox
    float32 scale_x     
    float32 scale_y     
    float32 scale_z     
    float32 scale_x_unc 
    float32 scale_y_unc 
    float32 scale_z_unc 
    float32 orientation_unc  
    float32 orientation  
    
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
    const resolved = new arbeSlamMsg(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.meta_data !== undefined) {
      resolved.meta_data = arbeTSlamMetadata.Resolve(msg.meta_data)
    }
    else {
      resolved.meta_data = new arbeTSlamMetadata()
    }

    if (msg.ObjectsBuffer !== undefined) {
      resolved.ObjectsBuffer = new Array(msg.ObjectsBuffer.length);
      for (let i = 0; i < resolved.ObjectsBuffer.length; ++i) {
        resolved.ObjectsBuffer[i] = arbeTSlamObj.Resolve(msg.ObjectsBuffer[i]);
      }
    }
    else {
      resolved.ObjectsBuffer = []
    }

    if (msg.fusionClasses !== undefined) {
      resolved.fusionClasses = arbeFusionClasses.Resolve(msg.fusionClasses)
    }
    else {
      resolved.fusionClasses = new arbeFusionClasses()
    }

    return resolved;
    }
};

// Constants for message
arbeSlamMsg.Constants = {
  FC_BIKE: 0,
  FC_TRUCK_BUS: 1,
  FC_VRU: 2,
  FC_CAR: 3,
  FC_NOT_CLASSIFIED: 4,
}

module.exports = arbeSlamMsg;
