; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbePcRes.msg.html

(cl:defclass <arbePcRes> (roslisp-msg-protocol:ros-message)
  ((range_resolution
    :reader range_resolution
    :initarg :range_resolution
    :type cl:float
    :initform 0.0)
   (doppler_resolution
    :reader doppler_resolution
    :initarg :doppler_resolution
    :type cl:float
    :initform 0.0)
   (azimuth_coefficient
    :reader azimuth_coefficient
    :initarg :azimuth_coefficient
    :type cl:float
    :initform 0.0)
   (elevation_coefficient
    :reader elevation_coefficient
    :initarg :elevation_coefficient
    :type cl:float
    :initform 0.0))
)

(cl:defclass arbePcRes (<arbePcRes>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbePcRes>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbePcRes)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbePcRes> is deprecated: use arbe_msgs-msg:arbePcRes instead.")))

(cl:ensure-generic-function 'range_resolution-val :lambda-list '(m))
(cl:defmethod range_resolution-val ((m <arbePcRes>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:range_resolution-val is deprecated.  Use arbe_msgs-msg:range_resolution instead.")
  (range_resolution m))

(cl:ensure-generic-function 'doppler_resolution-val :lambda-list '(m))
(cl:defmethod doppler_resolution-val ((m <arbePcRes>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:doppler_resolution-val is deprecated.  Use arbe_msgs-msg:doppler_resolution instead.")
  (doppler_resolution m))

(cl:ensure-generic-function 'azimuth_coefficient-val :lambda-list '(m))
(cl:defmethod azimuth_coefficient-val ((m <arbePcRes>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:azimuth_coefficient-val is deprecated.  Use arbe_msgs-msg:azimuth_coefficient instead.")
  (azimuth_coefficient m))

(cl:ensure-generic-function 'elevation_coefficient-val :lambda-list '(m))
(cl:defmethod elevation_coefficient-val ((m <arbePcRes>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:elevation_coefficient-val is deprecated.  Use arbe_msgs-msg:elevation_coefficient instead.")
  (elevation_coefficient m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbePcRes>) ostream)
  "Serializes a message object of type '<arbePcRes>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'range_resolution))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'doppler_resolution))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'azimuth_coefficient))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'elevation_coefficient))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbePcRes>) istream)
  "Deserializes a message object of type '<arbePcRes>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'range_resolution) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'doppler_resolution) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'azimuth_coefficient) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'elevation_coefficient) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbePcRes>)))
  "Returns string type for a message object of type '<arbePcRes>"
  "arbe_msgs/arbePcRes")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbePcRes)))
  "Returns string type for a message object of type 'arbePcRes"
  "arbe_msgs/arbePcRes")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbePcRes>)))
  "Returns md5sum for a message object of type '<arbePcRes>"
  "84103936b409bd62d8ea8aada9826bd5")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbePcRes)))
  "Returns md5sum for a message object of type 'arbePcRes"
  "84103936b409bd62d8ea8aada9826bd5")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbePcRes>)))
  "Returns full string definition for message of type '<arbePcRes>"
  (cl:format cl:nil "float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbePcRes)))
  "Returns full string definition for message of type 'arbePcRes"
  (cl:format cl:nil "float32 range_resolution~%float32 doppler_resolution~%float32 azimuth_coefficient~%float32 elevation_coefficient~%~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbePcRes>))
  (cl:+ 0
     4
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbePcRes>))
  "Converts a ROS message object to a list"
  (cl:list 'arbePcRes
    (cl:cons ':range_resolution (range_resolution msg))
    (cl:cons ':doppler_resolution (doppler_resolution msg))
    (cl:cons ':azimuth_coefficient (azimuth_coefficient msg))
    (cl:cons ':elevation_coefficient (elevation_coefficient msg))
))
