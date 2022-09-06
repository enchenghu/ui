; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbePcWithValid.msg.html

(cl:defclass <arbePcWithValid> (roslisp-msg-protocol:ros-message)
  ((pc
    :reader pc
    :initarg :pc
    :type arbe_msgs-msg:arbeNewPcMsg
    :initform (cl:make-instance 'arbe_msgs-msg:arbeNewPcMsg))
   (valid
    :reader valid
    :initarg :valid
    :type (cl:vector cl:boolean)
   :initform (cl:make-array 0 :element-type 'cl:boolean :initial-element cl:nil)))
)

(cl:defclass arbePcWithValid (<arbePcWithValid>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbePcWithValid>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbePcWithValid)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbePcWithValid> is deprecated: use arbe_msgs-msg:arbePcWithValid instead.")))

(cl:ensure-generic-function 'pc-val :lambda-list '(m))
(cl:defmethod pc-val ((m <arbePcWithValid>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:pc-val is deprecated.  Use arbe_msgs-msg:pc instead.")
  (pc m))

(cl:ensure-generic-function 'valid-val :lambda-list '(m))
(cl:defmethod valid-val ((m <arbePcWithValid>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:valid-val is deprecated.  Use arbe_msgs-msg:valid instead.")
  (valid m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbePcWithValid>) ostream)
  "Serializes a message object of type '<arbePcWithValid>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'pc) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'valid))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if ele 1 0)) ostream))
   (cl:slot-value msg 'valid))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbePcWithValid>) istream)
  "Deserializes a message object of type '<arbePcWithValid>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'pc) istream)
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'valid) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'valid)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:aref vals i) (cl:not (cl:zerop (cl:read-byte istream)))))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbePcWithValid>)))
  "Returns string type for a message object of type '<arbePcWithValid>"
  "arbe_msgs/arbePcWithValid")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbePcWithValid)))
  "Returns string type for a message object of type 'arbePcWithValid"
  "arbe_msgs/arbePcWithValid")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbePcWithValid>)))
  "Returns md5sum for a message object of type '<arbePcWithValid>"
  "e4ea8e0ece0df084b29db472b924eebf")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbePcWithValid)))
  "Returns md5sum for a message object of type 'arbePcWithValid"
  "e4ea8e0ece0df084b29db472b924eebf")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbePcWithValid>)))
  "Returns full string definition for message of type '<arbePcWithValid>"
  (cl:format cl:nil "arbeNewPcMsg pc~%bool[] valid~%~%================================================================================~%MSG: arbe_msgs/arbeNewPcMsg~%std_msgs/Header RosHeader~%arbePcFrameHeader PcHeader~%arbePcMetadata PcMetadata~%arbePcBins Points~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbePcFrameHeader~%uint64 time~%uint16 frame_counter~%uint8  is_4d~%uint8  frame_type~%uint16 udp_lost_packages~%uint32 number_of_points~%uint16 crd_count~%arbeFrameTypes frame_types~%~%================================================================================~%MSG: arbe_msgs/arbeFrameTypes~%uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%================================================================================~%MSG: arbe_msgs/arbePcMetadata~%arbePcRes PcResolution~%float32 range_offset~%float32 range_res_in_db~%uint32 azimuth_fft_size~%uint32 elevation_fft_size~%uint32 azimuth_fft_padding~%uint32 elevation_fft_padding~%~%~%================================================================================~%MSG: arbe_msgs/arbePcRes~%float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%================================================================================~%MSG: arbe_msgs/arbePcBins~%uint16[] range_bin~%int16[] doppler_signed_bin~%int8[] azimuth_signed_bin~%int8[] elevation_signed_bin~%int16[] phase~%int16[] power~%~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbePcWithValid)))
  "Returns full string definition for message of type 'arbePcWithValid"
  (cl:format cl:nil "arbeNewPcMsg pc~%bool[] valid~%~%================================================================================~%MSG: arbe_msgs/arbeNewPcMsg~%std_msgs/Header RosHeader~%arbePcFrameHeader PcHeader~%arbePcMetadata PcMetadata~%arbePcBins Points~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbePcFrameHeader~%uint64 time~%uint16 frame_counter~%uint8  is_4d~%uint8  frame_type~%uint16 udp_lost_packages~%uint32 number_of_points~%uint16 crd_count~%arbeFrameTypes frame_types~%~%================================================================================~%MSG: arbe_msgs/arbeFrameTypes~%uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%================================================================================~%MSG: arbe_msgs/arbePcMetadata~%arbePcRes PcResolution~%float32 range_offset~%float32 range_res_in_db~%uint32 azimuth_fft_size~%uint32 elevation_fft_size~%uint32 azimuth_fft_padding~%uint32 elevation_fft_padding~%~%~%================================================================================~%MSG: arbe_msgs/arbePcRes~%float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%================================================================================~%MSG: arbe_msgs/arbePcBins~%uint16[] range_bin~%int16[] doppler_signed_bin~%int8[] azimuth_signed_bin~%int8[] elevation_signed_bin~%int16[] phase~%int16[] power~%~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbePcWithValid>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'pc))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'valid) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 1)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbePcWithValid>))
  "Converts a ROS message object to a list"
  (cl:list 'arbePcWithValid
    (cl:cons ':pc (pc msg))
    (cl:cons ':valid (valid msg))
))
