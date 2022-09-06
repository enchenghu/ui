; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeRdInclination.msg.html

(cl:defclass <arbeRdInclination> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (ant_height
    :reader ant_height
    :initarg :ant_height
    :type cl:float
    :initform 0.0)
   (ant_tilt
    :reader ant_tilt
    :initarg :ant_tilt
    :type cl:float
    :initform 0.0))
)

(cl:defclass arbeRdInclination (<arbeRdInclination>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeRdInclination>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeRdInclination)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeRdInclination> is deprecated: use arbe_msgs-msg:arbeRdInclination instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <arbeRdInclination>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:header-val is deprecated.  Use arbe_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'ant_height-val :lambda-list '(m))
(cl:defmethod ant_height-val ((m <arbeRdInclination>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:ant_height-val is deprecated.  Use arbe_msgs-msg:ant_height instead.")
  (ant_height m))

(cl:ensure-generic-function 'ant_tilt-val :lambda-list '(m))
(cl:defmethod ant_tilt-val ((m <arbeRdInclination>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:ant_tilt-val is deprecated.  Use arbe_msgs-msg:ant_tilt instead.")
  (ant_tilt m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeRdInclination>) ostream)
  "Serializes a message object of type '<arbeRdInclination>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'ant_height))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'ant_tilt))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeRdInclination>) istream)
  "Deserializes a message object of type '<arbeRdInclination>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'ant_height) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'ant_tilt) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeRdInclination>)))
  "Returns string type for a message object of type '<arbeRdInclination>"
  "arbe_msgs/arbeRdInclination")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeRdInclination)))
  "Returns string type for a message object of type 'arbeRdInclination"
  "arbe_msgs/arbeRdInclination")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeRdInclination>)))
  "Returns md5sum for a message object of type '<arbeRdInclination>"
  "401e82b1a597051854c2fb29fdf032ba")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeRdInclination)))
  "Returns md5sum for a message object of type 'arbeRdInclination"
  "401e82b1a597051854c2fb29fdf032ba")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeRdInclination>)))
  "Returns full string definition for message of type '<arbeRdInclination>"
  (cl:format cl:nil "std_msgs/Header header~%float32 ant_height~%float32 ant_tilt~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeRdInclination)))
  "Returns full string definition for message of type 'arbeRdInclination"
  (cl:format cl:nil "std_msgs/Header header~%float32 ant_height~%float32 ant_tilt~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeRdInclination>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeRdInclination>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeRdInclination
    (cl:cons ':header (header msg))
    (cl:cons ':ant_height (ant_height msg))
    (cl:cons ':ant_tilt (ant_tilt msg))
))
