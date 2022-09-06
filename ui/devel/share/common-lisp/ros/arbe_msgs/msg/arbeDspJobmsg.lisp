; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeDspJobmsg.msg.html

(cl:defclass <arbeDspJobmsg> (roslisp-msg-protocol:ros-message)
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

(cl:defclass arbeDspJobmsg (<arbeDspJobmsg>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeDspJobmsg>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeDspJobmsg)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeDspJobmsg> is deprecated: use arbe_msgs-msg:arbeDspJobmsg instead.")))

(cl:ensure-generic-function 'Header-val :lambda-list '(m))
(cl:defmethod Header-val ((m <arbeDspJobmsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:Header-val is deprecated.  Use arbe_msgs-msg:Header instead.")
  (Header m))

(cl:ensure-generic-function 'Info-val :lambda-list '(m))
(cl:defmethod Info-val ((m <arbeDspJobmsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:Info-val is deprecated.  Use arbe_msgs-msg:Info instead.")
  (Info m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeDspJobmsg>) ostream)
  "Serializes a message object of type '<arbeDspJobmsg>"
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
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeDspJobmsg>) istream)
  "Deserializes a message object of type '<arbeDspJobmsg>"
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
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeDspJobmsg>)))
  "Returns string type for a message object of type '<arbeDspJobmsg>"
  "arbe_msgs/arbeDspJobmsg")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeDspJobmsg)))
  "Returns string type for a message object of type 'arbeDspJobmsg"
  "arbe_msgs/arbeDspJobmsg")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeDspJobmsg>)))
  "Returns md5sum for a message object of type '<arbeDspJobmsg>"
  "8d95b90f43b0c5ea15e47b0302160dcc")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeDspJobmsg)))
  "Returns md5sum for a message object of type 'arbeDspJobmsg"
  "8d95b90f43b0c5ea15e47b0302160dcc")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeDspJobmsg>)))
  "Returns full string definition for message of type '<arbeDspJobmsg>"
  (cl:format cl:nil "uint8[] Header~%uint8[] Info~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeDspJobmsg)))
  "Returns full string definition for message of type 'arbeDspJobmsg"
  (cl:format cl:nil "uint8[] Header~%uint8[] Info~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeDspJobmsg>))
  (cl:+ 0
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'Header) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 1)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'Info) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 1)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeDspJobmsg>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeDspJobmsg
    (cl:cons ':Header (Header msg))
    (cl:cons ':Info (Info msg))
))
