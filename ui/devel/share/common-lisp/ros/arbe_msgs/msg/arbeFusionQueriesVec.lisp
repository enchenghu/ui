; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeFusionQueriesVec.msg.html

(cl:defclass <arbeFusionQueriesVec> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (queries
    :reader queries
    :initarg :queries
    :type (cl:vector arbe_msgs-msg:arbeFusionQuery)
   :initform (cl:make-array 0 :element-type 'arbe_msgs-msg:arbeFusionQuery :initial-element (cl:make-instance 'arbe_msgs-msg:arbeFusionQuery))))
)

(cl:defclass arbeFusionQueriesVec (<arbeFusionQueriesVec>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeFusionQueriesVec>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeFusionQueriesVec)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeFusionQueriesVec> is deprecated: use arbe_msgs-msg:arbeFusionQueriesVec instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <arbeFusionQueriesVec>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:header-val is deprecated.  Use arbe_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'queries-val :lambda-list '(m))
(cl:defmethod queries-val ((m <arbeFusionQueriesVec>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:queries-val is deprecated.  Use arbe_msgs-msg:queries instead.")
  (queries m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeFusionQueriesVec>) ostream)
  "Serializes a message object of type '<arbeFusionQueriesVec>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'queries))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (roslisp-msg-protocol:serialize ele ostream))
   (cl:slot-value msg 'queries))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeFusionQueriesVec>) istream)
  "Deserializes a message object of type '<arbeFusionQueriesVec>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'queries) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'queries)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:aref vals i) (cl:make-instance 'arbe_msgs-msg:arbeFusionQuery))
  (roslisp-msg-protocol:deserialize (cl:aref vals i) istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeFusionQueriesVec>)))
  "Returns string type for a message object of type '<arbeFusionQueriesVec>"
  "arbe_msgs/arbeFusionQueriesVec")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeFusionQueriesVec)))
  "Returns string type for a message object of type 'arbeFusionQueriesVec"
  "arbe_msgs/arbeFusionQueriesVec")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeFusionQueriesVec>)))
  "Returns md5sum for a message object of type '<arbeFusionQueriesVec>"
  "ba44955926ecbdeed3c3d177166ac4a8")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeFusionQueriesVec)))
  "Returns md5sum for a message object of type 'arbeFusionQueriesVec"
  "ba44955926ecbdeed3c3d177166ac4a8")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeFusionQueriesVec>)))
  "Returns full string definition for message of type '<arbeFusionQueriesVec>"
  (cl:format cl:nil "std_msgs/Header header~%arbeFusionQuery[] queries~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeFusionQuery~%uint16  	obj_id~%uint16  	top~%uint16  	left~%uint16  	bottom~%uint16  	right~%uint32  	query_id~%uint64	  	query_time~%uint16  	obj_index~%uint8  	tta_count~%float32[] tta_scores~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeFusionQueriesVec)))
  "Returns full string definition for message of type 'arbeFusionQueriesVec"
  (cl:format cl:nil "std_msgs/Header header~%arbeFusionQuery[] queries~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeFusionQuery~%uint16  	obj_id~%uint16  	top~%uint16  	left~%uint16  	bottom~%uint16  	right~%uint32  	query_id~%uint64	  	query_time~%uint16  	obj_index~%uint8  	tta_count~%float32[] tta_scores~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeFusionQueriesVec>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'queries) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ (roslisp-msg-protocol:serialization-length ele))))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeFusionQueriesVec>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeFusionQueriesVec
    (cl:cons ':header (header msg))
    (cl:cons ':queries (queries msg))
))
