; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbePolarFreeSpacePoly.msg.html

(cl:defclass <arbePolarFreeSpacePoly> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (frameCounter
    :reader frameCounter
    :initarg :frameCounter
    :type cl:fixnum
    :initform 0)
   (rPerAzimuth
    :reader rPerAzimuth
    :initarg :rPerAzimuth
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0)))
)

(cl:defclass arbePolarFreeSpacePoly (<arbePolarFreeSpacePoly>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbePolarFreeSpacePoly>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbePolarFreeSpacePoly)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbePolarFreeSpacePoly> is deprecated: use arbe_msgs-msg:arbePolarFreeSpacePoly instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <arbePolarFreeSpacePoly>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:header-val is deprecated.  Use arbe_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'frameCounter-val :lambda-list '(m))
(cl:defmethod frameCounter-val ((m <arbePolarFreeSpacePoly>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:frameCounter-val is deprecated.  Use arbe_msgs-msg:frameCounter instead.")
  (frameCounter m))

(cl:ensure-generic-function 'rPerAzimuth-val :lambda-list '(m))
(cl:defmethod rPerAzimuth-val ((m <arbePolarFreeSpacePoly>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:rPerAzimuth-val is deprecated.  Use arbe_msgs-msg:rPerAzimuth instead.")
  (rPerAzimuth m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbePolarFreeSpacePoly>) ostream)
  "Serializes a message object of type '<arbePolarFreeSpacePoly>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frameCounter)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frameCounter)) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'rPerAzimuth))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'rPerAzimuth))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbePolarFreeSpacePoly>) istream)
  "Deserializes a message object of type '<arbePolarFreeSpacePoly>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frameCounter)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frameCounter)) (cl:read-byte istream))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'rPerAzimuth) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'rPerAzimuth)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbePolarFreeSpacePoly>)))
  "Returns string type for a message object of type '<arbePolarFreeSpacePoly>"
  "arbe_msgs/arbePolarFreeSpacePoly")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbePolarFreeSpacePoly)))
  "Returns string type for a message object of type 'arbePolarFreeSpacePoly"
  "arbe_msgs/arbePolarFreeSpacePoly")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbePolarFreeSpacePoly>)))
  "Returns md5sum for a message object of type '<arbePolarFreeSpacePoly>"
  "3e4fe2f26c99bce1f836beb8bd1176f9")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbePolarFreeSpacePoly)))
  "Returns md5sum for a message object of type 'arbePolarFreeSpacePoly"
  "3e4fe2f26c99bce1f836beb8bd1176f9")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbePolarFreeSpacePoly>)))
  "Returns full string definition for message of type '<arbePolarFreeSpacePoly>"
  (cl:format cl:nil "std_msgs/Header header~%uint16 frameCounter~%float32[] rPerAzimuth~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbePolarFreeSpacePoly)))
  "Returns full string definition for message of type 'arbePolarFreeSpacePoly"
  (cl:format cl:nil "std_msgs/Header header~%uint16 frameCounter~%float32[] rPerAzimuth~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbePolarFreeSpacePoly>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     2
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'rPerAzimuth) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbePolarFreeSpacePoly>))
  "Converts a ROS message object to a list"
  (cl:list 'arbePolarFreeSpacePoly
    (cl:cons ':header (header msg))
    (cl:cons ':frameCounter (frameCounter msg))
    (cl:cons ':rPerAzimuth (rPerAzimuth msg))
))
