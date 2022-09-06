; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeSlamMsg.msg.html

(cl:defclass <arbeSlamMsg> (roslisp-msg-protocol:ros-message)
  ((header
    :reader header
    :initarg :header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (meta_data
    :reader meta_data
    :initarg :meta_data
    :type arbe_msgs-msg:arbeTSlamMetadata
    :initform (cl:make-instance 'arbe_msgs-msg:arbeTSlamMetadata))
   (ObjectsBuffer
    :reader ObjectsBuffer
    :initarg :ObjectsBuffer
    :type (cl:vector arbe_msgs-msg:arbeTSlamObj)
   :initform (cl:make-array 0 :element-type 'arbe_msgs-msg:arbeTSlamObj :initial-element (cl:make-instance 'arbe_msgs-msg:arbeTSlamObj)))
   (fusionClasses
    :reader fusionClasses
    :initarg :fusionClasses
    :type arbe_msgs-msg:arbeFusionClasses
    :initform (cl:make-instance 'arbe_msgs-msg:arbeFusionClasses)))
)

(cl:defclass arbeSlamMsg (<arbeSlamMsg>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeSlamMsg>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeSlamMsg)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeSlamMsg> is deprecated: use arbe_msgs-msg:arbeSlamMsg instead.")))

(cl:ensure-generic-function 'header-val :lambda-list '(m))
(cl:defmethod header-val ((m <arbeSlamMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:header-val is deprecated.  Use arbe_msgs-msg:header instead.")
  (header m))

(cl:ensure-generic-function 'meta_data-val :lambda-list '(m))
(cl:defmethod meta_data-val ((m <arbeSlamMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:meta_data-val is deprecated.  Use arbe_msgs-msg:meta_data instead.")
  (meta_data m))

(cl:ensure-generic-function 'ObjectsBuffer-val :lambda-list '(m))
(cl:defmethod ObjectsBuffer-val ((m <arbeSlamMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:ObjectsBuffer-val is deprecated.  Use arbe_msgs-msg:ObjectsBuffer instead.")
  (ObjectsBuffer m))

(cl:ensure-generic-function 'fusionClasses-val :lambda-list '(m))
(cl:defmethod fusionClasses-val ((m <arbeSlamMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:fusionClasses-val is deprecated.  Use arbe_msgs-msg:fusionClasses instead.")
  (fusionClasses m))
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql '<arbeSlamMsg>)))
    "Constants for message type '<arbeSlamMsg>"
  '((:FC_BIKE . 0)
    (:FC_TRUCK_BUS . 1)
    (:FC_VRU . 2)
    (:FC_CAR . 3)
    (:FC_NOT_CLASSIFIED . 4))
)
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql 'arbeSlamMsg)))
    "Constants for message type 'arbeSlamMsg"
  '((:FC_BIKE . 0)
    (:FC_TRUCK_BUS . 1)
    (:FC_VRU . 2)
    (:FC_CAR . 3)
    (:FC_NOT_CLASSIFIED . 4))
)
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeSlamMsg>) ostream)
  "Serializes a message object of type '<arbeSlamMsg>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'header) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'meta_data) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'ObjectsBuffer))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (roslisp-msg-protocol:serialize ele ostream))
   (cl:slot-value msg 'ObjectsBuffer))
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'fusionClasses) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeSlamMsg>) istream)
  "Deserializes a message object of type '<arbeSlamMsg>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'header) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'meta_data) istream)
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'ObjectsBuffer) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'ObjectsBuffer)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:aref vals i) (cl:make-instance 'arbe_msgs-msg:arbeTSlamObj))
  (roslisp-msg-protocol:deserialize (cl:aref vals i) istream))))
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'fusionClasses) istream)
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeSlamMsg>)))
  "Returns string type for a message object of type '<arbeSlamMsg>"
  "arbe_msgs/arbeSlamMsg")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeSlamMsg)))
  "Returns string type for a message object of type 'arbeSlamMsg"
  "arbe_msgs/arbeSlamMsg")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeSlamMsg>)))
  "Returns md5sum for a message object of type '<arbeSlamMsg>"
  "ad8727f4b769ffeff212c8a8f681cd8a")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeSlamMsg)))
  "Returns md5sum for a message object of type 'arbeSlamMsg"
  "ad8727f4b769ffeff212c8a8f681cd8a")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeSlamMsg>)))
  "Returns full string definition for message of type '<arbeSlamMsg>"
  (cl:format cl:nil "std_msgs/Header header~%arbeTSlamMetadata  meta_data~%arbeTSlamObj[] ObjectsBuffer~%~%arbeFusionClasses fusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamMetadata~%uint64  CorresPcTime~%uint16  FrameNumber~%float32 HostHeading~%float32 HostHeadingUnc~%float32 HostVelocity~%float32 HostVelocityUnc~%float32 HostOmega		~%float32 HostOmegaUnc~%uint16  NumberOfObjects~%float32 local_catr_x~%float32 local_catr_y~%bool    location_valid~%bool    is_reset_geo_location~%~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamObj~%uint16              ID~%uint16              obj_class~%float32             class_conf~%arbeTSlamPos        position~%arbeTSlamVelocity   velocity ~%arbeTSlamBox        bounding_box~%uint32              age~%uint16              last_frame_update~%~%~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamPos~%float32 x ~%float32 y ~%float32 z ~%float32  dx ~%float32  dy ~%float32  dz~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamVelocity~%float32     x_dot ~%float32     y_dot ~%float32      dx_dot ~%float32      dy_dot ~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamBox~%float32 scale_x     ~%float32 scale_y     ~%float32 scale_z     ~%float32 scale_x_unc ~%float32 scale_y_unc ~%float32 scale_z_unc ~%float32 orientation_unc  ~%float32 orientation  ~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeSlamMsg)))
  "Returns full string definition for message of type 'arbeSlamMsg"
  (cl:format cl:nil "std_msgs/Header header~%arbeTSlamMetadata  meta_data~%arbeTSlamObj[] ObjectsBuffer~%~%arbeFusionClasses fusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamMetadata~%uint64  CorresPcTime~%uint16  FrameNumber~%float32 HostHeading~%float32 HostHeadingUnc~%float32 HostVelocity~%float32 HostVelocityUnc~%float32 HostOmega		~%float32 HostOmegaUnc~%uint16  NumberOfObjects~%float32 local_catr_x~%float32 local_catr_y~%bool    location_valid~%bool    is_reset_geo_location~%~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamObj~%uint16              ID~%uint16              obj_class~%float32             class_conf~%arbeTSlamPos        position~%arbeTSlamVelocity   velocity ~%arbeTSlamBox        bounding_box~%uint32              age~%uint16              last_frame_update~%~%~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamPos~%float32 x ~%float32 y ~%float32 z ~%float32  dx ~%float32  dy ~%float32  dz~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamVelocity~%float32     x_dot ~%float32     y_dot ~%float32      dx_dot ~%float32      dy_dot ~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamBox~%float32 scale_x     ~%float32 scale_y     ~%float32 scale_z     ~%float32 scale_x_unc ~%float32 scale_y_unc ~%float32 scale_z_unc ~%float32 orientation_unc  ~%float32 orientation  ~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeSlamMsg>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'header))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'meta_data))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'ObjectsBuffer) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ (roslisp-msg-protocol:serialization-length ele))))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'fusionClasses))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeSlamMsg>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeSlamMsg
    (cl:cons ':header (header msg))
    (cl:cons ':meta_data (meta_data msg))
    (cl:cons ':ObjectsBuffer (ObjectsBuffer msg))
    (cl:cons ':fusionClasses (fusionClasses msg))
))
