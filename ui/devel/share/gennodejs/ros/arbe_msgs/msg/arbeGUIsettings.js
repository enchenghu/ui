// Auto-generated. Do not edit!

// (in-package arbe_msgs.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;
let arbeSlamDisplaySettings = require('./arbeSlamDisplaySettings.js');
let arbeSettingsPerRadar = require('./arbeSettingsPerRadar.js');
let std_msgs = _finder('std_msgs');

//-----------------------------------------------------------

class arbeGUIsettings {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.header = null;
      this.mode = null;
      this.range = null;
      this.threshold3d = null;
      this.threshold4d = null;
      this.DynamicAzimuth = null;
      this.DynamicElevation = null;
      this.cam_euler_alpha = null;
      this.cam_euler_beta = null;
      this.cam_euler_gamma = null;
      this.isNtc3dOn = null;
      this.isNtc4dOn = null;
      this.isCfar3dOn = null;
      this.isCfar4dOn = null;
      this.isPhaseEnabled = null;
      this.isMetaDataEnabled = null;
      this.ColoringType = null;
      this.MinDoppler = null;
      this.MaxDoppler = null;
      this.showFloatingText = null;
      this.coloring_cc_min = null;
      this.coloring_cc_max = null;
      this.discardOutOfElContext = null;
      this.disp_processed_pc = null;
      this.disp_slam = null;
      this.noise_level_db = null;
      this.selectedAzimuthBin = null;
      this.tx_started = null;
      this.radar_id = null;
      this.radar_x_offset = null;
      this.radar_y_offset = null;
      this.radar_z_offset = null;
      this.radar_yaw_angle = null;
      this.radar_pitch_angle = null;
      this.localization_active = null;
      this.aggregation_active = null;
      this.slam_display = null;
      this.per_radar = null;
      this.color_obj_by_class = null;
      this.marker_text_size = null;
    }
    else {
      if (initObj.hasOwnProperty('header')) {
        this.header = initObj.header
      }
      else {
        this.header = new std_msgs.msg.Header();
      }
      if (initObj.hasOwnProperty('mode')) {
        this.mode = initObj.mode
      }
      else {
        this.mode = 0;
      }
      if (initObj.hasOwnProperty('range')) {
        this.range = initObj.range
      }
      else {
        this.range = 0;
      }
      if (initObj.hasOwnProperty('threshold3d')) {
        this.threshold3d = initObj.threshold3d
      }
      else {
        this.threshold3d = 0;
      }
      if (initObj.hasOwnProperty('threshold4d')) {
        this.threshold4d = initObj.threshold4d
      }
      else {
        this.threshold4d = 0;
      }
      if (initObj.hasOwnProperty('DynamicAzimuth')) {
        this.DynamicAzimuth = initObj.DynamicAzimuth
      }
      else {
        this.DynamicAzimuth = 0;
      }
      if (initObj.hasOwnProperty('DynamicElevation')) {
        this.DynamicElevation = initObj.DynamicElevation
      }
      else {
        this.DynamicElevation = 0;
      }
      if (initObj.hasOwnProperty('cam_euler_alpha')) {
        this.cam_euler_alpha = initObj.cam_euler_alpha
      }
      else {
        this.cam_euler_alpha = 0.0;
      }
      if (initObj.hasOwnProperty('cam_euler_beta')) {
        this.cam_euler_beta = initObj.cam_euler_beta
      }
      else {
        this.cam_euler_beta = 0.0;
      }
      if (initObj.hasOwnProperty('cam_euler_gamma')) {
        this.cam_euler_gamma = initObj.cam_euler_gamma
      }
      else {
        this.cam_euler_gamma = 0.0;
      }
      if (initObj.hasOwnProperty('isNtc3dOn')) {
        this.isNtc3dOn = initObj.isNtc3dOn
      }
      else {
        this.isNtc3dOn = false;
      }
      if (initObj.hasOwnProperty('isNtc4dOn')) {
        this.isNtc4dOn = initObj.isNtc4dOn
      }
      else {
        this.isNtc4dOn = false;
      }
      if (initObj.hasOwnProperty('isCfar3dOn')) {
        this.isCfar3dOn = initObj.isCfar3dOn
      }
      else {
        this.isCfar3dOn = false;
      }
      if (initObj.hasOwnProperty('isCfar4dOn')) {
        this.isCfar4dOn = initObj.isCfar4dOn
      }
      else {
        this.isCfar4dOn = false;
      }
      if (initObj.hasOwnProperty('isPhaseEnabled')) {
        this.isPhaseEnabled = initObj.isPhaseEnabled
      }
      else {
        this.isPhaseEnabled = false;
      }
      if (initObj.hasOwnProperty('isMetaDataEnabled')) {
        this.isMetaDataEnabled = initObj.isMetaDataEnabled
      }
      else {
        this.isMetaDataEnabled = false;
      }
      if (initObj.hasOwnProperty('ColoringType')) {
        this.ColoringType = initObj.ColoringType
      }
      else {
        this.ColoringType = '';
      }
      if (initObj.hasOwnProperty('MinDoppler')) {
        this.MinDoppler = initObj.MinDoppler
      }
      else {
        this.MinDoppler = 0.0;
      }
      if (initObj.hasOwnProperty('MaxDoppler')) {
        this.MaxDoppler = initObj.MaxDoppler
      }
      else {
        this.MaxDoppler = 0.0;
      }
      if (initObj.hasOwnProperty('showFloatingText')) {
        this.showFloatingText = initObj.showFloatingText
      }
      else {
        this.showFloatingText = false;
      }
      if (initObj.hasOwnProperty('coloring_cc_min')) {
        this.coloring_cc_min = initObj.coloring_cc_min
      }
      else {
        this.coloring_cc_min = 0.0;
      }
      if (initObj.hasOwnProperty('coloring_cc_max')) {
        this.coloring_cc_max = initObj.coloring_cc_max
      }
      else {
        this.coloring_cc_max = 0.0;
      }
      if (initObj.hasOwnProperty('discardOutOfElContext')) {
        this.discardOutOfElContext = initObj.discardOutOfElContext
      }
      else {
        this.discardOutOfElContext = false;
      }
      if (initObj.hasOwnProperty('disp_processed_pc')) {
        this.disp_processed_pc = initObj.disp_processed_pc
      }
      else {
        this.disp_processed_pc = false;
      }
      if (initObj.hasOwnProperty('disp_slam')) {
        this.disp_slam = initObj.disp_slam
      }
      else {
        this.disp_slam = false;
      }
      if (initObj.hasOwnProperty('noise_level_db')) {
        this.noise_level_db = initObj.noise_level_db
      }
      else {
        this.noise_level_db = 0;
      }
      if (initObj.hasOwnProperty('selectedAzimuthBin')) {
        this.selectedAzimuthBin = initObj.selectedAzimuthBin
      }
      else {
        this.selectedAzimuthBin = 0;
      }
      if (initObj.hasOwnProperty('tx_started')) {
        this.tx_started = initObj.tx_started
      }
      else {
        this.tx_started = false;
      }
      if (initObj.hasOwnProperty('radar_id')) {
        this.radar_id = initObj.radar_id
      }
      else {
        this.radar_id = 0;
      }
      if (initObj.hasOwnProperty('radar_x_offset')) {
        this.radar_x_offset = initObj.radar_x_offset
      }
      else {
        this.radar_x_offset = 0.0;
      }
      if (initObj.hasOwnProperty('radar_y_offset')) {
        this.radar_y_offset = initObj.radar_y_offset
      }
      else {
        this.radar_y_offset = 0.0;
      }
      if (initObj.hasOwnProperty('radar_z_offset')) {
        this.radar_z_offset = initObj.radar_z_offset
      }
      else {
        this.radar_z_offset = 0.0;
      }
      if (initObj.hasOwnProperty('radar_yaw_angle')) {
        this.radar_yaw_angle = initObj.radar_yaw_angle
      }
      else {
        this.radar_yaw_angle = 0.0;
      }
      if (initObj.hasOwnProperty('radar_pitch_angle')) {
        this.radar_pitch_angle = initObj.radar_pitch_angle
      }
      else {
        this.radar_pitch_angle = 0.0;
      }
      if (initObj.hasOwnProperty('localization_active')) {
        this.localization_active = initObj.localization_active
      }
      else {
        this.localization_active = false;
      }
      if (initObj.hasOwnProperty('aggregation_active')) {
        this.aggregation_active = initObj.aggregation_active
      }
      else {
        this.aggregation_active = false;
      }
      if (initObj.hasOwnProperty('slam_display')) {
        this.slam_display = initObj.slam_display
      }
      else {
        this.slam_display = new arbeSlamDisplaySettings();
      }
      if (initObj.hasOwnProperty('per_radar')) {
        this.per_radar = initObj.per_radar
      }
      else {
        this.per_radar = new arbeSettingsPerRadar();
      }
      if (initObj.hasOwnProperty('color_obj_by_class')) {
        this.color_obj_by_class = initObj.color_obj_by_class
      }
      else {
        this.color_obj_by_class = false;
      }
      if (initObj.hasOwnProperty('marker_text_size')) {
        this.marker_text_size = initObj.marker_text_size
      }
      else {
        this.marker_text_size = 0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type arbeGUIsettings
    // Serialize message field [header]
    bufferOffset = std_msgs.msg.Header.serialize(obj.header, buffer, bufferOffset);
    // Serialize message field [mode]
    bufferOffset = _serializer.int32(obj.mode, buffer, bufferOffset);
    // Serialize message field [range]
    bufferOffset = _serializer.int32(obj.range, buffer, bufferOffset);
    // Serialize message field [threshold3d]
    bufferOffset = _serializer.int32(obj.threshold3d, buffer, bufferOffset);
    // Serialize message field [threshold4d]
    bufferOffset = _serializer.int32(obj.threshold4d, buffer, bufferOffset);
    // Serialize message field [DynamicAzimuth]
    bufferOffset = _serializer.int32(obj.DynamicAzimuth, buffer, bufferOffset);
    // Serialize message field [DynamicElevation]
    bufferOffset = _serializer.int32(obj.DynamicElevation, buffer, bufferOffset);
    // Serialize message field [cam_euler_alpha]
    bufferOffset = _serializer.float32(obj.cam_euler_alpha, buffer, bufferOffset);
    // Serialize message field [cam_euler_beta]
    bufferOffset = _serializer.float32(obj.cam_euler_beta, buffer, bufferOffset);
    // Serialize message field [cam_euler_gamma]
    bufferOffset = _serializer.float32(obj.cam_euler_gamma, buffer, bufferOffset);
    // Serialize message field [isNtc3dOn]
    bufferOffset = _serializer.bool(obj.isNtc3dOn, buffer, bufferOffset);
    // Serialize message field [isNtc4dOn]
    bufferOffset = _serializer.bool(obj.isNtc4dOn, buffer, bufferOffset);
    // Serialize message field [isCfar3dOn]
    bufferOffset = _serializer.bool(obj.isCfar3dOn, buffer, bufferOffset);
    // Serialize message field [isCfar4dOn]
    bufferOffset = _serializer.bool(obj.isCfar4dOn, buffer, bufferOffset);
    // Serialize message field [isPhaseEnabled]
    bufferOffset = _serializer.bool(obj.isPhaseEnabled, buffer, bufferOffset);
    // Serialize message field [isMetaDataEnabled]
    bufferOffset = _serializer.bool(obj.isMetaDataEnabled, buffer, bufferOffset);
    // Serialize message field [ColoringType]
    bufferOffset = _serializer.string(obj.ColoringType, buffer, bufferOffset);
    // Serialize message field [MinDoppler]
    bufferOffset = _serializer.float32(obj.MinDoppler, buffer, bufferOffset);
    // Serialize message field [MaxDoppler]
    bufferOffset = _serializer.float32(obj.MaxDoppler, buffer, bufferOffset);
    // Serialize message field [showFloatingText]
    bufferOffset = _serializer.bool(obj.showFloatingText, buffer, bufferOffset);
    // Serialize message field [coloring_cc_min]
    bufferOffset = _serializer.float32(obj.coloring_cc_min, buffer, bufferOffset);
    // Serialize message field [coloring_cc_max]
    bufferOffset = _serializer.float32(obj.coloring_cc_max, buffer, bufferOffset);
    // Serialize message field [discardOutOfElContext]
    bufferOffset = _serializer.bool(obj.discardOutOfElContext, buffer, bufferOffset);
    // Serialize message field [disp_processed_pc]
    bufferOffset = _serializer.bool(obj.disp_processed_pc, buffer, bufferOffset);
    // Serialize message field [disp_slam]
    bufferOffset = _serializer.bool(obj.disp_slam, buffer, bufferOffset);
    // Serialize message field [noise_level_db]
    bufferOffset = _serializer.int8(obj.noise_level_db, buffer, bufferOffset);
    // Serialize message field [selectedAzimuthBin]
    bufferOffset = _serializer.int32(obj.selectedAzimuthBin, buffer, bufferOffset);
    // Serialize message field [tx_started]
    bufferOffset = _serializer.bool(obj.tx_started, buffer, bufferOffset);
    // Serialize message field [radar_id]
    bufferOffset = _serializer.int32(obj.radar_id, buffer, bufferOffset);
    // Serialize message field [radar_x_offset]
    bufferOffset = _serializer.float32(obj.radar_x_offset, buffer, bufferOffset);
    // Serialize message field [radar_y_offset]
    bufferOffset = _serializer.float32(obj.radar_y_offset, buffer, bufferOffset);
    // Serialize message field [radar_z_offset]
    bufferOffset = _serializer.float32(obj.radar_z_offset, buffer, bufferOffset);
    // Serialize message field [radar_yaw_angle]
    bufferOffset = _serializer.float32(obj.radar_yaw_angle, buffer, bufferOffset);
    // Serialize message field [radar_pitch_angle]
    bufferOffset = _serializer.float32(obj.radar_pitch_angle, buffer, bufferOffset);
    // Serialize message field [localization_active]
    bufferOffset = _serializer.bool(obj.localization_active, buffer, bufferOffset);
    // Serialize message field [aggregation_active]
    bufferOffset = _serializer.bool(obj.aggregation_active, buffer, bufferOffset);
    // Serialize message field [slam_display]
    bufferOffset = arbeSlamDisplaySettings.serialize(obj.slam_display, buffer, bufferOffset);
    // Serialize message field [per_radar]
    bufferOffset = arbeSettingsPerRadar.serialize(obj.per_radar, buffer, bufferOffset);
    // Serialize message field [color_obj_by_class]
    bufferOffset = _serializer.bool(obj.color_obj_by_class, buffer, bufferOffset);
    // Serialize message field [marker_text_size]
    bufferOffset = _serializer.int32(obj.marker_text_size, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type arbeGUIsettings
    let len;
    let data = new arbeGUIsettings(null);
    // Deserialize message field [header]
    data.header = std_msgs.msg.Header.deserialize(buffer, bufferOffset);
    // Deserialize message field [mode]
    data.mode = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [range]
    data.range = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [threshold3d]
    data.threshold3d = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [threshold4d]
    data.threshold4d = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [DynamicAzimuth]
    data.DynamicAzimuth = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [DynamicElevation]
    data.DynamicElevation = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [cam_euler_alpha]
    data.cam_euler_alpha = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [cam_euler_beta]
    data.cam_euler_beta = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [cam_euler_gamma]
    data.cam_euler_gamma = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [isNtc3dOn]
    data.isNtc3dOn = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [isNtc4dOn]
    data.isNtc4dOn = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [isCfar3dOn]
    data.isCfar3dOn = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [isCfar4dOn]
    data.isCfar4dOn = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [isPhaseEnabled]
    data.isPhaseEnabled = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [isMetaDataEnabled]
    data.isMetaDataEnabled = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [ColoringType]
    data.ColoringType = _deserializer.string(buffer, bufferOffset);
    // Deserialize message field [MinDoppler]
    data.MinDoppler = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [MaxDoppler]
    data.MaxDoppler = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [showFloatingText]
    data.showFloatingText = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [coloring_cc_min]
    data.coloring_cc_min = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [coloring_cc_max]
    data.coloring_cc_max = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [discardOutOfElContext]
    data.discardOutOfElContext = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [disp_processed_pc]
    data.disp_processed_pc = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [disp_slam]
    data.disp_slam = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [noise_level_db]
    data.noise_level_db = _deserializer.int8(buffer, bufferOffset);
    // Deserialize message field [selectedAzimuthBin]
    data.selectedAzimuthBin = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [tx_started]
    data.tx_started = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [radar_id]
    data.radar_id = _deserializer.int32(buffer, bufferOffset);
    // Deserialize message field [radar_x_offset]
    data.radar_x_offset = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [radar_y_offset]
    data.radar_y_offset = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [radar_z_offset]
    data.radar_z_offset = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [radar_yaw_angle]
    data.radar_yaw_angle = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [radar_pitch_angle]
    data.radar_pitch_angle = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [localization_active]
    data.localization_active = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [aggregation_active]
    data.aggregation_active = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [slam_display]
    data.slam_display = arbeSlamDisplaySettings.deserialize(buffer, bufferOffset);
    // Deserialize message field [per_radar]
    data.per_radar = arbeSettingsPerRadar.deserialize(buffer, bufferOffset);
    // Deserialize message field [color_obj_by_class]
    data.color_obj_by_class = _deserializer.bool(buffer, bufferOffset);
    // Deserialize message field [marker_text_size]
    data.marker_text_size = _deserializer.int32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    let length = 0;
    length += std_msgs.msg.Header.getMessageSize(object.header);
    length += object.ColoringType.length;
    length += arbeSettingsPerRadar.getMessageSize(object.per_radar);
    return length + 110;
  }

  static datatype() {
    // Returns string type for a message object
    return 'arbe_msgs/arbeGUIsettings';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return '0ab680bfc9fa5170d24dd6773926eca5';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    std_msgs/Header header
    int32 mode
    int32 range
    int32 threshold3d
    int32 threshold4d
    int32 DynamicAzimuth
    int32 DynamicElevation
    float32 cam_euler_alpha
    float32 cam_euler_beta
    float32 cam_euler_gamma
    bool  isNtc3dOn
    bool  isNtc4dOn
    bool  isCfar3dOn
    bool  isCfar4dOn
    bool  isPhaseEnabled
    bool  isMetaDataEnabled
    string ColoringType
    float32 MinDoppler
    float32 MaxDoppler
    bool showFloatingText
    float32 coloring_cc_min
    float32 coloring_cc_max
    bool discardOutOfElContext
    bool disp_processed_pc
    bool disp_slam
    int8 noise_level_db
    int32 selectedAzimuthBin
    bool tx_started
    int32 radar_id
    float32 radar_x_offset
    float32 radar_y_offset
    float32 radar_z_offset
    float32 radar_yaw_angle
    float32 radar_pitch_angle
    bool localization_active
    bool aggregation_active
    arbeSlamDisplaySettings slam_display
    arbeSettingsPerRadar per_radar
    bool color_obj_by_class
    int32 marker_text_size
    
    
    
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
    MSG: arbe_msgs/arbeSlamDisplaySettings
    bool slam_on_camera
    bool disp_dist_on_cam
    bool color_by_class
    bool show_classification
    bool disp_funnel
    bool disp_FS_on_cam
    bool disp_slam_eco_mode
    
    ================================================================================
    MSG: arbe_msgs/arbeSettingsPerRadar
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
    const resolved = new arbeGUIsettings(null);
    if (msg.header !== undefined) {
      resolved.header = std_msgs.msg.Header.Resolve(msg.header)
    }
    else {
      resolved.header = new std_msgs.msg.Header()
    }

    if (msg.mode !== undefined) {
      resolved.mode = msg.mode;
    }
    else {
      resolved.mode = 0
    }

    if (msg.range !== undefined) {
      resolved.range = msg.range;
    }
    else {
      resolved.range = 0
    }

    if (msg.threshold3d !== undefined) {
      resolved.threshold3d = msg.threshold3d;
    }
    else {
      resolved.threshold3d = 0
    }

    if (msg.threshold4d !== undefined) {
      resolved.threshold4d = msg.threshold4d;
    }
    else {
      resolved.threshold4d = 0
    }

    if (msg.DynamicAzimuth !== undefined) {
      resolved.DynamicAzimuth = msg.DynamicAzimuth;
    }
    else {
      resolved.DynamicAzimuth = 0
    }

    if (msg.DynamicElevation !== undefined) {
      resolved.DynamicElevation = msg.DynamicElevation;
    }
    else {
      resolved.DynamicElevation = 0
    }

    if (msg.cam_euler_alpha !== undefined) {
      resolved.cam_euler_alpha = msg.cam_euler_alpha;
    }
    else {
      resolved.cam_euler_alpha = 0.0
    }

    if (msg.cam_euler_beta !== undefined) {
      resolved.cam_euler_beta = msg.cam_euler_beta;
    }
    else {
      resolved.cam_euler_beta = 0.0
    }

    if (msg.cam_euler_gamma !== undefined) {
      resolved.cam_euler_gamma = msg.cam_euler_gamma;
    }
    else {
      resolved.cam_euler_gamma = 0.0
    }

    if (msg.isNtc3dOn !== undefined) {
      resolved.isNtc3dOn = msg.isNtc3dOn;
    }
    else {
      resolved.isNtc3dOn = false
    }

    if (msg.isNtc4dOn !== undefined) {
      resolved.isNtc4dOn = msg.isNtc4dOn;
    }
    else {
      resolved.isNtc4dOn = false
    }

    if (msg.isCfar3dOn !== undefined) {
      resolved.isCfar3dOn = msg.isCfar3dOn;
    }
    else {
      resolved.isCfar3dOn = false
    }

    if (msg.isCfar4dOn !== undefined) {
      resolved.isCfar4dOn = msg.isCfar4dOn;
    }
    else {
      resolved.isCfar4dOn = false
    }

    if (msg.isPhaseEnabled !== undefined) {
      resolved.isPhaseEnabled = msg.isPhaseEnabled;
    }
    else {
      resolved.isPhaseEnabled = false
    }

    if (msg.isMetaDataEnabled !== undefined) {
      resolved.isMetaDataEnabled = msg.isMetaDataEnabled;
    }
    else {
      resolved.isMetaDataEnabled = false
    }

    if (msg.ColoringType !== undefined) {
      resolved.ColoringType = msg.ColoringType;
    }
    else {
      resolved.ColoringType = ''
    }

    if (msg.MinDoppler !== undefined) {
      resolved.MinDoppler = msg.MinDoppler;
    }
    else {
      resolved.MinDoppler = 0.0
    }

    if (msg.MaxDoppler !== undefined) {
      resolved.MaxDoppler = msg.MaxDoppler;
    }
    else {
      resolved.MaxDoppler = 0.0
    }

    if (msg.showFloatingText !== undefined) {
      resolved.showFloatingText = msg.showFloatingText;
    }
    else {
      resolved.showFloatingText = false
    }

    if (msg.coloring_cc_min !== undefined) {
      resolved.coloring_cc_min = msg.coloring_cc_min;
    }
    else {
      resolved.coloring_cc_min = 0.0
    }

    if (msg.coloring_cc_max !== undefined) {
      resolved.coloring_cc_max = msg.coloring_cc_max;
    }
    else {
      resolved.coloring_cc_max = 0.0
    }

    if (msg.discardOutOfElContext !== undefined) {
      resolved.discardOutOfElContext = msg.discardOutOfElContext;
    }
    else {
      resolved.discardOutOfElContext = false
    }

    if (msg.disp_processed_pc !== undefined) {
      resolved.disp_processed_pc = msg.disp_processed_pc;
    }
    else {
      resolved.disp_processed_pc = false
    }

    if (msg.disp_slam !== undefined) {
      resolved.disp_slam = msg.disp_slam;
    }
    else {
      resolved.disp_slam = false
    }

    if (msg.noise_level_db !== undefined) {
      resolved.noise_level_db = msg.noise_level_db;
    }
    else {
      resolved.noise_level_db = 0
    }

    if (msg.selectedAzimuthBin !== undefined) {
      resolved.selectedAzimuthBin = msg.selectedAzimuthBin;
    }
    else {
      resolved.selectedAzimuthBin = 0
    }

    if (msg.tx_started !== undefined) {
      resolved.tx_started = msg.tx_started;
    }
    else {
      resolved.tx_started = false
    }

    if (msg.radar_id !== undefined) {
      resolved.radar_id = msg.radar_id;
    }
    else {
      resolved.radar_id = 0
    }

    if (msg.radar_x_offset !== undefined) {
      resolved.radar_x_offset = msg.radar_x_offset;
    }
    else {
      resolved.radar_x_offset = 0.0
    }

    if (msg.radar_y_offset !== undefined) {
      resolved.radar_y_offset = msg.radar_y_offset;
    }
    else {
      resolved.radar_y_offset = 0.0
    }

    if (msg.radar_z_offset !== undefined) {
      resolved.radar_z_offset = msg.radar_z_offset;
    }
    else {
      resolved.radar_z_offset = 0.0
    }

    if (msg.radar_yaw_angle !== undefined) {
      resolved.radar_yaw_angle = msg.radar_yaw_angle;
    }
    else {
      resolved.radar_yaw_angle = 0.0
    }

    if (msg.radar_pitch_angle !== undefined) {
      resolved.radar_pitch_angle = msg.radar_pitch_angle;
    }
    else {
      resolved.radar_pitch_angle = 0.0
    }

    if (msg.localization_active !== undefined) {
      resolved.localization_active = msg.localization_active;
    }
    else {
      resolved.localization_active = false
    }

    if (msg.aggregation_active !== undefined) {
      resolved.aggregation_active = msg.aggregation_active;
    }
    else {
      resolved.aggregation_active = false
    }

    if (msg.slam_display !== undefined) {
      resolved.slam_display = arbeSlamDisplaySettings.Resolve(msg.slam_display)
    }
    else {
      resolved.slam_display = new arbeSlamDisplaySettings()
    }

    if (msg.per_radar !== undefined) {
      resolved.per_radar = arbeSettingsPerRadar.Resolve(msg.per_radar)
    }
    else {
      resolved.per_radar = new arbeSettingsPerRadar()
    }

    if (msg.color_obj_by_class !== undefined) {
      resolved.color_obj_by_class = msg.color_obj_by_class;
    }
    else {
      resolved.color_obj_by_class = false
    }

    if (msg.marker_text_size !== undefined) {
      resolved.marker_text_size = msg.marker_text_size;
    }
    else {
      resolved.marker_text_size = 0
    }

    return resolved;
    }
};

module.exports = arbeGUIsettings;
