; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeClassificationTrainingPc.msg.html

(cl:defclass <arbeClassificationTrainingPc> (roslisp-msg-protocol:ros-message)
  ((Header
    :reader Header
    :initarg :Header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (Pc
    :reader Pc
    :initarg :Pc
    :type arbe_msgs-msg:arbeNewPcMsg
    :initform (cl:make-instance 'arbe_msgs-msg:arbeNewPcMsg))
   (fusionResponse
    :reader fusionResponse
    :initarg :fusionResponse
    :type arbe_msgs-msg:arbeFusionResponse
    :initform (cl:make-instance 'arbe_msgs-msg:arbeFusionResponse))
   (matching_com_indices
    :reader matching_com_indices
    :initarg :matching_com_indices
    :type (cl:vector cl:fixnum)
   :initform (cl:make-array 0 :element-type 'cl:fixnum :initial-element 0)))
)

(cl:defclass arbeClassificationTrainingPc (<arbeClassificationTrainingPc>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeClassificationTrainingPc>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeClassificationTrainingPc)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeClassificationTrainingPc> is deprecated: use arbe_msgs-msg:arbeClassificationTrainingPc instead.")))

(cl:ensure-generic-function 'Header-val :lambda-list '(m))
(cl:defmethod Header-val ((m <arbeClassificationTrainingPc>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:Header-val is deprecated.  Use arbe_msgs-msg:Header instead.")
  (Header m))

(cl:ensure-generic-function 'Pc-val :lambda-list '(m))
(cl:defmethod Pc-val ((m <arbeClassificationTrainingPc>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:Pc-val is deprecated.  Use arbe_msgs-msg:Pc instead.")
  (Pc m))

(cl:ensure-generic-function 'fusionResponse-val :lambda-list '(m))
(cl:defmethod fusionResponse-val ((m <arbeClassificationTrainingPc>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:fusionResponse-val is deprecated.  Use arbe_msgs-msg:fusionResponse instead.")
  (fusionResponse m))

(cl:ensure-generic-function 'matching_com_indices-val :lambda-list '(m))
(cl:defmethod matching_com_indices-val ((m <arbeClassificationTrainingPc>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:matching_com_indices-val is deprecated.  Use arbe_msgs-msg:matching_com_indices instead.")
  (matching_com_indices m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeClassificationTrainingPc>) ostream)
  "Serializes a message object of type '<arbeClassificationTrainingPc>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'Header) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'Pc) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'fusionResponse) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'matching_com_indices))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:write-byte (cl:ldb (cl:byte 8 0) ele) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) ele) ostream))
   (cl:slot-value msg 'matching_com_indices))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeClassificationTrainingPc>) istream)
  "Deserializes a message object of type '<arbeClassificationTrainingPc>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'Header) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'Pc) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'fusionResponse) istream)
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'matching_com_indices) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'matching_com_indices)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:aref vals i)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:aref vals i)) (cl:read-byte istream)))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeClassificationTrainingPc>)))
  "Returns string type for a message object of type '<arbeClassificationTrainingPc>"
  "arbe_msgs/arbeClassificationTrainingPc")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeClassificationTrainingPc)))
  "Returns string type for a message object of type 'arbeClassificationTrainingPc"
  "arbe_msgs/arbeClassificationTrainingPc")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeClassificationTrainingPc>)))
  "Returns md5sum for a message object of type '<arbeClassificationTrainingPc>"
  "e9b7fe5d9944489810bf1fbe1b2cac94")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeClassificationTrainingPc)))
  "Returns md5sum for a message object of type 'arbeClassificationTrainingPc"
  "e9b7fe5d9944489810bf1fbe1b2cac94")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeClassificationTrainingPc>)))
  "Returns full string definition for message of type '<arbeClassificationTrainingPc>"
  (cl:format cl:nil "std_msgs/Header Header~%arbeNewPcMsg Pc~%arbeFusionResponse fusionResponse~%uint16[] matching_com_indices~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeNewPcMsg~%std_msgs/Header RosHeader~%arbePcFrameHeader PcHeader~%arbePcMetadata PcMetadata~%arbePcBins Points~%~%================================================================================~%MSG: arbe_msgs/arbePcFrameHeader~%uint64 time~%uint16 frame_counter~%uint8  is_4d~%uint8  frame_type~%uint16 udp_lost_packages~%uint32 number_of_points~%uint16 crd_count~%arbeFrameTypes frame_types~%~%================================================================================~%MSG: arbe_msgs/arbeFrameTypes~%uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%================================================================================~%MSG: arbe_msgs/arbePcMetadata~%arbePcRes PcResolution~%float32 range_offset~%float32 range_res_in_db~%uint32 azimuth_fft_size~%uint32 elevation_fft_size~%uint32 azimuth_fft_padding~%uint32 elevation_fft_padding~%~%~%================================================================================~%MSG: arbe_msgs/arbePcRes~%float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%================================================================================~%MSG: arbe_msgs/arbePcBins~%uint16[] range_bin~%int16[] doppler_signed_bin~%int8[] azimuth_signed_bin~%int8[] elevation_signed_bin~%int16[] phase~%int16[] power~%~%~%~%================================================================================~%MSG: arbe_msgs/arbeFusionResponse~%bool valid~%uint16 obj_id~%uint32 query_id~%uint8 fusionClass~%uint16 obj_index~%uint64 queryTime~%float32 conf~%uint8 tta_count~%float32[] tta_scores~%string class_str~%arbeFusionClasses fusionClasses~%~%uint16  top~%uint16  left~%uint16  bottom~%uint16  right~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeClassificationTrainingPc)))
  "Returns full string definition for message of type 'arbeClassificationTrainingPc"
  (cl:format cl:nil "std_msgs/Header Header~%arbeNewPcMsg Pc~%arbeFusionResponse fusionResponse~%uint16[] matching_com_indices~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeNewPcMsg~%std_msgs/Header RosHeader~%arbePcFrameHeader PcHeader~%arbePcMetadata PcMetadata~%arbePcBins Points~%~%================================================================================~%MSG: arbe_msgs/arbePcFrameHeader~%uint64 time~%uint16 frame_counter~%uint8  is_4d~%uint8  frame_type~%uint16 udp_lost_packages~%uint32 number_of_points~%uint16 crd_count~%arbeFrameTypes frame_types~%~%================================================================================~%MSG: arbe_msgs/arbeFrameTypes~%uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%================================================================================~%MSG: arbe_msgs/arbePcMetadata~%arbePcRes PcResolution~%float32 range_offset~%float32 range_res_in_db~%uint32 azimuth_fft_size~%uint32 elevation_fft_size~%uint32 azimuth_fft_padding~%uint32 elevation_fft_padding~%~%~%================================================================================~%MSG: arbe_msgs/arbePcRes~%float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%================================================================================~%MSG: arbe_msgs/arbePcBins~%uint16[] range_bin~%int16[] doppler_signed_bin~%int8[] azimuth_signed_bin~%int8[] elevation_signed_bin~%int16[] phase~%int16[] power~%~%~%~%================================================================================~%MSG: arbe_msgs/arbeFusionResponse~%bool valid~%uint16 obj_id~%uint32 query_id~%uint8 fusionClass~%uint16 obj_index~%uint64 queryTime~%float32 conf~%uint8 tta_count~%float32[] tta_scores~%string class_str~%arbeFusionClasses fusionClasses~%~%uint16  top~%uint16  left~%uint16  bottom~%uint16  right~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeClassificationTrainingPc>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'Header))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'Pc))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'fusionResponse))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'matching_com_indices) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 2)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeClassificationTrainingPc>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeClassificationTrainingPc
    (cl:cons ':Header (Header msg))
    (cl:cons ':Pc (Pc msg))
    (cl:cons ':fusionResponse (fusionResponse msg))
    (cl:cons ':matching_com_indices (matching_com_indices msg))
))
