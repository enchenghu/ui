; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeThresholds.msg.html

(cl:defclass <arbeThresholds> (roslisp-msg-protocol:ros-message)
  ((Header
    :reader Header
    :initarg :Header
    :type (cl:vector cl:fixnum)
   :initform (cl:make-array 0 :element-type 'cl:fixnum :initial-element 0))
   (Info
    :reader Info
    :initarg :Info
    :type (cl:vector cl:fixnum)
   :initform (cl:make-array 0 :element-type 'cl:fixnum :initial-element 0)))
)

(cl:defclass arbeThresholds (<arbeThresholds>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeThresholds>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeThresholds)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeThresholds> is deprecated: use arbe_msgs-msg:arbeThresholds instead.")))

(cl:ensure-generic-function 'Header-val :lambda-list '(m))
(cl:defmethod Header-val ((m <arbeThresholds>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:Header-val is deprecated.  Use arbe_msgs-msg:Header instead.")
  (Header m))

(cl:ensure-generic-function 'Info-val :lambda-list '(m))
(cl:defmethod Info-val ((m <arbeThresholds>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:Info-val is deprecated.  Use arbe_msgs-msg:Info instead.")
  (Info m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeThresholds>) ostream)
  "Serializes a message object of type '<arbeThresholds>"
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'Header))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:write-byte (cl:ldb (cl:byte 8 0) ele) ostream))
   (cl:slot-value msg 'Header))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'Info))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:write-byte (cl:ldb (cl:byte 8 0) ele) ostream))
   (cl:slot-value msg 'Info))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeThresholds>) istream)
  "Deserializes a message object of type '<arbeThresholds>"
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'Header) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'Header)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:aref vals i)) (cl:read-byte istream)))))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'Info) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'Info)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:aref vals i)) (cl:read-byte istream)))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeThresholds>)))
  "Returns string type for a message object of type '<arbeThresholds>"
  "arbe_msgs/arbeThresholds")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeThresholds)))
  "Returns string type for a message object of type 'arbeThresholds"
  "arbe_msgs/arbeThresholds")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeThresholds>)))
  "Returns md5sum for a message object of type '<arbeThresholds>"
  "8d95b90f43b0c5ea15e47b0302160dcc")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeThresholds)))
  "Returns md5sum for a message object of type 'arbeThresholds"
  "8d95b90f43b0c5ea15e47b0302160dcc")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeThresholds>)))
  "Returns full string definition for message of type '<arbeThresholds>"
  (cl:format cl:nil "uint8[] Header~%uint8[] Info~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeThresholds)))
  "Returns full string definition for message of type 'arbeThresholds"
  (cl:format cl:nil "uint8[] Header~%uint8[] Info~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeThresholds>))
  (cl:+ 0
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'Header) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 1)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'Info) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 1)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeThresholds>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeThresholds
    (cl:cons ':Header (Header msg))
    (cl:cons ':Info (Info msg))
))
