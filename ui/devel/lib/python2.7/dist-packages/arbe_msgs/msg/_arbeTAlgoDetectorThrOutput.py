# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from arbe_msgs/arbeTAlgoDetectorThrOutput.msg. Do not edit."""
import codecs
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct

import arbe_msgs.msg
import std_msgs.msg

class arbeTAlgoDetectorThrOutput(genpy.Message):
  _md5sum = "888d5fc086e6ad00dbaf413f9befa62b"
  _type = "arbe_msgs/arbeTAlgoDetectorThrOutput"
  _has_header = False  # flag to mark the presence of a Header object
  _full_text = """std_msgs/Header RosHeader
arbeTAlgoDetectorThrInfo tinfo

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
MSG: arbe_msgs/arbeTAlgoDetectorThrInfo
uint32 unFrameNumber
uint8 eAlgoOutput
uint8 eThresholdType
uint8 eFrameType
uint32[] Data
"""
  __slots__ = ['RosHeader','tinfo']
  _slot_types = ['std_msgs/Header','arbe_msgs/arbeTAlgoDetectorThrInfo']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       RosHeader,tinfo

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(arbeTAlgoDetectorThrOutput, self).__init__(*args, **kwds)
      # message fields cannot be None, assign default values for those that are
      if self.RosHeader is None:
        self.RosHeader = std_msgs.msg.Header()
      if self.tinfo is None:
        self.tinfo = arbe_msgs.msg.arbeTAlgoDetectorThrInfo()
    else:
      self.RosHeader = std_msgs.msg.Header()
      self.tinfo = arbe_msgs.msg.arbeTAlgoDetectorThrInfo()

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
      buff.write(_get_struct_3I().pack(_x.RosHeader.seq, _x.RosHeader.stamp.secs, _x.RosHeader.stamp.nsecs))
      _x = self.RosHeader.frame_id
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_I3B().pack(_x.tinfo.unFrameNumber, _x.tinfo.eAlgoOutput, _x.tinfo.eThresholdType, _x.tinfo.eFrameType))
      length = len(self.tinfo.Data)
      buff.write(_struct_I.pack(length))
      pattern = '<%sI'%length
      buff.write(struct.Struct(pattern).pack(*self.tinfo.Data))
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
      if self.RosHeader is None:
        self.RosHeader = std_msgs.msg.Header()
      if self.tinfo is None:
        self.tinfo = arbe_msgs.msg.arbeTAlgoDetectorThrInfo()
      end = 0
      _x = self
      start = end
      end += 12
      (_x.RosHeader.seq, _x.RosHeader.stamp.secs, _x.RosHeader.stamp.nsecs,) = _get_struct_3I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.RosHeader.frame_id = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.RosHeader.frame_id = str[start:end]
      _x = self
      start = end
      end += 7
      (_x.tinfo.unFrameNumber, _x.tinfo.eAlgoOutput, _x.tinfo.eThresholdType, _x.tinfo.eFrameType,) = _get_struct_I3B().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sI'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.tinfo.Data = s.unpack(str[start:end])
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
      buff.write(_get_struct_3I().pack(_x.RosHeader.seq, _x.RosHeader.stamp.secs, _x.RosHeader.stamp.nsecs))
      _x = self.RosHeader.frame_id
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      _x = self
      buff.write(_get_struct_I3B().pack(_x.tinfo.unFrameNumber, _x.tinfo.eAlgoOutput, _x.tinfo.eThresholdType, _x.tinfo.eFrameType))
      length = len(self.tinfo.Data)
      buff.write(_struct_I.pack(length))
      pattern = '<%sI'%length
      buff.write(self.tinfo.Data.tostring())
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
      if self.RosHeader is None:
        self.RosHeader = std_msgs.msg.Header()
      if self.tinfo is None:
        self.tinfo = arbe_msgs.msg.arbeTAlgoDetectorThrInfo()
      end = 0
      _x = self
      start = end
      end += 12
      (_x.RosHeader.seq, _x.RosHeader.stamp.secs, _x.RosHeader.stamp.nsecs,) = _get_struct_3I().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.RosHeader.frame_id = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.RosHeader.frame_id = str[start:end]
      _x = self
      start = end
      end += 7
      (_x.tinfo.unFrameNumber, _x.tinfo.eAlgoOutput, _x.tinfo.eThresholdType, _x.tinfo.eFrameType,) = _get_struct_I3B().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sI'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.tinfo.Data = numpy.frombuffer(str[start:end], dtype=numpy.uint32, count=length)
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
_struct_I3B = None
def _get_struct_I3B():
    global _struct_I3B
    if _struct_I3B is None:
        _struct_I3B = struct.Struct("<I3B")
    return _struct_I3B
