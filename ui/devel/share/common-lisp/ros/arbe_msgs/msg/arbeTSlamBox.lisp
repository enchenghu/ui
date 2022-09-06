; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeTSlamBox.msg.html

(cl:defclass <arbeTSlamBox> (roslisp-msg-protocol:ros-message)
  ((scale_x
    :reader scale_x
    :initarg :scale_x
    :type cl:float
    :initform 0.0)
   (scale_y
    :reader scale_y
    :initarg :scale_y
    :type cl:float
    :initform 0.0)
   (scale_z
    :reader scale_z
    :initarg :scale_z
    :type cl:float
    :initform 0.0)
   (scale_x_unc
    :reader scale_x_unc
    :initarg :scale_x_unc
    :type cl:float
    :initform 0.0)
   (scale_y_unc
    :reader scale_y_unc
    :initarg :scale_y_unc
    :type cl:float
    :initform 0.0)
   (scale_z_unc
    :reader scale_z_unc
    :initarg :scale_z_unc
    :type cl:float
    :initform 0.0)
   (orientation_unc
    :reader orientation_unc
    :initarg :orientation_unc
    :type cl:float
    :initform 0.0)
   (orientation
    :reader orientation
    :initarg :orientation
    :type cl:float
    :initform 0.0))
)

(cl:defclass arbeTSlamBox (<arbeTSlamBox>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeTSlamBox>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeTSlamBox)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeTSlamBox> is deprecated: use arbe_msgs-msg:arbeTSlamBox instead.")))

(cl:ensure-generic-function 'scale_x-val :lambda-list '(m))
(cl:defmethod scale_x-val ((m <arbeTSlamBox>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:scale_x-val is deprecated.  Use arbe_msgs-msg:scale_x instead.")
  (scale_x m))

(cl:ensure-generic-function 'scale_y-val :lambda-list '(m))
(cl:defmethod scale_y-val ((m <arbeTSlamBox>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:scale_y-val is deprecated.  Use arbe_msgs-msg:scale_y instead.")
  (scale_y m))

(cl:ensure-generic-function 'scale_z-val :lambda-list '(m))
(cl:defmethod scale_z-val ((m <arbeTSlamBox>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:scale_z-val is deprecated.  Use arbe_msgs-msg:scale_z instead.")
  (scale_z m))

(cl:ensure-generic-function 'scale_x_unc-val :lambda-list '(m))
(cl:defmethod scale_x_unc-val ((m <arbeTSlamBox>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:scale_x_unc-val is deprecated.  Use arbe_msgs-msg:scale_x_unc instead.")
  (scale_x_unc m))

(cl:ensure-generic-function 'scale_y_unc-val :lambda-list '(m))
(cl:defmethod scale_y_unc-val ((m <arbeTSlamBox>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:scale_y_unc-val is deprecated.  Use arbe_msgs-msg:scale_y_unc instead.")
  (scale_y_unc m))

(cl:ensure-generic-function 'scale_z_unc-val :lambda-list '(m))
(cl:defmethod scale_z_unc-val ((m <arbeTSlamBox>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:scale_z_unc-val is deprecated.  Use arbe_msgs-msg:scale_z_unc instead.")
  (scale_z_unc m))

(cl:ensure-generic-function 'orientation_unc-val :lambda-list '(m))
(cl:defmethod orientation_unc-val ((m <arbeTSlamBox>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:orientation_unc-val is deprecated.  Use arbe_msgs-msg:orientation_unc instead.")
  (orientation_unc m))

(cl:ensure-generic-function 'orientation-val :lambda-list '(m))
(cl:defmethod orientation-val ((m <arbeTSlamBox>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:orientation-val is deprecated.  Use arbe_msgs-msg:orientation instead.")
  (orientation m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeTSlamBox>) ostream)
  "Serializes a message object of type '<arbeTSlamBox>"
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'scale_x))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'scale_y))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'scale_z))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'scale_x_unc))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'scale_y_unc))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'scale_z_unc))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'orientation_unc))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'orientation))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeTSlamBox>) istream)
  "Deserializes a message object of type '<arbeTSlamBox>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'scale_x) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'scale_y) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'scale_z) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'scale_x_unc) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'scale_y_unc) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'scale_z_unc) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'orientation_unc) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'orientation) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeTSlamBox>)))
  "Returns string type for a message object of type '<arbeTSlamBox>"
  "arbe_msgs/arbeTSlamBox")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeTSlamBox)))
  "Returns string type for a message object of type 'arbeTSlamBox"
  "arbe_msgs/arbeTSlamBox")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeTSlamBox>)))
  "Returns md5sum for a message object of type '<arbeTSlamBox>"
  "e36bb424db9b07f679bc8512896c93f7")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeTSlamBox)))
  "Returns md5sum for a message object of type 'arbeTSlamBox"
  "e36bb424db9b07f679bc8512896c93f7")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeTSlamBox>)))
  "Returns full string definition for message of type '<arbeTSlamBox>"
  (cl:format cl:nil "float32 scale_x     ~%float32 scale_y     ~%float32 scale_z     ~%float32 scale_x_unc ~%float32 scale_y_unc ~%float32 scale_z_unc ~%float32 orientation_unc  ~%float32 orientation  ~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeTSlamBox)))
  "Returns full string definition for message of type 'arbeTSlamBox"
  (cl:format cl:nil "float32 scale_x     ~%float32 scale_y     ~%float32 scale_z     ~%float32 scale_x_unc ~%float32 scale_y_unc ~%float32 scale_z_unc ~%float32 orientation_unc  ~%float32 orientation  ~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeTSlamBox>))
  (cl:+ 0
     4
     4
     4
     4
     4
     4
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeTSlamBox>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeTSlamBox
    (cl:cons ':scale_x (scale_x msg))
    (cl:cons ':scale_y (scale_y msg))
    (cl:cons ':scale_z (scale_z msg))
    (cl:cons ':scale_x_unc (scale_x_unc msg))
    (cl:cons ':scale_y_unc (scale_y_unc msg))
    (cl:cons ':scale_z_unc (scale_z_unc msg))
    (cl:cons ':orientation_unc (orientation_unc msg))
    (cl:cons ':orientation (orientation msg))
))
