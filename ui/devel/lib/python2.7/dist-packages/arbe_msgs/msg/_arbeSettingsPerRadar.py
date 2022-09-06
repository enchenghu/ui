# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from arbe_msgs/arbeSettingsPerRadar.msg. Do not edit."""
import codecs
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct


class arbeSettingsPerRadar(genpy.Message):
  _md5sum = "0ee305f5b1c77996631bbad5b0e5663d"
  _type = "arbe_msgs/arbeSettingsPerRadar"
  _has_header = False  # flag to mark the presence of a Header object
  _full_text = """uint8 n_radars
int8  radar_for_text
float32[] ant_height
float32[] ant_pitch
float32[] cam_euler_a
float32[] cam_euler_b
float32[] cam_euler_g
"""
  __slots__ = ['n_radars','radar_for_text','ant_height','ant_pitch','cam_euler_a','cam_euler_b','cam_euler_g']
  _slot_types = ['uint8','int8','float32[]','float32[]','float32[]','float32[]','float32[]']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       n_radars,radar_for_text,ant_height,ant_pitch,cam_euler_a,cam_euler_b,cam_euler_g

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(arbeSettingsPerRadar, self).__init__(*args, **kwds)
      # message fields cannot be None, assign default values for those that are
      if self.n_radars is None:
        self.n_radars = 0
      if self.radar_for_text is None:
        self.radar_for_text = 0
      if self.ant_height is None:
        self.ant_height = []
      if self.ant_pitch is None:
        self.ant_pitch = []
      if self.cam_euler_a is None:
        self.cam_euler_a = []
      if self.cam_euler_b is None:
        self.cam_euler_b = []
      if self.cam_euler_g is None:
        self.cam_euler_g = []
    else:
      self.n_radars = 0
      self.radar_for_text = 0
      self.ant_height = []
      self.ant_pitch = []
      self.cam_euler_a = []
      self.cam_euler_b = []
      self.cam_euler_g = []

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
      buff.write(_get_struct_Bb().pack(_x.n_radars, _x.radar_for_text))
      length = len(self.ant_height)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.ant_height))
      length = len(self.ant_pitch)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.ant_pitch))
      length = len(self.cam_euler_a)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.cam_euler_a))
      length = len(self.cam_euler_b)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.cam_euler_b))
      length = len(self.cam_euler_g)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.cam_euler_g))
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
      end = 0
      _x = self
      start = end
      end += 2
      (_x.n_radars, _x.radar_for_text,) = _get_struct_Bb().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.ant_height = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.ant_pitch = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.cam_euler_a = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.cam_euler_b = s.unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.cam_euler_g = s.unpack(str[start:end])
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
      buff.write(_get_struct_Bb().pack(_x.n_radars, _x.radar_for_text))
      length = len(self.ant_height)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.ant_height.tostring())
      length = len(self.ant_pitch)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.ant_pitch.tostring())
      length = len(self.cam_euler_a)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.cam_euler_a.tostring())
      length = len(self.cam_euler_b)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.cam_euler_b.tostring())
      length = len(self.cam_euler_g)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.cam_euler_g.tostring())
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
      end = 0
      _x = self
      start = end
      end += 2
      (_x.n_radars, _x.radar_for_text,) = _get_struct_Bb().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.ant_height = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.ant_pitch = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.cam_euler_a = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.cam_euler_b = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.cam_euler_g = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill

_struct_I = genpy.struct_I
def _get_struct_I():
    global _struct_I
    return _struct_I
_struct_Bb = None
def _get_struct_Bb():
    global _struct_Bb
    if _struct_Bb is None:
        _struct_Bb = struct.Struct("<Bb")
    return _struct_Bb
