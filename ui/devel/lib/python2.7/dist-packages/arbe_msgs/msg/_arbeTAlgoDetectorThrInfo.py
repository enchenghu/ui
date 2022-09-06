# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from arbe_msgs/arbeTAlgoDetectorThrInfo.msg. Do not edit."""
import codecs
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct


class arbeTAlgoDetectorThrInfo(genpy.Message):
  _md5sum = "5f9f71c2214552c0e7f4ddc765c4621d"
  _type = "arbe_msgs/arbeTAlgoDetectorThrInfo"
  _has_header = False  # flag to mark the presence of a Header object
  _full_text = """uint32 unFrameNumber
uint8 eAlgoOutput
uint8 eThresholdType
uint8 eFrameType
uint32[] Data
"""
  __slots__ = ['unFrameNumber','eAlgoOutput','eThresholdType','eFrameType','Data']
  _slot_types = ['uint32','uint8','uint8','uint8','uint32[]']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       unFrameNumber,eAlgoOutput,eThresholdType,eFrameType,Data

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(arbeTAlgoDetectorThrInfo, self).__init__(*args, **kwds)
      # message fields cannot be None, assign default values for those that are
      if self.unFrameNumber is None:
        self.unFrameNumber = 0
      if self.eAlgoOutput is None:
        self.eAlgoOutput = 0
      if self.eThresholdType is None:
        self.eThresholdType = 0
      if self.eFrameType is None:
        self.eFrameType = 0
      if self.Data is None:
        self.Data = []
    else:
      self.unFrameNumber = 0
      self.eAlgoOutput = 0
      self.eThresholdType = 0
      self.eFrameType = 0
      self.Data = []

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
      buff.write(_get_struct_I3B().pack(_x.unFrameNumber, _x.eAlgoOutput, _x.eThresholdType, _x.eFrameType))
      length = len(self.Data)
      buff.write(_struct_I.pack(length))
      pattern = '<%sI'%length
      buff.write(struct.Struct(pattern).pack(*self.Data))
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
      end += 7
      (_x.unFrameNumber, _x.eAlgoOutput, _x.eThresholdType, _x.eFrameType,) = _get_struct_I3B().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sI'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Data = s.unpack(str[start:end])
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
      buff.write(_get_struct_I3B().pack(_x.unFrameNumber, _x.eAlgoOutput, _x.eThresholdType, _x.eFrameType))
      length = len(self.Data)
      buff.write(_struct_I.pack(length))
      pattern = '<%sI'%length
      buff.write(self.Data.tostring())
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
      end += 7
      (_x.unFrameNumber, _x.eAlgoOutput, _x.eThresholdType, _x.eFrameType,) = _get_struct_I3B().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sI'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.Data = numpy.frombuffer(str[start:end], dtype=numpy.uint32, count=length)
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill

_struct_I = genpy.struct_I
def _get_struct_I():
    global _struct_I
    return _struct_I
_struct_I3B = None
def _get_struct_I3B():
    global _struct_I3B
    if _struct_I3B is None:
        _struct_I3B = struct.Struct("<I3B")
    return _struct_I3B
