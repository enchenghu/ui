; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeTAlgoDetectorThrInfo.msg.html

(cl:defclass <arbeTAlgoDetectorThrInfo> (roslisp-msg-protocol:ros-message)
  ((unFrameNumber
    :reader unFrameNumber
    :initarg :unFrameNumber
    :type cl:integer
    :initform 0)
   (eAlgoOutput
    :reader eAlgoOutput
    :initarg :eAlgoOutput
    :type cl:fixnum
    :initform 0)
   (eThresholdType
    :reader eThresholdType
    :initarg :eThresholdType
    :type cl:fixnum
    :initform 0)
   (eFrameType
    :reader eFrameType
    :initarg :eFrameType
    :type cl:fixnum
    :initform 0)
   (Data
    :reader Data
    :initarg :Data
    :type (cl:vector cl:integer)
   :initform (cl:make-array 0 :element-type 'cl:integer :initial-element 0)))
)

(cl:defclass arbeTAlgoDetectorThrInfo (<arbeTAlgoDetectorThrInfo>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeTAlgoDetectorThrInfo>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeTAlgoDetectorThrInfo)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeTAlgoDetectorThrInfo> is deprecated: use arbe_msgs-msg:arbeTAlgoDetectorThrInfo instead.")))

(cl:ensure-generic-function 'unFrameNumber-val :lambda-list '(m))
(cl:defmethod unFrameNumber-val ((m <arbeTAlgoDetectorThrInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:unFrameNumber-val is deprecated.  Use arbe_msgs-msg:unFrameNumber instead.")
  (unFrameNumber m))

(cl:ensure-generic-function 'eAlgoOutput-val :lambda-list '(m))
(cl:defmethod eAlgoOutput-val ((m <arbeTAlgoDetectorThrInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:eAlgoOutput-val is deprecated.  Use arbe_msgs-msg:eAlgoOutput instead.")
  (eAlgoOutput m))

(cl:ensure-generic-function 'eThresholdType-val :lambda-list '(m))
(cl:defmethod eThresholdType-val ((m <arbeTAlgoDetectorThrInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:eThresholdType-val is deprecated.  Use arbe_msgs-msg:eThresholdType instead.")
  (eThresholdType m))

(cl:ensure-generic-function 'eFrameType-val :lambda-list '(m))
(cl:defmethod eFrameType-val ((m <arbeTAlgoDetectorThrInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:eFrameType-val is deprecated.  Use arbe_msgs-msg:eFrameType instead.")
  (eFrameType m))

(cl:ensure-generic-function 'Data-val :lambda-list '(m))
(cl:defmethod Data-val ((m <arbeTAlgoDetectorThrInfo>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:Data-val is deprecated.  Use arbe_msgs-msg:Data instead.")
  (Data m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeTAlgoDetectorThrInfo>) ostream)
  "Serializes a message object of type '<arbeTAlgoDetectorThrInfo>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'unFrameNumber)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'unFrameNumber)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'unFrameNumber)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'unFrameNumber)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'eAlgoOutput)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'eThresholdType)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'eFrameType)) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'Data))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:write-byte (cl:ldb (cl:byte 8 0) ele) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) ele) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) ele) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) ele) ostream))
   (cl:slot-value msg 'Data))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeTAlgoDetectorThrInfo>) istream)
  "Deserializes a message object of type '<arbeTAlgoDetectorThrInfo>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'unFrameNumber)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'unFrameNumber)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'unFrameNumber)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'unFrameNumber)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'eAlgoOutput)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'eThresholdType)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'eFrameType)) (cl:read-byte istream))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'Data) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'Data)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:aref vals i)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:aref vals i)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:aref vals i)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:aref vals i)) (cl:read-byte istream)))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeTAlgoDetectorThrInfo>)))
  "Returns string type for a message object of type '<arbeTAlgoDetectorThrInfo>"
  "arbe_msgs/arbeTAlgoDetectorThrInfo")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeTAlgoDetectorThrInfo)))
  "Returns string type for a message object of type 'arbeTAlgoDetectorThrInfo"
  "arbe_msgs/arbeTAlgoDetectorThrInfo")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeTAlgoDetectorThrInfo>)))
  "Returns md5sum for a message object of type '<arbeTAlgoDetectorThrInfo>"
  "5f9f71c2214552c0e7f4ddc765c4621d")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeTAlgoDetectorThrInfo)))
  "Returns md5sum for a message object of type 'arbeTAlgoDetectorThrInfo"
  "5f9f71c2214552c0e7f4ddc765c4621d")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeTAlgoDetectorThrInfo>)))
  "Returns full string definition for message of type '<arbeTAlgoDetectorThrInfo>"
  (cl:format cl:nil "uint32 unFrameNumber~%uint8 eAlgoOutput~%uint8 eThresholdType~%uint8 eFrameType~%uint32[] Data~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeTAlgoDetectorThrInfo)))
  "Returns full string definition for message of type 'arbeTAlgoDetectorThrInfo"
  (cl:format cl:nil "uint32 unFrameNumber~%uint8 eAlgoOutput~%uint8 eThresholdType~%uint8 eFrameType~%uint32[] Data~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeTAlgoDetectorThrInfo>))
  (cl:+ 0
     4
     1
     1
     1
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'Data) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeTAlgoDetectorThrInfo>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeTAlgoDetectorThrInfo
    (cl:cons ':unFrameNumber (unFrameNumber msg))
    (cl:cons ':eAlgoOutput (eAlgoOutput msg))
    (cl:cons ':eThresholdType (eThresholdType msg))
    (cl:cons ':eFrameType (eFrameType msg))
    (cl:cons ':Data (Data msg))
))
