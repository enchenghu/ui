; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbePcMetadata.msg.html

(cl:defclass <arbePcMetadata> (roslisp-msg-protocol:ros-message)
  ((PcResolution
    :reader PcResolution
    :initarg :PcResolution
    :type arbe_msgs-msg:arbePcRes
    :initform (cl:make-instance 'arbe_msgs-msg:arbePcRes))
   (range_offset
    :reader range_offset
    :initarg :range_offset
    :type cl:float
    :initform 0.0)
   (range_res_in_db
    :reader range_res_in_db
    :initarg :range_res_in_db
    :type cl:float
    :initform 0.0)
   (azimuth_fft_size
    :reader azimuth_fft_size
    :initarg :azimuth_fft_size
    :type cl:integer
    :initform 0)
   (elevation_fft_size
    :reader elevation_fft_size
    :initarg :elevation_fft_size
    :type cl:integer
    :initform 0)
   (azimuth_fft_padding
    :reader azimuth_fft_padding
    :initarg :azimuth_fft_padding
    :type cl:integer
    :initform 0)
   (elevation_fft_padding
    :reader elevation_fft_padding
    :initarg :elevation_fft_padding
    :type cl:integer
    :initform 0))
)

(cl:defclass arbePcMetadata (<arbePcMetadata>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbePcMetadata>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbePcMetadata)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbePcMetadata> is deprecated: use arbe_msgs-msg:arbePcMetadata instead.")))

(cl:ensure-generic-function 'PcResolution-val :lambda-list '(m))
(cl:defmethod PcResolution-val ((m <arbePcMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:PcResolution-val is deprecated.  Use arbe_msgs-msg:PcResolution instead.")
  (PcResolution m))

(cl:ensure-generic-function 'range_offset-val :lambda-list '(m))
(cl:defmethod range_offset-val ((m <arbePcMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:range_offset-val is deprecated.  Use arbe_msgs-msg:range_offset instead.")
  (range_offset m))

(cl:ensure-generic-function 'range_res_in_db-val :lambda-list '(m))
(cl:defmethod range_res_in_db-val ((m <arbePcMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:range_res_in_db-val is deprecated.  Use arbe_msgs-msg:range_res_in_db instead.")
  (range_res_in_db m))

(cl:ensure-generic-function 'azimuth_fft_size-val :lambda-list '(m))
(cl:defmethod azimuth_fft_size-val ((m <arbePcMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:azimuth_fft_size-val is deprecated.  Use arbe_msgs-msg:azimuth_fft_size instead.")
  (azimuth_fft_size m))

(cl:ensure-generic-function 'elevation_fft_size-val :lambda-list '(m))
(cl:defmethod elevation_fft_size-val ((m <arbePcMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:elevation_fft_size-val is deprecated.  Use arbe_msgs-msg:elevation_fft_size instead.")
  (elevation_fft_size m))

(cl:ensure-generic-function 'azimuth_fft_padding-val :lambda-list '(m))
(cl:defmethod azimuth_fft_padding-val ((m <arbePcMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:azimuth_fft_padding-val is deprecated.  Use arbe_msgs-msg:azimuth_fft_padding instead.")
  (azimuth_fft_padding m))

(cl:ensure-generic-function 'elevation_fft_padding-val :lambda-list '(m))
(cl:defmethod elevation_fft_padding-val ((m <arbePcMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:elevation_fft_padding-val is deprecated.  Use arbe_msgs-msg:elevation_fft_padding instead.")
  (elevation_fft_padding m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbePcMetadata>) ostream)
  "Serializes a message object of type '<arbePcMetadata>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'PcResolution) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'range_offset))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'range_res_in_db))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'azimuth_fft_size)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'azimuth_fft_size)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'azimuth_fft_size)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'azimuth_fft_size)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'elevation_fft_size)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'elevation_fft_size)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'elevation_fft_size)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'elevation_fft_size)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'azimuth_fft_padding)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'azimuth_fft_padding)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'azimuth_fft_padding)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'azimuth_fft_padding)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'elevation_fft_padding)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'elevation_fft_padding)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'elevation_fft_padding)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'elevation_fft_padding)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbePcMetadata>) istream)
  "Deserializes a message object of type '<arbePcMetadata>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'PcResolution) istream)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'range_offset) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'range_res_in_db) (roslisp-utils:decode-single-float-bits bits)))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'azimuth_fft_size)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'azimuth_fft_size)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'azimuth_fft_size)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'azimuth_fft_size)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'elevation_fft_size)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'elevation_fft_size)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'elevation_fft_size)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'elevation_fft_size)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'azimuth_fft_padding)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'azimuth_fft_padding)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'azimuth_fft_padding)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'azimuth_fft_padding)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'elevation_fft_padding)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'elevation_fft_padding)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'elevation_fft_padding)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'elevation_fft_padding)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbePcMetadata>)))
  "Returns string type for a message object of type '<arbePcMetadata>"
  "arbe_msgs/arbePcMetadata")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbePcMetadata)))
  "Returns string type for a message object of type 'arbePcMetadata"
  "arbe_msgs/arbePcMetadata")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbePcMetadata>)))
  "Returns md5sum for a message object of type '<arbePcMetadata>"
  "07014ddb83398e57a7856835fad2cfca")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbePcMetadata)))
  "Returns md5sum for a message object of type 'arbePcMetadata"
  "07014ddb83398e57a7856835fad2cfca")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbePcMetadata>)))
  "Returns full string definition for message of type '<arbePcMetadata>"
  (cl:format cl:nil "arbePcRes PcResolution~%float32 range_offset~%float32 range_res_in_db~%uint32 azimuth_fft_size~%uint32 elevation_fft_size~%uint32 azimuth_fft_padding~%uint32 elevation_fft_padding~%~%~%================================================================================~%MSG: arbe_msgs/arbePcRes~%float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbePcMetadata)))
  "Returns full string definition for message of type 'arbePcMetadata"
  (cl:format cl:nil "arbePcRes PcResolution~%float32 range_offset~%float32 range_res_in_db~%uint32 azimuth_fft_size~%uint32 elevation_fft_size~%uint32 azimuth_fft_padding~%uint32 elevation_fft_padding~%~%~%================================================================================~%MSG: arbe_msgs/arbePcRes~%float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbePcMetadata>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'PcResolution))
     4
     4
     4
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbePcMetadata>))
  "Converts a ROS message object to a list"
  (cl:list 'arbePcMetadata
    (cl:cons ':PcResolution (PcResolution msg))
    (cl:cons ':range_offset (range_offset msg))
    (cl:cons ':range_res_in_db (range_res_in_db msg))
    (cl:cons ':azimuth_fft_size (azimuth_fft_size msg))
    (cl:cons ':elevation_fft_size (elevation_fft_size msg))
    (cl:cons ':azimuth_fft_padding (azimuth_fft_padding msg))
    (cl:cons ':elevation_fft_padding (elevation_fft_padding msg))
))
