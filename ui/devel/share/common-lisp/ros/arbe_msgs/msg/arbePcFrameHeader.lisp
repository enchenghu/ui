; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbePcFrameHeader.msg.html

(cl:defclass <arbePcFrameHeader> (roslisp-msg-protocol:ros-message)
  ((time
    :reader time
    :initarg :time
    :type cl:integer
    :initform 0)
   (frame_counter
    :reader frame_counter
    :initarg :frame_counter
    :type cl:fixnum
    :initform 0)
   (is_4d
    :reader is_4d
    :initarg :is_4d
    :type cl:fixnum
    :initform 0)
   (frame_type
    :reader frame_type
    :initarg :frame_type
    :type cl:fixnum
    :initform 0)
   (udp_lost_packages
    :reader udp_lost_packages
    :initarg :udp_lost_packages
    :type cl:fixnum
    :initform 0)
   (number_of_points
    :reader number_of_points
    :initarg :number_of_points
    :type cl:integer
    :initform 0)
   (crd_count
    :reader crd_count
    :initarg :crd_count
    :type cl:fixnum
    :initform 0)
   (frame_types
    :reader frame_types
    :initarg :frame_types
    :type arbe_msgs-msg:arbeFrameTypes
    :initform (cl:make-instance 'arbe_msgs-msg:arbeFrameTypes)))
)

(cl:defclass arbePcFrameHeader (<arbePcFrameHeader>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbePcFrameHeader>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbePcFrameHeader)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbePcFrameHeader> is deprecated: use arbe_msgs-msg:arbePcFrameHeader instead.")))

(cl:ensure-generic-function 'time-val :lambda-list '(m))
(cl:defmethod time-val ((m <arbePcFrameHeader>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:time-val is deprecated.  Use arbe_msgs-msg:time instead.")
  (time m))

(cl:ensure-generic-function 'frame_counter-val :lambda-list '(m))
(cl:defmethod frame_counter-val ((m <arbePcFrameHeader>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:frame_counter-val is deprecated.  Use arbe_msgs-msg:frame_counter instead.")
  (frame_counter m))

(cl:ensure-generic-function 'is_4d-val :lambda-list '(m))
(cl:defmethod is_4d-val ((m <arbePcFrameHeader>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:is_4d-val is deprecated.  Use arbe_msgs-msg:is_4d instead.")
  (is_4d m))

(cl:ensure-generic-function 'frame_type-val :lambda-list '(m))
(cl:defmethod frame_type-val ((m <arbePcFrameHeader>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:frame_type-val is deprecated.  Use arbe_msgs-msg:frame_type instead.")
  (frame_type m))

(cl:ensure-generic-function 'udp_lost_packages-val :lambda-list '(m))
(cl:defmethod udp_lost_packages-val ((m <arbePcFrameHeader>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:udp_lost_packages-val is deprecated.  Use arbe_msgs-msg:udp_lost_packages instead.")
  (udp_lost_packages m))

(cl:ensure-generic-function 'number_of_points-val :lambda-list '(m))
(cl:defmethod number_of_points-val ((m <arbePcFrameHeader>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:number_of_points-val is deprecated.  Use arbe_msgs-msg:number_of_points instead.")
  (number_of_points m))

(cl:ensure-generic-function 'crd_count-val :lambda-list '(m))
(cl:defmethod crd_count-val ((m <arbePcFrameHeader>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:crd_count-val is deprecated.  Use arbe_msgs-msg:crd_count instead.")
  (crd_count m))

(cl:ensure-generic-function 'frame_types-val :lambda-list '(m))
(cl:defmethod frame_types-val ((m <arbePcFrameHeader>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:frame_types-val is deprecated.  Use arbe_msgs-msg:frame_types instead.")
  (frame_types m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbePcFrameHeader>) ostream)
  "Serializes a message object of type '<arbePcFrameHeader>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_counter)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_counter)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'is_4d)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_type)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'udp_lost_packages)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'udp_lost_packages)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'number_of_points)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'number_of_points)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'number_of_points)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'number_of_points)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'crd_count)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'crd_count)) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'frame_types) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbePcFrameHeader>) istream)
  "Deserializes a message object of type '<arbePcFrameHeader>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_counter)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_counter)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'is_4d)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_type)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'udp_lost_packages)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'udp_lost_packages)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'number_of_points)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'number_of_points)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'number_of_points)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'number_of_points)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'crd_count)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'crd_count)) (cl:read-byte istream))
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'frame_types) istream)
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbePcFrameHeader>)))
  "Returns string type for a message object of type '<arbePcFrameHeader>"
  "arbe_msgs/arbePcFrameHeader")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbePcFrameHeader)))
  "Returns string type for a message object of type 'arbePcFrameHeader"
  "arbe_msgs/arbePcFrameHeader")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbePcFrameHeader>)))
  "Returns md5sum for a message object of type '<arbePcFrameHeader>"
  "ea7e8a42429d20d4636665fbdb9eab60")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbePcFrameHeader)))
  "Returns md5sum for a message object of type 'arbePcFrameHeader"
  "ea7e8a42429d20d4636665fbdb9eab60")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbePcFrameHeader>)))
  "Returns full string definition for message of type '<arbePcFrameHeader>"
  (cl:format cl:nil "uint64 time~%uint16 frame_counter~%uint8  is_4d~%uint8  frame_type~%uint16 udp_lost_packages~%uint32 number_of_points~%uint16 crd_count~%arbeFrameTypes frame_types~%~%================================================================================~%MSG: arbe_msgs/arbeFrameTypes~%uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbePcFrameHeader)))
  "Returns full string definition for message of type 'arbePcFrameHeader"
  (cl:format cl:nil "uint64 time~%uint16 frame_counter~%uint8  is_4d~%uint8  frame_type~%uint16 udp_lost_packages~%uint32 number_of_points~%uint16 crd_count~%arbeFrameTypes frame_types~%~%================================================================================~%MSG: arbe_msgs/arbeFrameTypes~%uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbePcFrameHeader>))
  (cl:+ 0
     8
     2
     1
     1
     2
     4
     2
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'frame_types))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbePcFrameHeader>))
  "Converts a ROS message object to a list"
  (cl:list 'arbePcFrameHeader
    (cl:cons ':time (time msg))
    (cl:cons ':frame_counter (frame_counter msg))
    (cl:cons ':is_4d (is_4d msg))
    (cl:cons ':frame_type (frame_type msg))
    (cl:cons ':udp_lost_packages (udp_lost_packages msg))
    (cl:cons ':number_of_points (number_of_points msg))
    (cl:cons ':crd_count (crd_count msg))
    (cl:cons ':frame_types (frame_types msg))
))
