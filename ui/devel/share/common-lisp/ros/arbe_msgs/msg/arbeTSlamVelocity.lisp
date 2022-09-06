; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeTSlamVelocity.msg.html

(cl:defclass <arbeTSlamVelocity> (roslisp-msg-protocol:ros-message)
  ((x_dot
    :reader x_dot
    :initarg :x_dot
    :type cl:float
    :initform 0.0)
   (y_dot
    :reader y_dot
    :initarg :y_dot
    :type cl:float
    :initform 0.0)
   (dx_dot
    :reader dx_dot
    :initarg :dx_dot
    :type cl:float
    :initform 0.0)
   (dy_dot
    :reader dy_dot
    :initarg :dy_dot
    :type cl:float
    :initform 0.0))
)

(cl:defclass arbeTSlamVelocity (<arbeTSlamVelocity>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeTSlamVelocity>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeTSlamVelocity)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeTSlamVelocity> is deprecated: use arbe_msgs-msg:arbeTSlamVelocity instead.")))

(cl:ensure-generic-function 'x_dot-val :lambda-list '(m))
(cl:defmethod x_dot-val ((m <arbeTSlamVelocity>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:x_dot-val is deprecated.  Use arbe_msgs-msg:x_dot instead.")
  (x_dot m))

(cl:ensure-generic-function 'y_dot-val :lambda-list '(m))
(cl:defmethod y_dot-val ((m <arbeTSlamVelocity>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:y_dot-val is deprecated.  Use arbe_msgs-msg:y_dot instead.")
  (y_dot m))

(cl:ensure-generic-function 'dx_dot-val :lambda-list '(m))
(cl:defmethod dx_dot-val ((m <arbeTSlamVelocity>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:dx_dot-val is deprecated.  Use arbe_msgs-msg:dx_dot instead.")
  (dx_dot m))

(cl:ensure-generic-function 'dy_dot-val :lambda-list '(m))
(cl:defmethod dy_dot-val ((m <arbeTSlamVelocity>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:dy_dot-val is deprecated.  Use arbe_msgs-msg:dy_dot instead.")
  (dy_dot m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeTSlamVelocity>) ostream)
  "Serializes a message object of type '<arbeTSlamVelocity>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'x_dot))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'y_dot))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'dx_dot))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'dy_dot))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeTSlamVelocity>) istream)
  "Deserializes a message object of type '<arbeTSlamVelocity>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'x_dot) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'y_dot) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'dx_dot) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'dy_dot) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeTSlamVelocity>)))
  "Returns string type for a message object of type '<arbeTSlamVelocity>"
  "arbe_msgs/arbeTSlamVelocity")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeTSlamVelocity)))
  "Returns string type for a message object of type 'arbeTSlamVelocity"
  "arbe_msgs/arbeTSlamVelocity")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeTSlamVelocity>)))
  "Returns md5sum for a message object of type '<arbeTSlamVelocity>"
  "01747869987f0c9150027780a275e95f")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeTSlamVelocity)))
  "Returns md5sum for a message object of type 'arbeTSlamVelocity"
  "01747869987f0c9150027780a275e95f")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeTSlamVelocity>)))
  "Returns full string definition for message of type '<arbeTSlamVelocity>"
  (cl:format cl:nil "float32     x_dot ~%float32     y_dot ~%float32      dx_dot ~%float32      dy_dot ~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeTSlamVelocity)))
  "Returns full string definition for message of type 'arbeTSlamVelocity"
  (cl:format cl:nil "float32     x_dot ~%float32     y_dot ~%float32      dx_dot ~%float32      dy_dot ~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeTSlamVelocity>))
  (cl:+ 0
     4
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeTSlamVelocity>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeTSlamVelocity
    (cl:cons ':x_dot (x_dot msg))
    (cl:cons ':y_dot (y_dot msg))
    (cl:cons ':dx_dot (dx_dot msg))
    (cl:cons ':dy_dot (dy_dot msg))
))
