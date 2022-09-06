# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from arbe_msgs/arbeClassSingleResponse.msg. Do not edit."""
import codecs
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct

import arbe_msgs.msg

class arbeClassSingleResponse(genpy.Message):
  _md5sum = "1b400a19f8488a87ab3155d78880124a"
  _type = "arbe_msgs/arbeClassSingleResponse"
  _has_header = False  # flag to mark the presence of a Header object
  _full_text = """bool valid
uint16 obj_id
uint16 obj_index
uint8 inferredClass
float32 conf
string class_str
arbeFusionClasses fusionClasses
float32[] tta_scores
uint32 tta_count

================================================================================
MSG: arbe_msgs/arbeFusionClasses
uint16 FC_BIKE = 0
uint16 FC_TRUCK_BUS = 1
uint16 FC_VRU = 2
uint16 FC_CAR = 3
uint16 FC_NOT_CLASSIFIED = 4
uint16 NUM_CLASSES = 4
"""
  __slots__ = ['valid','obj_id','obj_index','inferredClass','conf','class_str','fusionClasses','tta_scores','tta_count']
  _slot_types = ['bool','uint16','uint16','uint8','float32','string','arbe_msgs/arbeFusionClasses','float32[]','uint32']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       valid,obj_id,obj_index,inferredClass,conf,class_str,fusionClasses,tta_scores,tta_count

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(arbeClassSingleResponse, self).__init__(*args, **kwds)
      # message fields cannot be None, assign default values for those that are
      if self.valid is None:
        self.valid = False
      if self.obj_id is None:
        self.obj_id = 0
      if self.obj_index is None:
        self.obj_index = 0
      if self.inferredClass is None:
        self.inferredClass = 0
      if self.conf is None:
        self.conf = 0.
      if self.class_str is None:
        self.class_str = ''
      if self.fusionClasses is None:
        self.fusionClasses = arbe_msgs.msg.arbeFusionClasses()
      if self.tta_scores is None:
        self.tta_scores = []
      if self.tta_count is None:
        self.tta_count = 0
    else:
      self.valid = False
      self.obj_id = 0
      self.obj_index = 0
      self.inferredClass = 0
      self.conf = 0.
      self.class_str = ''
      self.fusionClasses = arbe_msgs.msg.arbeFusionClasses()
      self.tta_scores = []
      self.tta_count = 0

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
      buff.write(_get_struct_B2HBf().pack(_x.valid, _x.obj_id, _x.obj_index, _x.inferredClass, _x.conf))
      _x = self.class_str
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      length = len(self.tta_scores)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.tta_scores))
      _x = self.tta_count
      buff.write(_get_struct_I().pack(_x))
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
      if self.fusionClasses is None:
        self.fusionClasses = arbe_msgs.msg.arbeFusionClasses()
      end = 0
      _x = self
      start = end
      end += 10
      (_x.valid, _x.obj_id, _x.obj_index, _x.inferredClass, _x.conf,) = _get_struct_B2HBf().unpack(str[start:end])
      self.valid = bool(self.valid)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.class_str = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.class_str = str[start:end]
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.tta_scores = s.unpack(str[start:end])
      start = end
      end += 4
      (self.tta_count,) = _get_struct_I().unpack(str[start:end])
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
      buff.write(_get_struct_B2HBf().pack(_x.valid, _x.obj_id, _x.obj_index, _x.inferredClass, _x.conf))
      _x = self.class_str
      length = len(_x)
      if python3 or type(_x) == unicode:
        _x = _x.encode('utf-8')
        length = len(_x)
      buff.write(struct.Struct('<I%ss'%length).pack(length, _x))
      length = len(self.tta_scores)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.tta_scores.tostring())
      _x = self.tta_count
      buff.write(_get_struct_I().pack(_x))
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
      if self.fusionClasses is None:
        self.fusionClasses = arbe_msgs.msg.arbeFusionClasses()
      end = 0
      _x = self
      start = end
      end += 10
      (_x.valid, _x.obj_id, _x.obj_index, _x.inferredClass, _x.conf,) = _get_struct_B2HBf().unpack(str[start:end])
      self.valid = bool(self.valid)
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      start = end
      end += length
      if python3:
        self.class_str = str[start:end].decode('utf-8', 'rosmsg')
      else:
        self.class_str = str[start:end]
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.tta_scores = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      start = end
      end += 4
      (self.tta_count,) = _get_struct_I().unpack(str[start:end])
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill

_struct_I = genpy.struct_I
def _get_struct_I():
    global _struct_I
    return _struct_I
_struct_B2HBf = None
def _get_struct_B2HBf():
    global _struct_B2HBf
    if _struct_B2HBf is None:
        _struct_B2HBf = struct.Struct("<B2HBf")
    return _struct_B2HBf
