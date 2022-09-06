; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeNewPcMsg.msg.html

(cl:defclass <arbeNewPcMsg> (roslisp-msg-protocol:ros-message)
  ((RosHeader
    :reader RosHeader
    :initarg :RosHeader
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (PcHeader
    :reader PcHeader
    :initarg :PcHeader
    :type arbe_msgs-msg:arbePcFrameHeader
    :initform (cl:make-instance 'arbe_msgs-msg:arbePcFrameHeader))
   (PcMetadata
    :reader PcMetadata
    :initarg :PcMetadata
    :type arbe_msgs-msg:arbePcMetadata
    :initform (cl:make-instance 'arbe_msgs-msg:arbePcMetadata))
   (Points
    :reader Points
    :initarg :Points
    :type arbe_msgs-msg:arbePcBins
    :initform (cl:make-instance 'arbe_msgs-msg:arbePcBins)))
)

(cl:defclass arbeNewPcMsg (<arbeNewPcMsg>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeNewPcMsg>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeNewPcMsg)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeNewPcMsg> is deprecated: use arbe_msgs-msg:arbeNewPcMsg instead.")))

(cl:ensure-generic-function 'RosHeader-val :lambda-list '(m))
(cl:defmethod RosHeader-val ((m <arbeNewPcMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:RosHeader-val is deprecated.  Use arbe_msgs-msg:RosHeader instead.")
  (RosHeader m))

(cl:ensure-generic-function 'PcHeader-val :lambda-list '(m))
(cl:defmethod PcHeader-val ((m <arbeNewPcMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:PcHeader-val is deprecated.  Use arbe_msgs-msg:PcHeader instead.")
  (PcHeader m))

(cl:ensure-generic-function 'PcMetadata-val :lambda-list '(m))
(cl:defmethod PcMetadata-val ((m <arbeNewPcMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:PcMetadata-val is deprecated.  Use arbe_msgs-msg:PcMetadata instead.")
  (PcMetadata m))

(cl:ensure-generic-function 'Points-val :lambda-list '(m))
(cl:defmethod Points-val ((m <arbeNewPcMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:Points-val is deprecated.  Use arbe_msgs-msg:Points instead.")
  (Points m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeNewPcMsg>) ostream)
  "Serializes a message object of type '<arbeNewPcMsg>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'RosHeader) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'PcHeader) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'PcMetadata) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'Points) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeNewPcMsg>) istream)
  "Deserializes a message object of type '<arbeNewPcMsg>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'RosHeader) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'PcHeader) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'PcMetadata) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'Points) istream)
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeNewPcMsg>)))
  "Returns string type for a message object of type '<arbeNewPcMsg>"
  "arbe_msgs/arbeNewPcMsg")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeNewPcMsg)))
  "Returns string type for a message object of type 'arbeNewPcMsg"
  "arbe_msgs/arbeNewPcMsg")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeNewPcMsg>)))
  "Returns md5sum for a message object of type '<arbeNewPcMsg>"
  "6537ae358990354b634606bcfb0fa7db")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeNewPcMsg)))
  "Returns md5sum for a message object of type 'arbeNewPcMsg"
  "6537ae358990354b634606bcfb0fa7db")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeNewPcMsg>)))
  "Returns full string definition for message of type '<arbeNewPcMsg>"
  (cl:format cl:nil "std_msgs/Header RosHeader~%arbePcFrameHeader PcHeader~%arbePcMetadata PcMetadata~%arbePcBins Points~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbePcFrameHeader~%uint64 time~%uint16 frame_counter~%uint8  is_4d~%uint8  frame_type~%uint16 udp_lost_packages~%uint32 number_of_points~%uint16 crd_count~%arbeFrameTypes frame_types~%~%================================================================================~%MSG: arbe_msgs/arbeFrameTypes~%uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%================================================================================~%MSG: arbe_msgs/arbePcMetadata~%arbePcRes PcResolution~%float32 range_offset~%float32 range_res_in_db~%uint32 azimuth_fft_size~%uint32 elevation_fft_size~%uint32 azimuth_fft_padding~%uint32 elevation_fft_padding~%~%~%================================================================================~%MSG: arbe_msgs/arbePcRes~%float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%================================================================================~%MSG: arbe_msgs/arbePcBins~%uint16[] range_bin~%int16[] doppler_signed_bin~%int8[] azimuth_signed_bin~%int8[] elevation_signed_bin~%int16[] phase~%int16[] power~%~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeNewPcMsg)))
  "Returns full string definition for message of type 'arbeNewPcMsg"
  (cl:format cl:nil "std_msgs/Header RosHeader~%arbePcFrameHeader PcHeader~%arbePcMetadata PcMetadata~%arbePcBins Points~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbePcFrameHeader~%uint64 time~%uint16 frame_counter~%uint8  is_4d~%uint8  frame_type~%uint16 udp_lost_packages~%uint32 number_of_points~%uint16 crd_count~%arbeFrameTypes frame_types~%~%================================================================================~%MSG: arbe_msgs/arbeFrameTypes~%uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%================================================================================~%MSG: arbe_msgs/arbePcMetadata~%arbePcRes PcResolution~%float32 range_offset~%float32 range_res_in_db~%uint32 azimuth_fft_size~%uint32 elevation_fft_size~%uint32 azimuth_fft_padding~%uint32 elevation_fft_padding~%~%~%================================================================================~%MSG: arbe_msgs/arbePcRes~%float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%================================================================================~%MSG: arbe_msgs/arbePcBins~%uint16[] range_bin~%int16[] doppler_signed_bin~%int8[] azimuth_signed_bin~%int8[] elevation_signed_bin~%int16[] phase~%int16[] power~%~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeNewPcMsg>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'RosHeader))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'PcHeader))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'PcMetadata))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'Points))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeNewPcMsg>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeNewPcMsg
    (cl:cons ':RosHeader (RosHeader msg))
    (cl:cons ':PcHeader (PcHeader msg))
    (cl:cons ':PcMetadata (PcMetadata msg))
    (cl:cons ':Points (Points msg))
))
