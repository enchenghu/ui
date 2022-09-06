# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from arbe_msgs/arbeGUIsettings.msg. Do not edit."""
import codecs
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct

import arbe_msgs.msg
import std_msgs.msg

class arbeGUIsettings(genpy.Message):
  _md5sum = "0ab680bfc9fa5170d24dd6773926eca5"
  _type = "arbe_msgs/arbeGUIsettings"
  _has_header = True  # flag to mark the presence of a Header object
  _full_text = """std_msgs/Header header
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
"""
  __slots__ = ['header','mode','range','threshold3d','threshold4d','DynamicAzimuth','DynamicElevation','cam_euler_alpha','cam_euler_beta','cam_euler_gamma','isNtc3dOn','isNtc4dOn','isCfar3dOn','isCfar4dOn','isPhaseEnabled','isMetaDataEnabled','ColoringType','MinDoppler','MaxDoppler','showFloatingText','coloring_cc_min','coloring_cc_max','discardOutOfElContext','disp_processed_pc','disp_slam','noise_level_db','selectedAzimuthBin','tx_started','radar_id','radar_x_offset','radar_y_offset','radar_z_offset','radar_yaw_angle','radar_pitch_angle','localization_active','aggregation_active','slam_display','per_radar','color_obj_by_class','marker_text_size']
  _slot_types = ['std_msgs/Header','int32','int32','int32','int32','int32','int32','float32','float32','float32','bool','bool','bool','bool','bool','bool','string','float32','float32','bool','float32','float32','bool','bool','bool','int8','int32','bool','int32','float32','float32','float32','float32','float32','bool','bool','arbe_msgs/arbeSlamDisplaySettings','arbe_msgs/arbeSettingsPerRadar','bool','int32']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       header,mode,range,threshold3d,threshold4d,DynamicAzimuth,DynamicElevation,cam_euler_alpha,cam_euler_beta,cam_euler_gamma,isNtc3dOn,isNtc4dOn,isCfar3dOn,isCfar4dOn,isPhaseEnabled,isMetaDataEnabled,ColoringType,MinDoppler,MaxDoppler,showFloatingText,coloring_cc_min,coloring_cc_max,discardOutOfElContext,disp_processed_pc,disp_slam,noise_level_db,selectedAzimuthBin,tx_started,radar_id,radar_x_offset,radar_y_offset,radar_z_offset,radar_yaw_angle,radar_pitch_angle,localization_active,aggregation_active,slam_display,per_radar,color_obj_by_class,marker_text_size

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(arbeGUIsettings, self).__init__(*args, **kwds)
      # message fields cannot be None, assign default values for those that are
      if self.header is None:
        self.header = std_msgs.msg.Header()
      if self.mode is None:
        self.mode = 0
      if self.range is None:
        self.range = 0
      if self.threshold3d is None:
        self.threshold3d = 0
      if self.threshold4d is None:
        self.threshold4d = 0
      if self.DynamicAzimuth is None:
        self.DynamicAzimuth = 0
      if self.DynamicElevation is None:
        self.DynamicElevation = 0
      if self.cam_euler_alpha is None:
        self.cam_euler_alpha = 0.
      if self.cam_euler_beta is None:
        self.cam_euler_beta = 0.
      if self.cam_euler_gamma is None:
        self.cam_euler_gamma = 0.
      if self.isNtc3dOn is None:
        self.isNtc3dOn = False
      if self.isNtc4dOn is None:
        self.isNtc4dOn = False
      if self.isCfar3dOn is None:
        self.isCfar3dOn = False
      if self.isCfar4dOn is None:
        self.isCfar4dOn = False
      if self.isPhaseEnabled is None:
        self.isPhaseEnabled = False
      if self.isMetaDataEnabled is None:
        self.isMetaDataEnabled = False
      if self.ColoringType is None:
        self.ColoringType = ''
      if self.MinDoppler is None:
        self.MinDoppler = 0.
      if self.MaxDoppler is None:
        self.MaxDoppler = 0.
      if self.showFloatingText is None:
        self.showFloatingText = False
      if self.coloring_cc_min is None:
        self.coloring_cc_min = 0.
      if self.coloring_cc_max is None:
        self.coloring_cc_max = 0.
      if self.discardOutOfElContext is None:
        self.discardOutOfElContext = False
      if self.disp_processed_pc is None:
        self.disp_processed_pc = False
      if self.disp_slam is None:
        self.disp_slam = False
      if self.noise_level_db is None:
        self.noise_level_db = 0
      if self.selectedAzimuthBin is None:
        self.selectedAzimuthBin = 0
      if self.tx_started is None:
        self.tx_started = False
      if self.radar_id is None:
        self.radar_id = 0
      if self.radar_x_offset is None:
        self.radar_x_offset = 0.
      if self.radar_y_offset is None:
        self.radar_y_offset = 0.
      if self.radar_z_offset is None:
        self.radar_z_offset = 0.
      if self.radar_yaw_angle is None:
        self.radar_yaw_angle = 0.
      if self.radar_pitch_angle is None:
        self.radar_pitch_angle = 0.
      if self.localization_active is None:
        self.localization_active = False
      if self.aggregation_active is None:
        self.aggregation_active = False
      if self.slam_display is None:
        self.slam_display = arbe_msgs.msg.arbeSlamDisplaySettings()
      if self.per_radar is None:
        self.per_radar = arbe_msgs.msg.arbeSettingsPerRadar()
      if self.color_obj_by_class is None:
        self.color_obj_by_class = False
      if self.marker_text_size is None:
        self.marker_text_size = 0
    else:
      self.header = std_msgs.msg.Header()
      self.mode = 0
      self.range = 0
      self.threshold3d = 0
      self.threshold4d = 0
      self.DynamicAzimuth = 0
      self.DynamicElevation = 0
      self.cam_euler_alpha = 0.
      self.cam_euler_beta = 0.
      self.cam_euler_gamma = 0.
      self.isNtc3dOn = False
      self.isNtc4dOn = False
      self.isCfar3dOn = False
      self.isCfar4dOn = False
      self.isPhaseEnabled = False
      self.isMetaDataEnabled = False
      self.ColoringType = ''
      self.MinDoppler = 0.
      self.MaxDoppler = 0.
      self.showFloatingText = False
      self.coloring_cc_min = 0.
      self.coloring_cc_max = 0.
      self.discardOutOfElContext = False
      self.disp_processed_pc = False
      self.disp_slam = False
      self.noise_level_db = 0
      self.selectedAzimuthBin = 0
      self.tx_started = False
      self.radar_id = 0
      self.radar_x_offset = 0.
      self.radar_y_offset = 0.
      self.radar_z_offset = 0.
      self.radar_yaw_angle = 0.
      self.radar_pitch_angle = 0.
      self.localization_active = False
      self.aggregation_active = False
      self.slam_display = arbe_msgs.msg.arbeSlamDisplaySettings()
      self.per_radar = arbe_msgs.msg.arbeSettingsPerRadar()
      self.color_obj_by_class = False
      self.marker_text_size = 0

  def _get_types(self):
    """
    internal API method
    """
    return self._slot_types

  def serialize(self, buff):
    """
    serialize message into buffer
    :param buff: buffer, ``StringIO``
    """
    try:
      _x = self
      buff.write(_get_struct_3I().pack(_x.header.seq, _x.header.stamp.secs, _x.header.stamp.nsecs))
      _x = self.header.frame_id
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_6i3f6B().pack(_x.mode, _x.range, _x.threshold3d, _x.threshold4d, _x.DynamicAzimuth, _x.DynamicElevation, _x.cam_euler_alpha, _x.cam_euler_beta, _x.cam_euler_gamma, _x.isNtc3dOn, _x.isNtc4dOn, _x.isCfar3dOn, _x.isCfar4dOn, _x.isPhaseEnabled, _x.isMetaDataEnabled))
      _x = self.ColoringType
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_2fB2f3BbiBi5f10Bb().pack(_x.MinDoppler, _x.MaxDoppler, _x.showFloatingText, _x.coloring_cc_min, _x.coloring_cc_max, _x.discardOutOfElContext, _x.disp_processed_pc, _x.disp_slam, _x.noise_level_db, _x.selectedAzimuthBin, _x.tx_started, _x.radar_id, _x.radar_x_offset, _x.radar_y_offset, _x.radar_z_offset, _x.radar_yaw_angle, _x.radar_pitch_angle, _x.localization_active, _x.aggregation_active, _x.slam_display.slam_on_camera, _x.slam_display.disp_dist_on_cam, _x.slam_display.color_by_class, _x.slam_display.show_classification, _x.slam_display.disp_funnel, _x.slam_display.disp_FS_on_cam, _x.slam_display.disp_slam_eco_mode, _x.per_radar.n_radars, _x.per_radar.radar_for_text))
      length = len(self.per_radar.ant_height)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.per_radar.ant_height))
      length = len(self.per_radar.ant_pitch)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.per_radar.ant_pitch))
      length = len(self.per_radar.cam_euler_a)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.per_radar.cam_euler_a))
      length = len(self.per_radar.cam_euler_b)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.per_radar.cam_euler_b))
      length = len(self.per_radar.cam_euler_g)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.per_radar.cam_euler_g))
      _x = self
      buff.write(_get_struct_Bi().pack(_x.color_obj_by_class, _x.marker_text_size))
    except struct.error as se: self._check_types(struct.error("%s: '%s' when writing '%s'" % (type(se), str(se), str(locals().get('_x', self)))))
    except TypeError as te: self._check_types(ValueError("%s: '%s' when writing '%s'" % (type(te), str(te), str(locals().get('_x', self)))))

  def deserialize(self, str):
    """
    unpack serialized message in str into this message instance
    :param str: byte array of serialized message, ``str``
    """
    if python3:
      codecs.lookup_error("rosmsg").msg_type = self._type
    try:
      if self.header is None:
        self.header = std_msgs.msg.Header()
      if self.slam_display is None:
        self.slam_display = arbe_msgs.msg.arbeSlamDisplaySettings()
      if self.per_radar is None:
        self.per_radar = arbe_msgs.msg.arbeSettingsPerRadar()
      end = 0
      _x = self
      start = end
      end += 12
      (_x.header.seq, _x.header.stamp.secs, _x.header.stamp.nsecs,) = _get_struct_3I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.header.frame_id = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.header.frame_id = str[start:end]
      _x = self
      start = end
      end += 42
      (_x.mode, _x.range, _x.threshold3d, _x.threshold4d, _x.DynamicAzimuth, _x.DynamicElevation, _x.cam_euler_alpha, _x.cam_euler_beta, _x.cam_euler_gamma, _x.isNtc3dOn, _x.isNtc4dOn, _x.isCfar3dOn, _x.isCfar4dOn, _x.isPhaseEnabled, _x.isMetaDataEnabled,) = _get_struct_6i3f6B().unpack(str[start:end])
      self.isNtc3dOn = bool(self.isNtc3dOn)
      self.isNtc4dOn = bool(self.isNtc4dOn)
      self.isCfar3dOn = bool(self.isCfar3dOn)
      self.isCfar4dOn = bool(self.isCfar4dOn)
      self.isPhaseEnabled = bool(self.isPhaseEnabled)
      self.isMetaDataEnabled = bool(self.isMetaDataEnabled)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.ColoringType = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.ColoringType = str[start:end]
      _x = self
      start = end
      end += 61
      (_x.MinDoppler, _x.MaxDoppler, _x.showFloatingText, _x.coloring_cc_min, _x.coloring_cc_max, _x.discardOutOfElContext, _x.disp_processed_pc, _x.disp_slam, _x.noise_level_db, _x.selectedAzimuthBin, _x.tx_started, _x.radar_id, _x.radar_x_offset, _x.radar_y_offset, _x.radar_z_offset, _x.radar_yaw_angle, _x.radar_pitch_angle, _x.localization_active, _x.aggregation_active, _x.slam_display.slam_on_camera, _x.slam_display.disp_dist_on_cam, _x.slam_display.color_by_class, _x.slam_display.show_classification, _x.slam_display.disp_funnel, _x.slam_display.disp_FS_on_cam, _x.slam_display.disp_slam_eco_mode, _x.per_radar.n_radars, _x.per_radar.radar_for_text,) = _get_struct_2fB2f3BbiBi5f10Bb().unpack(str[start:end])
      self.showFloatingText = bool(self.showFloatingText)
      self.discardOutOfElContext = bool(self.discardOutOfElContext)
      self.disp_processed_pc = bool(self.disp_processed_pc)
      self.disp_slam = bool(self.disp_slam)
      self.tx_started = bool(self.tx_started)
      self.localization_active = bool(self.localization_active)
      self.aggregation_active = bool(self.aggregation_active)
      self.slam_display.slam_on_camera = bool(self.slam_display.slam_on_camera)
      self.slam_display.disp_dist_on_cam = bool(self.slam_display.disp_dist_on_cam)
      self.slam_display.color_by_class = bool(self.slam_display.color_by_class)
      self.slam_display.show_classification = bool(self.slam_display.show_classification)
      self.slam_display.disp_funnel = bool(self.slam_display.disp_funnel)
      self.slam_display.disp_FS_on_cam = bool(self.slam_display.disp_FS_on_cam)
      self.slam_display.disp_slam_eco_mode = bool(self.slam_display.disp_slam_eco_mode)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.ant_height = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.ant_pitch = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.cam_euler_a = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.cam_euler_b = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.cam_euler_g = s.unpack(str[start:end])
      _x = self
      start = end
      end += 5
      (_x.color_obj_by_class, _x.marker_text_size,) = _get_struct_Bi().unpack(str[start:end])
      self.color_obj_by_class = bool(self.color_obj_by_class)
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill


  def serialize_numpy(self, buff, numpy):
    """
    serialize message with numpy array types into buffer
    :param buff: buffer, ``StringIO``
    :param numpy: numpy python module
    """
    try:
      _x = self
      buff.write(_get_struct_3I().pack(_x.header.seq, _x.header.stamp.secs, _x.header.stamp.nsecs))
      _x = self.header.frame_id
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_6i3f6B().pack(_x.mode, _x.range, _x.threshold3d, _x.threshold4d, _x.DynamicAzimuth, _x.DynamicElevation, _x.cam_euler_alpha, _x.cam_euler_beta, _x.cam_euler_gamma, _x.isNtc3dOn, _x.isNtc4dOn, _x.isCfar3dOn, _x.isCfar4dOn, _x.isPhaseEnabled, _x.isMetaDataEnabled))
      _x = self.ColoringType
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_2fB2f3BbiBi5f10Bb().pack(_x.MinDoppler, _x.MaxDoppler, _x.showFloatingText, _x.coloring_cc_min, _x.coloring_cc_max, _x.discardOutOfElContext, _x.disp_processed_pc, _x.disp_slam, _x.noise_level_db, _x.selectedAzimuthBin, _x.tx_started, _x.radar_id, _x.radar_x_offset, _x.radar_y_offset, _x.radar_z_offset, _x.radar_yaw_angle, _x.radar_pitch_angle, _x.localization_active, _x.aggregation_active, _x.slam_display.slam_on_camera, _x.slam_display.disp_dist_on_cam, _x.slam_display.color_by_class, _x.slam_display.show_classification, _x.slam_display.disp_funnel, _x.slam_display.disp_FS_on_cam, _x.slam_display.disp_slam_eco_mode, _x.per_radar.n_radars, _x.per_radar.radar_for_text))
      length = len(self.per_radar.ant_height)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.per_radar.ant_height.tostring())
      length = len(self.per_radar.ant_pitch)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.per_radar.ant_pitch.tostring())
      length = len(self.per_radar.cam_euler_a)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.per_radar.cam_euler_a.tostring())
      length = len(self.per_radar.cam_euler_b)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.per_radar.cam_euler_b.tostring())
      length = len(self.per_radar.cam_euler_g)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.per_radar.cam_euler_g.tostring())
      _x = self
      buff.write(_get_struct_Bi().pack(_x.color_obj_by_class, _x.marker_text_size))
    except struct.error as se: self._check_types(struct.error("%s: '%s' when writing '%s'" % (type(se), str(se), str(locals().get('_x', self)))))
    except TypeError as te: self._check_types(ValueError("%s: '%s' when writing '%s'" % (type(te), str(te), str(locals().get('_x', self)))))

  def deserialize_numpy(self, str, numpy):
    """
    unpack serialized message in str into this message instance using numpy for array types
    :param str: byte array of serialized message, ``str``
    :param numpy: numpy python module
    """
    if python3:
      codecs.lookup_error("rosmsg").msg_type = self._type
    try:
      if self.header is None:
        self.header = std_msgs.msg.Header()
      if self.slam_display is None:
        self.slam_display = arbe_msgs.msg.arbeSlamDisplaySettings()
      if self.per_radar is None:
        self.per_radar = arbe_msgs.msg.arbeSettingsPerRadar()
      end = 0
      _x = self
      start = end
      end += 12
      (_x.header.seq, _x.header.stamp.secs, _x.header.stamp.nsecs,) = _get_struct_3I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.header.frame_id = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.header.frame_id = str[start:end]
      _x = self
      start = end
      end += 42
      (_x.mode, _x.range, _x.threshold3d, _x.threshold4d, _x.DynamicAzimuth, _x.DynamicElevation, _x.cam_euler_alpha, _x.cam_euler_beta, _x.cam_euler_gamma, _x.isNtc3dOn, _x.isNtc4dOn, _x.isCfar3dOn, _x.isCfar4dOn, _x.isPhaseEnabled, _x.isMetaDataEnabled,) = _get_struct_6i3f6B().unpack(str[start:end])
      self.isNtc3dOn = bool(self.isNtc3dOn)
      self.isNtc4dOn = bool(self.isNtc4dOn)
      self.isCfar3dOn = bool(self.isCfar3dOn)
      self.isCfar4dOn = bool(self.isCfar4dOn)
      self.isPhaseEnabled = bool(self.isPhaseEnabled)
      self.isMetaDataEnabled = bool(self.isMetaDataEnabled)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.ColoringType = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.ColoringType = str[start:end]
      _x = self
      start = end
      end += 61
      (_x.MinDoppler, _x.MaxDoppler, _x.showFloatingText, _x.coloring_cc_min, _x.coloring_cc_max, _x.discardOutOfElContext, _x.disp_processed_pc, _x.disp_slam, _x.noise_level_db, _x.selectedAzimuthBin, _x.tx_started, _x.radar_id, _x.radar_x_offset, _x.radar_y_offset, _x.radar_z_offset, _x.radar_yaw_angle, _x.radar_pitch_angle, _x.localization_active, _x.aggregation_active, _x.slam_display.slam_on_camera, _x.slam_display.disp_dist_on_cam, _x.slam_display.color_by_class, _x.slam_display.show_classification, _x.slam_display.disp_funnel, _x.slam_display.disp_FS_on_cam, _x.slam_display.disp_slam_eco_mode, _x.per_radar.n_radars, _x.per_radar.radar_for_text,) = _get_struct_2fB2f3BbiBi5f10Bb().unpack(str[start:end])
      self.showFloatingText = bool(self.showFloatingText)
      self.discardOutOfElContext = bool(self.discardOutOfElContext)
      self.disp_processed_pc = bool(self.disp_processed_pc)
      self.disp_slam = bool(self.disp_slam)
      self.tx_started = bool(self.tx_started)
      self.localization_active = bool(self.localization_active)
      self.aggregation_active = bool(self.aggregation_active)
      self.slam_display.slam_on_camera = bool(self.slam_display.slam_on_camera)
      self.slam_display.disp_dist_on_cam = bool(self.slam_display.disp_dist_on_cam)
      self.slam_display.color_by_class = bool(self.slam_display.color_by_class)
      self.slam_display.show_classification = bool(self.slam_display.show_classification)
      self.slam_display.disp_funnel = bool(self.slam_display.disp_funnel)
      self.slam_display.disp_FS_on_cam = bool(self.slam_display.disp_FS_on_cam)
      self.slam_display.disp_slam_eco_mode = bool(self.slam_display.disp_slam_eco_mode)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.ant_height = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.ant_pitch = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.cam_euler_a = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.cam_euler_b = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.per_radar.cam_euler_g = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      _x = self
      start = end
      end += 5
      (_x.color_obj_by_class, _x.marker_text_size,) = _get_struct_Bi().unpack(str[start:end])
      self.color_obj_by_class = bool(self.color_obj_by_class)
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill

_struct_I = genpy.struct_I
def _get_struct_I():
    global _struct_I
    return _struct_I
_struct_2fB2f3BbiBi5f10Bb = None
def _get_struct_2fB2f3BbiBi5f10Bb():
    global _struct_2fB2f3BbiBi5f10Bb
    if _struct_2fB2f3BbiBi5f10Bb is None:
        _struct_2fB2f3BbiBi5f10Bb = struct.Struct("<2fB2f3BbiBi5f10Bb")
    return _struct_2fB2f3BbiBi5f10Bb
_struct_3I = None
def _get_struct_3I():
    global _struct_3I
    if _struct_3I is None:
        _struct_3I = struct.Struct("<3I")
    return _struct_3I
_struct_6i3f6B = None
def _get_struct_6i3f6B():
    global _struct_6i3f6B
    if _struct_6i3f6B is None:
        _struct_6i3f6B = struct.Struct("<6i3f6B")
    return _struct_6i3f6B
_struct_Bi = None
def _get_struct_Bi():
    global _struct_Bi
    if _struct_Bi is None:
        _struct_Bi = struct.Struct("<Bi")
    return _struct_Bi
