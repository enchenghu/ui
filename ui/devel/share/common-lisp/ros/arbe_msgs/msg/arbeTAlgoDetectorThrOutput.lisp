; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeTAlgoDetectorThrOutput.msg.html

(cl:defclass <arbeTAlgoDetectorThrOutput> (roslisp-msg-protocol:ros-message)
  ((RosHeader
    :reader RosHeader
    :initarg :RosHeader
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (tinfo
    :reader tinfo
    :initarg :tinfo
    :type arbe_msgs-msg:arbeTAlgoDetectorThrInfo
    :initform (cl:make-instance 'arbe_msgs-msg:arbeTAlgoDetectorThrInfo)))
)

(cl:defclass arbeTAlgoDetectorThrOutput (<arbeTAlgoDetectorThrOutput>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeTAlgoDetectorThrOutput>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeTAlgoDetectorThrOutput)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeTAlgoDetectorThrOutput> is deprecated: use arbe_msgs-msg:arbeTAlgoDetectorThrOutput instead.")))

(cl:ensure-generic-function 'RosHeader-val :lambda-list '(m))
(cl:defmethod RosHeader-val ((m <arbeTAlgoDetectorThrOutput>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:RosHeader-val is deprecated.  Use arbe_msgs-msg:RosHeader instead.")
  (RosHeader m))

(cl:ensure-generic-function 'tinfo-val :lambda-list '(m))
(cl:defmethod tinfo-val ((m <arbeTAlgoDetectorThrOutput>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tinfo-val is deprecated.  Use arbe_msgs-msg:tinfo instead.")
  (tinfo m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeTAlgoDetectorThrOutput>) ostream)
  "Serializes a message object of type '<arbeTAlgoDetectorThrOutput>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'RosHeader) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'tinfo) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeTAlgoDetectorThrOutput>) istream)
  "Deserializes a message object of type '<arbeTAlgoDetectorThrOutput>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'RosHeader) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'tinfo) istream)
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeTAlgoDetectorThrOutput>)))
  "Returns string type for a message object of type '<arbeTAlgoDetectorThrOutput>"
  "arbe_msgs/arbeTAlgoDetectorThrOutput")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeTAlgoDetectorThrOutput)))
  "Returns string type for a message object of type 'arbeTAlgoDetectorThrOutput"
  "arbe_msgs/arbeTAlgoDetectorThrOutput")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeTAlgoDetectorThrOutput>)))
  "Returns md5sum for a message object of type '<arbeTAlgoDetectorThrOutput>"
  "888d5fc086e6ad00dbaf413f9befa62b")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeTAlgoDetectorThrOutput)))
  "Returns md5sum for a message object of type 'arbeTAlgoDetectorThrOutput"
  "888d5fc086e6ad00dbaf413f9befa62b")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeTAlgoDetectorThrOutput>)))
  "Returns full string definition for message of type '<arbeTAlgoDetectorThrOutput>"
  (cl:format cl:nil "std_msgs/Header RosHeader~%arbeTAlgoDetectorThrInfo tinfo~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeTAlgoDetectorThrInfo~%uint32 unFrameNumber~%uint8 eAlgoOutput~%uint8 eThresholdType~%uint8 eFrameType~%uint32[] Data~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeTAlgoDetectorThrOutput)))
  "Returns full string definition for message of type 'arbeTAlgoDetectorThrOutput"
  (cl:format cl:nil "std_msgs/Header RosHeader~%arbeTAlgoDetectorThrInfo tinfo~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeTAlgoDetectorThrInfo~%uint32 unFrameNumber~%uint8 eAlgoOutput~%uint8 eThresholdType~%uint8 eFrameType~%uint32[] Data~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeTAlgoDetectorThrOutput>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'RosHeader))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'tinfo))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeTAlgoDetectorThrOutput>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeTAlgoDetectorThrOutput
    (cl:cons ':RosHeader (RosHeader msg))
    (cl:cons ':tinfo (tinfo msg))
))
