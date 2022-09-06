; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeBoolWithTime.msg.html

(cl:defclass <arbeBoolWithTime> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (flag
    :reader flag
    :initarg :flag
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass arbeBoolWithTime (<arbeBoolWithTime>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeBoolWithTime>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeBoolWithTime)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeBoolWithTime> is deprecated: use arbe_msgs-msg:arbeBoolWithTime instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <arbeBoolWithTime>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:header-val is deprecated.  Use arbe_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'flag-val :lambda-list '(m))
(cl:defmethod flag-val ((m <arbeBoolWithTime>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:flag-val is deprecated.  Use arbe_msgs-msg:flag instead.")
  (flag m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeBoolWithTime>) ostream)
  "Serializes a message object of type '<arbeBoolWithTime>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'flag) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeBoolWithTime>) istream)
  "Deserializes a message object of type '<arbeBoolWithTime>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:setf (cl:slot-value msg 'flag) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeBoolWithTime>)))
  "Returns string type for a message object of type '<arbeBoolWithTime>"
  "arbe_msgs/arbeBoolWithTime")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeBoolWithTime)))
  "Returns string type for a message object of type 'arbeBoolWithTime"
  "arbe_msgs/arbeBoolWithTime")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeBoolWithTime>)))
  "Returns md5sum for a message object of type '<arbeBoolWithTime>"
  "1faeb665af69705369ce1bafa748a16c")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeBoolWithTime)))
  "Returns md5sum for a message object of type 'arbeBoolWithTime"
  "1faeb665af69705369ce1bafa748a16c")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeBoolWithTime>)))
  "Returns full string definition for message of type '<arbeBoolWithTime>"
  (cl:format cl:nil "std_msgs/Header header~%bool flag~%~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeBoolWithTime)))
  "Returns full string definition for message of type 'arbeBoolWithTime"
  (cl:format cl:nil "std_msgs/Header header~%bool flag~%~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeBoolWithTime>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeBoolWithTime>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeBoolWithTime
    (cl:cons ':header (header msg))
    (cl:cons ':flag (flag msg))
))
