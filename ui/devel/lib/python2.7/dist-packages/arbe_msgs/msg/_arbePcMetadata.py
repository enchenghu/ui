# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from arbe_msgs/arbePcMetadata.msg. Do not edit."""
import codecs
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct

import arbe_msgs.msg

class arbePcMetadata(genpy.Message):
  _md5sum = "07014ddb83398e57a7856835fad2cfca"
  _type = "arbe_msgs/arbePcMetadata"
  _has_header = False  # flag to mark the presence of a Header object
  _full_text = """arbePcRes PcResolution
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


"""
  __slots__ = ['PcResolution','range_offset','range_res_in_db','azimuth_fft_size','elevation_fft_size','azimuth_fft_padding','elevation_fft_padding']
  _slot_types = ['arbe_msgs/arbePcRes','float32','float32','uint32','uint32','uint32','uint32']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       PcResolution,range_offset,range_res_in_db,azimuth_fft_size,elevation_fft_size,azimuth_fft_padding,elevation_fft_padding

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(arbePcMetadata, self).__init__(*args, **kwds)
      # message fields cannot be None, assign default values for those that are
      if self.PcResolution is None:
        self.PcResolution = arbe_msgs.msg.arbePcRes()
      if self.range_offset is None:
        self.range_offset = 0.
      if self.range_res_in_db is None:
        self.range_res_in_db = 0.
      if self.azimuth_fft_size is None:
        self.azimuth_fft_size = 0
      if self.elevation_fft_size is None:
        self.elevation_fft_size = 0
      if self.azimuth_fft_padding is None:
        self.azimuth_fft_padding = 0
      if self.elevation_fft_padding is None:
        self.elevation_fft_padding = 0
    else:
      self.PcResolution = arbe_msgs.msg.arbePcRes()
      self.range_offset = 0.
      self.range_res_in_db = 0.
      self.azimuth_fft_size = 0
      self.elevation_fft_size = 0
      self.azimuth_fft_padding = 0
      self.elevation_fft_padding = 0

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
      buff.write(_get_struct_6f4I().pack(_x.PcResolution.range_resolution, _x.PcResolution.doppler_resolution, _x.PcResolution.azimuth_coefficient, _x.PcResolution.elevation_coefficient, _x.range_offset, _x.range_res_in_db, _x.azimuth_fft_size, _x.elevation_fft_size, _x.azimuth_fft_padding, _x.elevation_fft_padding))
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
      if self.PcResolution is None:
        self.PcResolution = arbe_msgs.msg.arbePcRes()
      end = 0
      _x = self
      start = end
      end += 40
      (_x.PcResolution.range_resolution, _x.PcResolution.doppler_resolution, _x.PcResolution.azimuth_coefficient, _x.PcResolution.elevation_coefficient, _x.range_offset, _x.range_res_in_db, _x.azimuth_fft_size, _x.elevation_fft_size, _x.azimuth_fft_padding, _x.elevation_fft_padding,) = _get_struct_6f4I().unpack(str[start:end])
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
      buff.write(_get_struct_6f4I().pack(_x.PcResolution.range_resolution, _x.PcResolution.doppler_resolution, _x.PcResolution.azimuth_coefficient, _x.PcResolution.elevation_coefficient, _x.range_offset, _x.range_res_in_db, _x.azimuth_fft_size, _x.elevation_fft_size, _x.azimuth_fft_padding, _x.elevation_fft_padding))
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
      if self.PcResolution is None:
        self.PcResolution = arbe_msgs.msg.arbePcRes()
      end = 0
      _x = self
      start = end
      end += 40
      (_x.PcResolution.range_resolution, _x.PcResolution.doppler_resolution, _x.PcResolution.azimuth_coefficient, _x.PcResolution.elevation_coefficient, _x.range_offset, _x.range_res_in_db, _x.azimuth_fft_size, _x.elevation_fft_size, _x.azimuth_fft_padding, _x.elevation_fft_padding,) = _get_struct_6f4I().unpack(str[start:end])
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill

_struct_I = genpy.struct_I
def _get_struct_I():
    global _struct_I
    return _struct_I
_struct_6f4I = None
def _get_struct_6f4I():
    global _struct_6f4I
    if _struct_6f4I is None:
        _struct_6f4I = struct.Struct("<6f4I")
    return _struct_6f4I
