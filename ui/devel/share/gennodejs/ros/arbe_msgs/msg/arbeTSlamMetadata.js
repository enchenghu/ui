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

class arbeTSlamMetadata {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.CorresPcTime = null;
      this.FrameNumber = null;
      this.HostHeading = null;
      this.HostHeadingUnc = null;
      this.HostVelocity = null;
      this.HostVelocityUnc = null;
      this.HostOmega = null;
      this.HostOmegaUnc = null;
      this.NumberOfObjects = null;
      this.local_catr_x = null;
      this.local_catr_y = null;
      this.location_valid = null;
      this.is_reset_geo_location = null;
    }
    else {
      if (initObj.hasOwnProperty('CorresPcTime')) {
        this.CorresPcTime = initObj.CorresPcTime
      }
      else {
        this.CorresPcTime = 0;
      }
      if (initObj.hasOwnProperty('FrameNumber')) {
        this.FrameNumber = initObj.FrameNumber
      }
      else {
        this.FrameNumber = 0;
      }
      if (initObj.hasOwnProperty('HostHeading')) {
        this.HostHeading = initObj.HostHeading
      }
      else {
        this.HostHeading = 0.0;
      }
      if (initObj.hasOwnProperty('HostHeadingUnc')) {
        this.HostHeadingUnc = initObj.HostHeadingUnc
      }
      else {
        this.HostHeadingUnc = 0.0;
      }
      if (initObj.hasOwnProperty('HostVelocity')) {
        this.HostVelocity = initObj.HostVelocity
      }
      else {
        this.HostVelocity = 0.0;
      }
      if (initObj.hasOwnProperty('HostVelocityUnc')) {
        this.HostVelocityUnc = initObj.HostVelocityUnc
      }
      else {
        this.HostVelocityUnc = 0.0;
      }
      if (initObj.hasOwnProperty('HostOmega')) {
        this.HostOmega = initObj.HostOmega
      }
      else {
        this.HostOmega = 0.0;
      }
      if (initObj.hasOwnProperty('HostOmegaUnc')) {
        this.HostOmegaUnc = initObj.HostOmegaUnc
      }
      else {
        this.HostOmegaUnc = 0.0;
      }
      if (initObj.hasOwnProperty('NumberOfObjects')) {
        this.NumberOfObjects = initObj.NumberOfObjects
      }
      else {
        this.NumberOfObjects = 0;
      }
      if (initObj.hasOwnProperty('local_catr_x')) {
        this.local_catr_x = initObj.local_catr_x
      }
      else {
        this.local_catr_x = 0.0;
      }
      if (initObj.hasOwnProperty('local_catr_y')) {
        this.local_catr_y = initObj.local_catr_y
      }
      else {
        this.local_catr_y = 0.0;
      }
      if (initObj.hasOwnProperty('location_valid')) {
        this.location_valid = initObj.location_valid
      }
      else {
        this.location_valid = false;
      }
      if (initObj.hasOwnProperty('is_reset_geo_location')) {
        this.is_reset_geo_location = initObj.is_reset_geo_location
      }
      else {
        this.is_reset_geo_location = false;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeTSlamMetadata
    // Serialize message field [CorresPcTime]
    bufferOffset = _serializer.uint64(obj.CorresPcTime, buffer, bufferOffset);
    // Serialize message field [FrameNumber]
    bufferOffset = _serializer.uint16(obj.FrameNumber, buffer, bufferOffset);
    // Serialize message field [HostHeading]
    bufferOffset = _serializer.float32(obj.HostHeading, buffer, bufferOffset);
    // Serialize message field [HostHeadingUnc]
    bufferOffset = _serializer.float32(obj.HostHeadingUnc, buffer, bufferOffset);
    // Serialize message field [HostVelocity]
    bufferOffset = _serializer.float32(obj.HostVelocity, buffer, bufferOffset);
    // Serialize message field [HostVelocityUnc]
    bufferOffset = _serializer.float32(obj.HostVelocityUnc, buffer, bufferOffset);
    // Serialize message field [HostOmega]
    bufferOffset = _serializer.float32(obj.HostOmega, buffer, bufferOffset);
    // Serialize message field [HostOmegaUnc]
    bufferOffset = _serializer.float32(obj.HostOmegaUnc, buffer, bufferOffset);
    // Serialize message field [NumberOfObjects]
    bufferOffset = _serializer.uint16(obj.NumberOfObjects, buffer, bufferOffset);
    // Serialize message field [local_catr_x]
    bufferOffset = _serializer.float32(obj.local_catr_x, buffer, bufferOffset);
    // Serialize message field [local_catr_y]
    bufferOffset = _serializer.float32(obj.local_catr_y, buffer, bufferOffset);
    // Serialize message field [location_valid]
    bufferOffset = _serializer.bool(obj.location_valid, buffer, bufferOffset);
    // Serialize message field [is_reset_geo_location]
    bufferOffset = _serializer.bool(obj.is_reset_geo_location, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeTSlamMetadata
    let len;
    let data = new arbeTSlamMetadata(null);
    // Deserialize message field [CorresPcTime]
    data.CorresPcTime = _deserializer.uint64(buffer, bufferOffset);
    // Deserialize message field [FrameNumber]
    data.FrameNumber = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [HostHeading]
    data.HostHeading = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [HostHeadingUnc]
    data.HostHeadingUnc = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [HostVelocity]
    data.HostVelocity = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [HostVelocityUnc]
    data.HostVelocityUnc = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [HostOmega]
    data.HostOmega = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [HostOmegaUnc]
    data.HostOmegaUnc = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [NumberOfObjects]
    data.NumberOfObjects = _deserializer.uint16(buffer, bufferOffset);
    // Deserialize message field [local_catr_x]
    data.local_catr_x = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [local_catr_y]
    data.local_catr_y = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [location_valid]
    data.location_valid = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [is_reset_geo_location]
    data.is_reset_geo_location = _deserializer.bool(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 46;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeTSlamMetadata';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '9f419d3fbb87f9bdc08e8e38e6a62de8';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
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
    
    
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new arbeTSlamMetadata(null);
    if (msg.CorresPcTime !== undefined) {
      resolved.CorresPcTime = msg.CorresPcTime;
    }
    else {
      resolved.CorresPcTime = 0
    }

    if (msg.FrameNumber !== undefined) {
      resolved.FrameNumber = msg.FrameNumber;
    }
    else {
      resolved.FrameNumber = 0
    }

    if (msg.HostHeading !== undefined) {
      resolved.HostHeading = msg.HostHeading;
    }
    else {
      resolved.HostHeading = 0.0
    }

    if (msg.HostHeadingUnc !== undefined) {
      resolved.HostHeadingUnc = msg.HostHeadingUnc;
    }
    else {
      resolved.HostHeadingUnc = 0.0
    }

    if (msg.HostVelocity !== undefined) {
      resolved.HostVelocity = msg.HostVelocity;
    }
    else {
      resolved.HostVelocity = 0.0
    }

    if (msg.HostVelocityUnc !== undefined) {
      resolved.HostVelocityUnc = msg.HostVelocityUnc;
    }
    else {
      resolved.HostVelocityUnc = 0.0
    }

    if (msg.HostOmega !== undefined) {
      resolved.HostOmega = msg.HostOmega;
    }
    else {
      resolved.HostOmega = 0.0
    }

    if (msg.HostOmegaUnc !== undefined) {
      resolved.HostOmegaUnc = msg.HostOmegaUnc;
    }
    else {
      resolved.HostOmegaUnc = 0.0
    }

    if (msg.NumberOfObjects !== undefined) {
      resolved.NumberOfObjects = msg.NumberOfObjects;
    }
    else {
      resolved.NumberOfObjects = 0
    }

    if (msg.local_catr_x !== undefined) {
      resolved.local_catr_x = msg.local_catr_x;
    }
    else {
      resolved.local_catr_x = 0.0
    }

    if (msg.local_catr_y !== undefined) {
      resolved.local_catr_y = msg.local_catr_y;
    }
    else {
      resolved.local_catr_y = 0.0
    }

    if (msg.location_valid !== undefined) {
      resolved.location_valid = msg.location_valid;
    }
    else {
      resolved.location_valid = false
    }

    if (msg.is_reset_geo_location !== undefined) {
      resolved.is_reset_geo_location = msg.is_reset_geo_location;
    }
    else {
      resolved.is_reset_geo_location = false
    }

    return resolved;
    }
};

module.exports = arbeTSlamMetadata;
