# This Python file uses the following encoding: utf-8
"""autogenerated by genpy from arbe_msgs/arbeFusionQuery.msg. Do not edit."""
import codecs
import sys
python3 = True if sys.hexversion > 0x03000000 else False
import genpy
import struct


class arbeFusionQuery(genpy.Message):
  _md5sum = "d6277f1c47cde1be60fd589e75618a8d"
  _type = "arbe_msgs/arbeFusionQuery"
  _has_header = False  # flag to mark the presence of a Header object
  _full_text = """uint16  	obj_id
uint16  	top
uint16  	left
uint16  	bottom
uint16  	right
uint32  	query_id
uint64	  	query_time
uint16  	obj_index
uint8  	tta_count
float32[] tta_scores

"""
  __slots__ = ['obj_id','top','left','bottom','right','query_id','query_time','obj_index','tta_count','tta_scores']
  _slot_types = ['uint16','uint16','uint16','uint16','uint16','uint32','uint64','uint16','uint8','float32[]']

  def __init__(self, *args, **kwds):
    """
    Constructor. Any message fields that are implicitly/explicitly
    set to None will be assigned a default value. The recommend
    use is keyword arguments as this is more robust to future message
    changes.  You cannot mix in-order arguments and keyword arguments.

    The available fields are:
       obj_id,top,left,bottom,right,query_id,query_time,obj_index,tta_count,tta_scores

    :param args: complete set of field values, in .msg order
    :param kwds: use keyword arguments corresponding to message field names
    to set specific fields.
    """
    if args or kwds:
      super(arbeFusionQuery, self).__init__(*args, **kwds)
      # message fields cannot be None, assign default values for those that are
      if self.obj_id is None:
        self.obj_id = 0
      if self.top is None:
        self.top = 0
      if self.left is None:
        self.left = 0
      if self.bottom is None:
        self.bottom = 0
      if self.right is None:
        self.right = 0
      if self.query_id is None:
        self.query_id = 0
      if self.query_time is None:
        self.query_time = 0
      if self.obj_index is None:
        self.obj_index = 0
      if self.tta_count is None:
        self.tta_count = 0
      if self.tta_scores is None:
        self.tta_scores = []
    else:
      self.obj_id = 0
      self.top = 0
      self.left = 0
      self.bottom = 0
      self.right = 0
      self.query_id = 0
      self.query_time = 0
      self.obj_index = 0
      self.tta_count = 0
      self.tta_scores = []

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
      buff.write(_get_struct_5HIQHB().pack(_x.obj_id, _x.top, _x.left, _x.bottom, _x.right, _x.query_id, _x.query_time, _x.obj_index, _x.tta_count))
      length = len(self.tta_scores)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(struct.Struct(pattern).pack(*self.tta_scores))
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
      end += 25
      (_x.obj_id, _x.top, _x.left, _x.bottom, _x.right, _x.query_id, _x.query_time, _x.obj_index, _x.tta_count,) = _get_struct_5HIQHB().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.tta_scores = s.unpack(str[start:end])
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
      buff.write(_get_struct_5HIQHB().pack(_x.obj_id, _x.top, _x.left, _x.bottom, _x.right, _x.query_id, _x.query_time, _x.obj_index, _x.tta_count))
      length = len(self.tta_scores)
      buff.write(_struct_I.pack(length))
      pattern = '<%sf'%length
      buff.write(self.tta_scores.tostring())
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
      end += 25
      (_x.obj_id, _x.top, _x.left, _x.bottom, _x.right, _x.query_id, _x.query_time, _x.obj_index, _x.tta_count,) = _get_struct_5HIQHB().unpack(str[start:end])
      start = end
      end += 4
      (length,) = _struct_I.unpack(str[start:end])
      pattern = '<%sf'%length
      start = end
      s = struct.Struct(pattern)
      end += s.size
      self.tta_scores = numpy.frombuffer(str[start:end], dtype=numpy.float32, count=length)
      return self
    except struct.error as e:
      raise genpy.DeserializationError(e)  # most likely buffer underfill

_struct_I = genpy.struct_I
def _get_struct_I():
    global _struct_I
    return _struct_I
_struct_5HIQHB = None
def _get_struct_5HIQHB():
    global _struct_5HIQHB
    if _struct_5HIQHB is None:
        _struct_5HIQHB = struct.Struct("<5HIQHB")
    return _struct_5HIQHB
