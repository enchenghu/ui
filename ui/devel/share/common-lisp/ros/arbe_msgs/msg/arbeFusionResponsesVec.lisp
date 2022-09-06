; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeFusionResponsesVec.msg.html

(cl:defclass <arbeFusionResponsesVec> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (dt
    :reader dt
    :initarg :dt
    :type cl:integer
    :initform 0)
   (responses
    :reader responses
    :initarg :responses
    :type (cl:vector arbe_msgs-msg:arbeFusionResponse)
   :initform (cl:make-array 0 :element-type 'arbe_msgs-msg:arbeFusionResponse :initial-element (cl:make-instance 'arbe_msgs-msg:arbeFusionResponse))))
)

(cl:defclass arbeFusionResponsesVec (<arbeFusionResponsesVec>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeFusionResponsesVec>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeFusionResponsesVec)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeFusionResponsesVec> is deprecated: use arbe_msgs-msg:arbeFusionResponsesVec instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <arbeFusionResponsesVec>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:header-val is deprecated.  Use arbe_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'dt-val :lambda-list '(m))
(cl:defmethod dt-val ((m <arbeFusionResponsesVec>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:dt-val is deprecated.  Use arbe_msgs-msg:dt instead.")
  (dt m))

(cl:ensure-generic-function 'responses-val :lambda-list '(m))
(cl:defmethod responses-val ((m <arbeFusionResponsesVec>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:responses-val is deprecated.  Use arbe_msgs-msg:responses instead.")
  (responses m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeFusionResponsesVec>) ostream)
  "Serializes a message object of type '<arbeFusionResponsesVec>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'dt)) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'responses))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (roslisp-msg-protocol:serialize ele ostream))
   (cl:slot-value msg 'responses))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeFusionResponsesVec>) istream)
  "Deserializes a message object of type '<arbeFusionResponsesVec>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'dt)) (cl:read-byte istream))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'responses) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'responses)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:aref vals i) (cl:make-instance 'arbe_msgs-msg:arbeFusionResponse))
  (roslisp-msg-protocol:deserialize (cl:aref vals i) istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeFusionResponsesVec>)))
  "Returns string type for a message object of type '<arbeFusionResponsesVec>"
  "arbe_msgs/arbeFusionResponsesVec")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeFusionResponsesVec)))
  "Returns string type for a message object of type 'arbeFusionResponsesVec"
  "arbe_msgs/arbeFusionResponsesVec")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeFusionResponsesVec>)))
  "Returns md5sum for a message object of type '<arbeFusionResponsesVec>"
  "e85897c08185dc25f773baf17831351f")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeFusionResponsesVec)))
  "Returns md5sum for a message object of type 'arbeFusionResponsesVec"
  "e85897c08185dc25f773baf17831351f")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeFusionResponsesVec>)))
  "Returns full string definition for message of type '<arbeFusionResponsesVec>"
  (cl:format cl:nil "std_msgs/Header header~%uint64 dt~%arbeFusionResponse[] responses~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeFusionResponse~%bool valid~%uint16 obj_id~%uint32 query_id~%uint8 fusionClass~%uint16 obj_index~%uint64 queryTime~%float32 conf~%uint8 tta_count~%float32[] tta_scores~%string class_str~%arbeFusionClasses fusionClasses~%~%uint16  top~%uint16  left~%uint16  bottom~%uint16  right~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeFusionResponsesVec)))
  "Returns full string definition for message of type 'arbeFusionResponsesVec"
  (cl:format cl:nil "std_msgs/Header header~%uint64 dt~%arbeFusionResponse[] responses~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeFusionResponse~%bool valid~%uint16 obj_id~%uint32 query_id~%uint8 fusionClass~%uint16 obj_index~%uint64 queryTime~%float32 conf~%uint8 tta_count~%float32[] tta_scores~%string class_str~%arbeFusionClasses fusionClasses~%~%uint16  top~%uint16  left~%uint16  bottom~%uint16  right~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeFusionResponsesVec>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     8
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'responses) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ (roslisp-msg-protocol:serialization-length ele))))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeFusionResponsesVec>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeFusionResponsesVec
    (cl:cons ':header (header msg))
    (cl:cons ':dt (dt msg))
    (cl:cons ':responses (responses msg))
))
