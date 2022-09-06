# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from arbe_msgs/arbeClassificationTrainingPc.msg. Do not edit."""
import codecs
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct

import arbe_msgs.msg
import std_msgs.msg

class arbeClassificationTrainingPc(genpy.Message):
  _md5sum = "e9b7fe5d9944489810bf1fbe1b2cac94"
  _type = "arbe_msgs/arbeClassificationTrainingPc"
  _has_header = False  # flag to mark the presence of a Header object
  _full_text = """std_msgs/Header Header
arbeNewPcMsg Pc
arbeFusionResponse fusionResponse
uint16[] matching_com_indices

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
MSG: arbe_msgs/arbeNewPcMsg
std_msgs/Header RosHeader
arbePcFrameHeader PcHeader
arbePcMetadata PcMetadata
arbePcBins Points

================================================================================
MSG: arbe_msgs/arbePcFrameHeader
uint64 time
uint16 frame_counter
uint8  is_4d
uint8  frame_type
uint16 udp_lost_packages
uint32 number_of_points
uint16 crd_count
arbeFrameTypes frame_types

================================================================================
MSG: arbe_msgs/arbeFrameTypes
uint8 Frame_Type_3d_short = 1
uint8 Frame_Type_3d_mid = 2
uint8 Frame_Type_3d_long = 3
uint8 Frame_Type_3d_ultra_long = 13
uint8 Frame_Type_4d_short = 4
uint8 Frame_Type_4d_mid = 5
uint8 Frame_Type_4d_long = 6
uint8 Frame_Type_4d_ultra_long = 16

================================================================================
MSG: arbe_msgs/arbePcMetadata
arbePcRes PcResolution
float32 range_offset
float32 range_res_in_db
uint32 azimuth_fft_size
uint32 elevation_fft_size
uint32 azimuth_fft_padding
uint32 elevation_fft_padding


================================================================================
MSG: arbe_msgs/arbePcRes
float32 range_resolution
float32 doppler_resolution
float32 azimuth_coefficient
float32 elevation_coefficient



================================================================================
MSG: arbe_msgs/arbePcBins
uint16[] range_bin
int16[] doppler_signed_bin
int8[] azimuth_signed_bin
int8[] elevation_signed_bin
int16[] phase
int16[] power



================================================================================
MSG: arbe_msgs/arbeFusionResponse
bool valid
uint16 obj_id
uint32 query_id
uint8 fusionClass
uint16 obj_index
uint64 queryTime
float32 conf
uint8 tta_count
float32[] tta_scores
string class_str
arbeFusionClasses fusionClasses

uint16  top
uint16  left
uint16  bottom
uint16  right

================================================================================
MSG: arbe_msgs/arbeFusionClasses
uint16 FC_BIKE = 0
uint16 FC_TRUCK_BUS = 1
uint16 FC_VRU = 2
uint16 FC_CAR = 3
uint16 FC_NOT_CLASSIFIED = 4
uint16 NUM_CLASSES = 4
"""
  __slots__ = ['Header','Pc','fusionResponse','matching_com_indices']
  _slot_types = ['std_msgs/Header','arbe_msgs/arbeNewPcMsg','arbe_msgs/arbeFusionResponse','uint16[]']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       Header,Pc,fusionResponse,matching_com_indices

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(arbeClassificationTrainingPc, self).__init__(*args, **kwds)
      # message fields cannot be None, assign default values for those that are
      if self.Header is None:
        self.Header = std_msgs.msg.Header()
      if self.Pc is None:
        self.Pc = arbe_msgs.msg.arbeNewPcMsg()
      if self.fusionResponse is None:
        self.fusionResponse = arbe_msgs.msg.arbeFusionResponse()
      if self.matching_com_indices is None:
        self.matching_com_indices = []
    else:
      self.Header = std_msgs.msg.Header()
      self.Pc = arbe_msgs.msg.arbeNewPcMsg()
      self.fusionResponse = arbe_msgs.msg.arbeFusionResponse()
      self.matching_com_indices = []

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
      buff.write(_get_struct_3I().pack(_x.Header.seq, _x.Header.stamp.secs, _x.Header.stamp.nsecs))
      _x = self.Header.frame_id
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_3I().pack(_x.Pc.RosHeader.seq, _x.Pc.RosHeader.stamp.secs, _x.Pc.RosHeader.stamp.nsecs))
      _x = self.Pc.RosHeader.frame_id
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_QH2BHIH6f4I().pack(_x.Pc.PcHeader.time, _x.Pc.PcHeader.frame_counter, _x.Pc.PcHeader.is_4d, _x.Pc.PcHeader.frame_type, _x.Pc.PcHeader.udp_lost_packages, _x.Pc.PcHeader.number_of_points, _x.Pc.PcHeader.crd_count, _x.Pc.PcMetadata.PcResolution.range_resolution, _x.Pc.PcMetadata.PcResolution.doppler_resolution, _x.Pc.PcMetadata.PcResolution.azimuth_coefficient, _x.Pc.PcMetadata.PcResolution.elevation_coefficient, _x.Pc.PcMetadata.range_offset, _x.Pc.PcMetadata.range_res_in_db, _x.Pc.PcMetadata.azimuth_fft_size, _x.Pc.PcMetadata.elevation_fft_size, _x.Pc.PcMetadata.azimuth_fft_padding, _x.Pc.PcMetadata.elevation_fft_padding))
      length = len(self.Pc.Points.range_bin)
      buff.write(_struct_I.pack(length))
      pattern = '<%sH'%length
      buff.write(struct.Struct(pattern).pack(*self.Pc.Points.range_bin))
      length = len(self.Pc.Points.doppler_signed_bin)
      buff.write(_struct_I.pack(length))
      pattern = '<%sh'%length
      buff.write(struct.Struct(pattern).pack(*self.Pc.Points.doppler_signed_bin))
      length = len(self.Pc.Points.azimuth_signed_bin)
      buff.write(_struct_I.pack(length))
      pattern = '<%sb'%length
      buff.write(struct.Struct(pattern).pack(*self.Pc.Points.azimuth_signed_bin))
      length = len(self.Pc.Points.elevation_signed_bin)
      buff.write(_struct_I.pack(length))
      pattern = '<%sb'%length
      buff.write(struct.Struct(pattern).pack(*self.Pc.Points.elevation_signed_bin))
      length = len(self.Pc.Points.phase)
      buff.write(_struct_I.pack(length))
      pattern = '<%sh'%length
      buff.write(struct.Struct(pattern).pack(*self.Pc.Points.phase))
      length = len(self.Pc.Points.power)
      buff.write(_struct_I.pack(length))
      pattern = '<%sh'%length
      buff.write(struct.Struct(pattern).pack(*self.Pc.Points.power))
      _x = self
      buff.write(_get_struct_BHIBHQfB().pack(_x.fusionResponse.valid, _x.fusionResponse.obj_id, _x.fusionResponse.query_id, _x.fusionResponse.fusionClass, _x.fusionResponse.obj_index, _x.fusionResponse.queryTime, _x.fusionResponse.conf, _x.fusionResponse.tta_count))
      length = len(self.fusionResponse.tta_scores)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.fusionResponse.tta_scores))
      _x = self.fusionResponse.class_str
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_4H().pack(_x.fusionResponse.top, _x.fusionResponse.left, _x.fusionResponse.bottom, _x.fusionResponse.right))
      length = len(self.matching_com_indices)
      buff.write(_struct_I.pack(length))
      pattern = '<%sH'%length
      buff.write(struct.Struct(pattern).pack(*self.matching_com_indices))
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
      if self.Header is None:
        self.Header = std_msgs.msg.Header()
      if self.Pc is None:
        self.Pc = arbe_msgs.msg.arbeNewPcMsg()
      if self.fusionResponse is None:
        self.fusionResponse = arbe_msgs.msg.arbeFusionResponse()
      end = 0
      _x = self
      start = end
      end += 12
      (_x.Header.seq, _x.Header.stamp.secs, _x.Header.stamp.nsecs,) = _get_struct_3I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.Header.frame_id = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.Header.frame_id = str[start:end]
      _x = self
      start = end
      end += 12
      (_x.Pc.RosHeader.seq, _x.Pc.RosHeader.stamp.secs, _x.Pc.RosHeader.stamp.nsecs,) = _get_struct_3I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.Pc.RosHeader.frame_id = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.Pc.RosHeader.frame_id = str[start:end]
      _x = self
      start = end
      end += 60
      (_x.Pc.PcHeader.time, _x.Pc.PcHeader.frame_counter, _x.Pc.PcHeader.is_4d, _x.Pc.PcHeader.frame_type, _x.Pc.PcHeader.udp_lost_packages, _x.Pc.PcHeader.number_of_points, _x.Pc.PcHeader.crd_count, _x.Pc.PcMetadata.PcResolution.range_resolution, _x.Pc.PcMetadata.PcResolution.doppler_resolution, _x.Pc.PcMetadata.PcResolution.azimuth_coefficient, _x.Pc.PcMetadata.PcResolution.elevation_coefficient, _x.Pc.PcMetadata.range_offset, _x.Pc.PcMetadata.range_res_in_db, _x.Pc.PcMetadata.azimuth_fft_size, _x.Pc.PcMetadata.elevation_fft_size, _x.Pc.PcMetadata.azimuth_fft_padding, _x.Pc.PcMetadata.elevation_fft_padding,) = _get_struct_QH2BHIH6f4I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sH'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.range_bin = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sh'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.doppler_signed_bin = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sb'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.azimuth_signed_bin = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sb'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.elevation_signed_bin = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sh'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.phase = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sh'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.power = s.unpack(str[start:end])
      _x = self
      start = end
      end += 23
      (_x.fusionResponse.valid, _x.fusionResponse.obj_id, _x.fusionResponse.query_id, _x.fusionResponse.fusionClass, _x.fusionResponse.obj_index, _x.fusionResponse.queryTime, _x.fusionResponse.conf, _x.fusionResponse.tta_count,) = _get_struct_BHIBHQfB().unpack(str[start:end])
      self.fusionResponse.valid = bool(self.fusionResponse.valid)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.fusionResponse.tta_scores = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.fusionResponse.class_str = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.fusionResponse.class_str = str[start:end]
      _x = self
      start = end
      end += 8
      (_x.fusionResponse.top, _x.fusionResponse.left, _x.fusionResponse.bottom, _x.fusionResponse.right,) = _get_struct_4H().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sH'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.matching_com_indices = s.unpack(str[start:end])
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
      buff.write(_get_struct_3I().pack(_x.Header.seq, _x.Header.stamp.secs, _x.Header.stamp.nsecs))
      _x = self.Header.frame_id
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_3I().pack(_x.Pc.RosHeader.seq, _x.Pc.RosHeader.stamp.secs, _x.Pc.RosHeader.stamp.nsecs))
      _x = self.Pc.RosHeader.frame_id
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_QH2BHIH6f4I().pack(_x.Pc.PcHeader.time, _x.Pc.PcHeader.frame_counter, _x.Pc.PcHeader.is_4d, _x.Pc.PcHeader.frame_type, _x.Pc.PcHeader.udp_lost_packages, _x.Pc.PcHeader.number_of_points, _x.Pc.PcHeader.crd_count, _x.Pc.PcMetadata.PcResolution.range_resolution, _x.Pc.PcMetadata.PcResolution.doppler_resolution, _x.Pc.PcMetadata.PcResolution.azimuth_coefficient, _x.Pc.PcMetadata.PcResolution.elevation_coefficient, _x.Pc.PcMetadata.range_offset, _x.Pc.PcMetadata.range_res_in_db, _x.Pc.PcMetadata.azimuth_fft_size, _x.Pc.PcMetadata.elevation_fft_size, _x.Pc.PcMetadata.azimuth_fft_padding, _x.Pc.PcMetadata.elevation_fft_padding))
      length = len(self.Pc.Points.range_bin)
      buff.write(_struct_I.pack(length))
      pattern = '<%sH'%length
      buff.write(self.Pc.Points.range_bin.tostring())
      length = len(self.Pc.Points.doppler_signed_bin)
      buff.write(_struct_I.pack(length))
      pattern = '<%sh'%length
      buff.write(self.Pc.Points.doppler_signed_bin.tostring())
      length = len(self.Pc.Points.azimuth_signed_bin)
      buff.write(_struct_I.pack(length))
      pattern = '<%sb'%length
      buff.write(self.Pc.Points.azimuth_signed_bin.tostring())
      length = len(self.Pc.Points.elevation_signed_bin)
      buff.write(_struct_I.pack(length))
      pattern = '<%sb'%length
      buff.write(self.Pc.Points.elevation_signed_bin.tostring())
      length = len(self.Pc.Points.phase)
      buff.write(_struct_I.pack(length))
      pattern = '<%sh'%length
      buff.write(self.Pc.Points.phase.tostring())
      length = len(self.Pc.Points.power)
      buff.write(_struct_I.pack(length))
      pattern = '<%sh'%length
      buff.write(self.Pc.Points.power.tostring())
      _x = self
      buff.write(_get_struct_BHIBHQfB().pack(_x.fusionResponse.valid, _x.fusionResponse.obj_id, _x.fusionResponse.query_id, _x.fusionResponse.fusionClass, _x.fusionResponse.obj_index, _x.fusionResponse.queryTime, _x.fusionResponse.conf, _x.fusionResponse.tta_count))
      length = len(self.fusionResponse.tta_scores)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.fusionResponse.tta_scores.tostring())
      _x = self.fusionResponse.class_str
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_4H().pack(_x.fusionResponse.top, _x.fusionResponse.left, _x.fusionResponse.bottom, _x.fusionResponse.right))
      length = len(self.matching_com_indices)
      buff.write(_struct_I.pack(length))
      pattern = '<%sH'%length
      buff.write(self.matching_com_indices.tostring())
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
      if self.Header is None:
        self.Header = std_msgs.msg.Header()
      if self.Pc is None:
        self.Pc = arbe_msgs.msg.arbeNewPcMsg()
      if self.fusionResponse is None:
        self.fusionResponse = arbe_msgs.msg.arbeFusionResponse()
      end = 0
      _x = self
      start = end
      end += 12
      (_x.Header.seq, _x.Header.stamp.secs, _x.Header.stamp.nsecs,) = _get_struct_3I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.Header.frame_id = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.Header.frame_id = str[start:end]
      _x = self
      start = end
      end += 12
      (_x.Pc.RosHeader.seq, _x.Pc.RosHeader.stamp.secs, _x.Pc.RosHeader.stamp.nsecs,) = _get_struct_3I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.Pc.RosHeader.frame_id = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.Pc.RosHeader.frame_id = str[start:end]
      _x = self
      start = end
      end += 60
      (_x.Pc.PcHeader.time, _x.Pc.PcHeader.frame_counter, _x.Pc.PcHeader.is_4d, _x.Pc.PcHeader.frame_type, _x.Pc.PcHeader.udp_lost_packages, _x.Pc.PcHeader.number_of_points, _x.Pc.PcHeader.crd_count, _x.Pc.PcMetadata.PcResolution.range_resolution, _x.Pc.PcMetadata.PcResolution.doppler_resolution, _x.Pc.PcMetadata.PcResolution.azimuth_coefficient, _x.Pc.PcMetadata.PcResolution.elevation_coefficient, _x.Pc.PcMetadata.range_offset, _x.Pc.PcMetadata.range_res_in_db, _x.Pc.PcMetadata.azimuth_fft_size, _x.Pc.PcMetadata.elevation_fft_size, _x.Pc.PcMetadata.azimuth_fft_padding, _x.Pc.PcMetadata.elevation_fft_padding,) = _get_struct_QH2BHIH6f4I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sH'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.range_bin = numpy.frombuffer(str[start:end], dtype=numpy.uint16, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sh'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.doppler_signed_bin = numpy.frombuffer(str[start:end], dtype=numpy.int16, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sb'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.azimuth_signed_bin = numpy.frombuffer(str[start:end], dtype=numpy.int8, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sb'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.elevation_signed_bin = numpy.frombuffer(str[start:end], dtype=numpy.int8, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sh'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.phase = numpy.frombuffer(str[start:end], dtype=numpy.int16, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sh'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Pc.Points.power = numpy.frombuffer(str[start:end], dtype=numpy.int16, count=length)
      _x = self
      start = end
      end += 23
      (_x.fusionResponse.valid, _x.fusionResponse.obj_id, _x.fusionResponse.query_id, _x.fusionResponse.fusionClass, _x.fusionResponse.obj_index, _x.fusionResponse.queryTime, _x.fusionResponse.conf, _x.fusionResponse.tta_count,) = _get_struct_BHIBHQfB().unpack(str[start:end])
      self.fusionResponse.valid = bool(self.fusionResponse.valid)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.fusionResponse.tta_scores = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.fusionResponse.class_str = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.fusionResponse.class_str = str[start:end]
      _x = self
      start = end
      end += 8
      (_x.fusionResponse.top, _x.fusionResponse.left, _x.fusionResponse.bottom, _x.fusionResponse.right,) = _get_struct_4H().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sH'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.matching_com_indices = numpy.frombuffer(str[start:end], dtype=numpy.uint16, count=length)
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill

_struct_I = genpy.struct_I
def _get_struct_I():
    global _struct_I
    return _struct_I
_struct_3I = None
def _get_struct_3I():
    global _struct_3I
    if _struct_3I is None:
        _struct_3I = struct.Struct("<3I")
    return _struct_3I
_struct_4H = None
def _get_struct_4H():
    global _struct_4H
    if _struct_4H is None:
        _struct_4H = struct.Struct("<4H")
    return _struct_4H
_struct_BHIBHQfB = None
def _get_struct_BHIBHQfB():
    global _struct_BHIBHQfB
    if _struct_BHIBHQfB is None:
        _struct_BHIBHQfB = struct.Struct("<BHIBHQfB")
    return _struct_BHIBHQfB
_struct_QH2BHIH6f4I = None
def _get_struct_QH2BHIH6f4I():
    global _struct_QH2BHIH6f4I
    if _struct_QH2BHIH6f4I is None:
        _struct_QH2BHIH6f4I = struct.Struct("<QH2BHIH6f4I")
    return _struct_QH2BHIH6f4I
