; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeBuildNumber.msg.html

(cl:defclass <arbeBuildNumber> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (slam
    :reader slam
    :initarg :slam
    :type cl:integer
    :initform 0)
   (arbe_gui
    :reader arbe_gui
    :initarg :arbe_gui
    :type cl:integer
    :initform 0)
   (arbe_api_common
    :reader arbe_api_common
    :initarg :arbe_api_common
    :type cl:integer
    :initform 0)
   (arbe_msgs
    :reader arbe_msgs
    :initarg :arbe_msgs
    :type cl:integer
    :initform 0)
   (radar
    :reader radar
    :initarg :radar
    :type cl:integer
    :initform 0))
)

(cl:defclass arbeBuildNumber (<arbeBuildNumber>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeBuildNumber>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeBuildNumber)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeBuildNumber> is deprecated: use arbe_msgs-msg:arbeBuildNumber instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <arbeBuildNumber>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:header-val is deprecated.  Use arbe_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'slam-val :lambda-list '(m))
(cl:defmethod slam-val ((m <arbeBuildNumber>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:slam-val is deprecated.  Use arbe_msgs-msg:slam instead.")
  (slam m))

(cl:ensure-generic-function 'arbe_gui-val :lambda-list '(m))
(cl:defmethod arbe_gui-val ((m <arbeBuildNumber>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:arbe_gui-val is deprecated.  Use arbe_msgs-msg:arbe_gui instead.")
  (arbe_gui m))

(cl:ensure-generic-function 'arbe_api_common-val :lambda-list '(m))
(cl:defmethod arbe_api_common-val ((m <arbeBuildNumber>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:arbe_api_common-val is deprecated.  Use arbe_msgs-msg:arbe_api_common instead.")
  (arbe_api_common m))

(cl:ensure-generic-function 'arbe_msgs-val :lambda-list '(m))
(cl:defmethod arbe_msgs-val ((m <arbeBuildNumber>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:arbe_msgs-val is deprecated.  Use arbe_msgs-msg:arbe_msgs instead.")
  (arbe_msgs m))

(cl:ensure-generic-function 'radar-val :lambda-list '(m))
(cl:defmethod radar-val ((m <arbeBuildNumber>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:radar-val is deprecated.  Use arbe_msgs-msg:radar instead.")
  (radar m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeBuildNumber>) ostream)
  "Serializes a message object of type '<arbeBuildNumber>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:let* ((signed (cl:slot-value msg 'slam)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'arbe_gui)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'arbe_api_common)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'arbe_msgs)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
  (cl:let* ((signed (cl:slot-value msg 'radar)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 4294967296) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) unsigned) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) unsigned) ostream)
    )
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeBuildNumber>) istream)
  "Deserializes a message object of type '<arbeBuildNumber>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'slam) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'arbe_gui) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'arbe_api_common) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'arbe_msgs) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) unsigned) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'radar) (cl:if (cl:< unsigned 2147483648) unsigned (cl:- unsigned 4294967296))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeBuildNumber>)))
  "Returns string type for a message object of type '<arbeBuildNumber>"
  "arbe_msgs/arbeBuildNumber")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeBuildNumber)))
  "Returns string type for a message object of type 'arbeBuildNumber"
  "arbe_msgs/arbeBuildNumber")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeBuildNumber>)))
  "Returns md5sum for a message object of type '<arbeBuildNumber>"
  "45c45703c45e662d3433cd39dca68964")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeBuildNumber)))
  "Returns md5sum for a message object of type 'arbeBuildNumber"
  "45c45703c45e662d3433cd39dca68964")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeBuildNumber>)))
  "Returns full string definition for message of type '<arbeBuildNumber>"
  (cl:format cl:nil "std_msgs/Header header~%int32 slam~%int32 arbe_gui~%int32 arbe_api_common~%int32 arbe_msgs~%int32 radar~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeBuildNumber)))
  "Returns full string definition for message of type 'arbeBuildNumber"
  (cl:format cl:nil "std_msgs/Header header~%int32 slam~%int32 arbe_gui~%int32 arbe_api_common~%int32 arbe_msgs~%int32 radar~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeBuildNumber>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     4
     4
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeBuildNumber>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeBuildNumber
    (cl:cons ':header (header msg))
    (cl:cons ':slam (slam msg))
    (cl:cons ':arbe_gui (arbe_gui msg))
    (cl:cons ':arbe_api_common (arbe_api_common msg))
    (cl:cons ':arbe_msgs (arbe_msgs msg))
    (cl:cons ':radar (radar msg))
))
