; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeTSlamMetadata.msg.html

(cl:defclass <arbeTSlamMetadata> (roslisp-msg-protocol:ros-message)
  ((CorresPcTime
    :reader CorresPcTime
    :initarg :CorresPcTime
    :type cl:integer
    :initform 0)
   (FrameNumber
    :reader FrameNumber
    :initarg :FrameNumber
    :type cl:fixnum
    :initform 0)
   (HostHeading
    :reader HostHeading
    :initarg :HostHeading
    :type cl:float
    :initform 0.0)
   (HostHeadingUnc
    :reader HostHeadingUnc
    :initarg :HostHeadingUnc
    :type cl:float
    :initform 0.0)
   (HostVelocity
    :reader HostVelocity
    :initarg :HostVelocity
    :type cl:float
    :initform 0.0)
   (HostVelocityUnc
    :reader HostVelocityUnc
    :initarg :HostVelocityUnc
    :type cl:float
    :initform 0.0)
   (HostOmega
    :reader HostOmega
    :initarg :HostOmega
    :type cl:float
    :initform 0.0)
   (HostOmegaUnc
    :reader HostOmegaUnc
    :initarg :HostOmegaUnc
    :type cl:float
    :initform 0.0)
   (NumberOfObjects
    :reader NumberOfObjects
    :initarg :NumberOfObjects
    :type cl:fixnum
    :initform 0)
   (local_catr_x
    :reader local_catr_x
    :initarg :local_catr_x
    :type cl:float
    :initform 0.0)
   (local_catr_y
    :reader local_catr_y
    :initarg :local_catr_y
    :type cl:float
    :initform 0.0)
   (location_valid
    :reader location_valid
    :initarg :location_valid
    :type cl:boolean
    :initform cl:nil)
   (is_reset_geo_location
    :reader is_reset_geo_location
    :initarg :is_reset_geo_location
    :type cl:boolean
    :initform cl:nil))
)

(cl:defclass arbeTSlamMetadata (<arbeTSlamMetadata>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeTSlamMetadata>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeTSlamMetadata)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeTSlamMetadata> is deprecated: use arbe_msgs-msg:arbeTSlamMetadata instead.")))

(cl:ensure-generic-function 'CorresPcTime-val :lambda-list '(m))
(cl:defmethod CorresPcTime-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:CorresPcTime-val is deprecated.  Use arbe_msgs-msg:CorresPcTime instead.")
  (CorresPcTime m))

(cl:ensure-generic-function 'FrameNumber-val :lambda-list '(m))
(cl:defmethod FrameNumber-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:FrameNumber-val is deprecated.  Use arbe_msgs-msg:FrameNumber instead.")
  (FrameNumber m))

(cl:ensure-generic-function 'HostHeading-val :lambda-list '(m))
(cl:defmethod HostHeading-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:HostHeading-val is deprecated.  Use arbe_msgs-msg:HostHeading instead.")
  (HostHeading m))

(cl:ensure-generic-function 'HostHeadingUnc-val :lambda-list '(m))
(cl:defmethod HostHeadingUnc-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:HostHeadingUnc-val is deprecated.  Use arbe_msgs-msg:HostHeadingUnc instead.")
  (HostHeadingUnc m))

(cl:ensure-generic-function 'HostVelocity-val :lambda-list '(m))
(cl:defmethod HostVelocity-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:HostVelocity-val is deprecated.  Use arbe_msgs-msg:HostVelocity instead.")
  (HostVelocity m))

(cl:ensure-generic-function 'HostVelocityUnc-val :lambda-list '(m))
(cl:defmethod HostVelocityUnc-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:HostVelocityUnc-val is deprecated.  Use arbe_msgs-msg:HostVelocityUnc instead.")
  (HostVelocityUnc m))

(cl:ensure-generic-function 'HostOmega-val :lambda-list '(m))
(cl:defmethod HostOmega-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:HostOmega-val is deprecated.  Use arbe_msgs-msg:HostOmega instead.")
  (HostOmega m))

(cl:ensure-generic-function 'HostOmegaUnc-val :lambda-list '(m))
(cl:defmethod HostOmegaUnc-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:HostOmegaUnc-val is deprecated.  Use arbe_msgs-msg:HostOmegaUnc instead.")
  (HostOmegaUnc m))

(cl:ensure-generic-function 'NumberOfObjects-val :lambda-list '(m))
(cl:defmethod NumberOfObjects-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:NumberOfObjects-val is deprecated.  Use arbe_msgs-msg:NumberOfObjects instead.")
  (NumberOfObjects m))

(cl:ensure-generic-function 'local_catr_x-val :lambda-list '(m))
(cl:defmethod local_catr_x-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:local_catr_x-val is deprecated.  Use arbe_msgs-msg:local_catr_x instead.")
  (local_catr_x m))

(cl:ensure-generic-function 'local_catr_y-val :lambda-list '(m))
(cl:defmethod local_catr_y-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:local_catr_y-val is deprecated.  Use arbe_msgs-msg:local_catr_y instead.")
  (local_catr_y m))

(cl:ensure-generic-function 'location_valid-val :lambda-list '(m))
(cl:defmethod location_valid-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:location_valid-val is deprecated.  Use arbe_msgs-msg:location_valid instead.")
  (location_valid m))

(cl:ensure-generic-function 'is_reset_geo_location-val :lambda-list '(m))
(cl:defmethod is_reset_geo_location-val ((m <arbeTSlamMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:is_reset_geo_location-val is deprecated.  Use arbe_msgs-msg:is_reset_geo_location instead.")
  (is_reset_geo_location m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeTSlamMetadata>) ostream)
  "Serializes a message object of type '<arbeTSlamMetadata>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'CorresPcTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'CorresPcTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'CorresPcTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'CorresPcTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'CorresPcTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'CorresPcTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'CorresPcTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'CorresPcTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'FrameNumber)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'FrameNumber)) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'HostHeading))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'HostHeadingUnc))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'HostVelocity))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'HostVelocityUnc))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'HostOmega))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'HostOmegaUnc))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'NumberOfObjects)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'NumberOfObjects)) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'local_catr_x))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'local_catr_y))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'location_valid) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'is_reset_geo_location) 1 0)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeTSlamMetadata>) istream)
  "Deserializes a message object of type '<arbeTSlamMetadata>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'CorresPcTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'CorresPcTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'CorresPcTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'CorresPcTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'CorresPcTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'CorresPcTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'CorresPcTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'CorresPcTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'FrameNumber)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'FrameNumber)) (cl:read-byte istream))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'HostHeading) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'HostHeadingUnc) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'HostVelocity) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'HostVelocityUnc) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'HostOmega) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'HostOmegaUnc) (roslisp-utils:decode-single-float-bits bits)))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'NumberOfObjects)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'NumberOfObjects)) (cl:read-byte istream))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'local_catr_x) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'local_catr_y) (roslisp-utils:decode-single-float-bits bits)))
    (cl:setf (cl:slot-value msg 'location_valid) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'is_reset_geo_location) (cl:not (cl:zerop (cl:read-byte istream))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeTSlamMetadata>)))
  "Returns string type for a message object of type '<arbeTSlamMetadata>"
  "arbe_msgs/arbeTSlamMetadata")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeTSlamMetadata)))
  "Returns string type for a message object of type 'arbeTSlamMetadata"
  "arbe_msgs/arbeTSlamMetadata")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeTSlamMetadata>)))
  "Returns md5sum for a message object of type '<arbeTSlamMetadata>"
  "9f419d3fbb87f9bdc08e8e38e6a62de8")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeTSlamMetadata)))
  "Returns md5sum for a message object of type 'arbeTSlamMetadata"
  "9f419d3fbb87f9bdc08e8e38e6a62de8")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeTSlamMetadata>)))
  "Returns full string definition for message of type '<arbeTSlamMetadata>"
  (cl:format cl:nil "uint64  CorresPcTime~%uint16  FrameNumber~%float32 HostHeading~%float32 HostHeadingUnc~%float32 HostVelocity~%float32 HostVelocityUnc~%float32 HostOmega		~%float32 HostOmegaUnc~%uint16  NumberOfObjects~%float32 local_catr_x~%float32 local_catr_y~%bool    location_valid~%bool    is_reset_geo_location~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeTSlamMetadata)))
  "Returns full string definition for message of type 'arbeTSlamMetadata"
  (cl:format cl:nil "uint64  CorresPcTime~%uint16  FrameNumber~%float32 HostHeading~%float32 HostHeadingUnc~%float32 HostVelocity~%float32 HostVelocityUnc~%float32 HostOmega		~%float32 HostOmegaUnc~%uint16  NumberOfObjects~%float32 local_catr_x~%float32 local_catr_y~%bool    location_valid~%bool    is_reset_geo_location~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeTSlamMetadata>))
  (cl:+ 0
     8
     2
     4
     4
     4
     4
     4
     4
     2
     4
     4
     1
     1
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeTSlamMetadata>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeTSlamMetadata
    (cl:cons ':CorresPcTime (CorresPcTime msg))
    (cl:cons ':FrameNumber (FrameNumber msg))
    (cl:cons ':HostHeading (HostHeading msg))
    (cl:cons ':HostHeadingUnc (HostHeadingUnc msg))
    (cl:cons ':HostVelocity (HostVelocity msg))
    (cl:cons ':HostVelocityUnc (HostVelocityUnc msg))
    (cl:cons ':HostOmega (HostOmega msg))
    (cl:cons ':HostOmegaUnc (HostOmegaUnc msg))
    (cl:cons ':NumberOfObjects (NumberOfObjects msg))
    (cl:cons ':local_catr_x (local_catr_x msg))
    (cl:cons ':local_catr_y (local_catr_y msg))
    (cl:cons ':location_valid (location_valid msg))
    (cl:cons ':is_reset_geo_location (is_reset_geo_location msg))
))
