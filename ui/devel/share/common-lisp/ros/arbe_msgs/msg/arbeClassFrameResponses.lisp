; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeClassFrameResponses.msg.html

(cl:defclass <arbeClassFrameResponses> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (frame_counter
    :reader frame_counter
    :initarg :frame_counter
    :type cl:fixnum
    :initform 0)
   (process_time_ms
    :reader process_time_ms
    :initarg :process_time_ms
    :type cl:integer
    :initform 0)
   (responses
    :reader responses
    :initarg :responses
    :type (cl:vector arbe_msgs-msg:arbeClassSingleResponse)
   :initform (cl:make-array 0 :element-type 'arbe_msgs-msg:arbeClassSingleResponse :initial-element (cl:make-instance 'arbe_msgs-msg:arbeClassSingleResponse))))
)

(cl:defclass arbeClassFrameResponses (<arbeClassFrameResponses>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeClassFrameResponses>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeClassFrameResponses)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeClassFrameResponses> is deprecated: use arbe_msgs-msg:arbeClassFrameResponses instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <arbeClassFrameResponses>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:header-val is deprecated.  Use arbe_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'frame_counter-val :lambda-list '(m))
(cl:defmethod frame_counter-val ((m <arbeClassFrameResponses>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:frame_counter-val is deprecated.  Use arbe_msgs-msg:frame_counter instead.")
  (frame_counter m))

(cl:ensure-generic-function 'process_time_ms-val :lambda-list '(m))
(cl:defmethod process_time_ms-val ((m <arbeClassFrameResponses>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:process_time_ms-val is deprecated.  Use arbe_msgs-msg:process_time_ms instead.")
  (process_time_ms m))

(cl:ensure-generic-function 'responses-val :lambda-list '(m))
(cl:defmethod responses-val ((m <arbeClassFrameResponses>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:responses-val is deprecated.  Use arbe_msgs-msg:responses instead.")
  (responses m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeClassFrameResponses>) ostream)
  "Serializes a message object of type '<arbeClassFrameResponses>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_counter)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_counter)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'process_time_ms)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'process_time_ms)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'process_time_ms)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'process_time_ms)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'process_time_ms)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'process_time_ms)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'process_time_ms)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'process_time_ms)) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'responses))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (roslisp-msg-protocol:serialize ele ostream))
   (cl:slot-value msg 'responses))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeClassFrameResponses>) istream)
  "Deserializes a message object of type '<arbeClassFrameResponses>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_counter)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_counter)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'process_time_ms)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'process_time_ms)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'process_time_ms)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'process_time_ms)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'process_time_ms)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'process_time_ms)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'process_time_ms)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'process_time_ms)) (cl:read-byte istream))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'responses) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'responses)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:aref vals i) (cl:make-instance 'arbe_msgs-msg:arbeClassSingleResponse))
  (roslisp-msg-protocol:deserialize (cl:aref vals i) istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeClassFrameResponses>)))
  "Returns string type for a message object of type '<arbeClassFrameResponses>"
  "arbe_msgs/arbeClassFrameResponses")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeClassFrameResponses)))
  "Returns string type for a message object of type 'arbeClassFrameResponses"
  "arbe_msgs/arbeClassFrameResponses")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeClassFrameResponses>)))
  "Returns md5sum for a message object of type '<arbeClassFrameResponses>"
  "5abc2de3f3441b5705fbb97c192204d4")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeClassFrameResponses)))
  "Returns md5sum for a message object of type 'arbeClassFrameResponses"
  "5abc2de3f3441b5705fbb97c192204d4")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeClassFrameResponses>)))
  "Returns full string definition for message of type '<arbeClassFrameResponses>"
  (cl:format cl:nil "std_msgs/Header header~%uint16 frame_counter~%uint64 process_time_ms~%arbeClassSingleResponse[] responses~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeClassSingleResponse~%bool valid~%uint16 obj_id~%uint16 obj_index~%uint8 inferredClass~%float32 conf~%string class_str~%arbeFusionClasses fusionClasses~%float32[] tta_scores~%uint32 tta_count~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeClassFrameResponses)))
  "Returns full string definition for message of type 'arbeClassFrameResponses"
  (cl:format cl:nil "std_msgs/Header header~%uint16 frame_counter~%uint64 process_time_ms~%arbeClassSingleResponse[] responses~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeClassSingleResponse~%bool valid~%uint16 obj_id~%uint16 obj_index~%uint8 inferredClass~%float32 conf~%string class_str~%arbeFusionClasses fusionClasses~%float32[] tta_scores~%uint32 tta_count~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeClassFrameResponses>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     2
     8
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'responses) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ (roslisp-msg-protocol:serialization-length ele))))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeClassFrameResponses>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeClassFrameResponses
    (cl:cons ':header (header msg))
    (cl:cons ':frame_counter (frame_counter msg))
    (cl:cons ':process_time_ms (process_time_ms msg))
    (cl:cons ':responses (responses msg))
))
