; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeTSlamObj.msg.html

(cl:defclass <arbeTSlamObj> (roslisp-msg-protocol:ros-message)
  ((ID
    :reader ID
    :initarg :ID
    :type cl:fixnum
    :initform 0)
   (obj_class
    :reader obj_class
    :initarg :obj_class
    :type cl:fixnum
    :initform 0)
   (class_conf
    :reader class_conf
    :initarg :class_conf
    :type cl:float
    :initform 0.0)
   (position
    :reader position
    :initarg :position
    :type arbe_msgs-msg:arbeTSlamPos
    :initform (cl:make-instance 'arbe_msgs-msg:arbeTSlamPos))
   (velocity
    :reader velocity
    :initarg :velocity
    :type arbe_msgs-msg:arbeTSlamVelocity
    :initform (cl:make-instance 'arbe_msgs-msg:arbeTSlamVelocity))
   (bounding_box
    :reader bounding_box
    :initarg :bounding_box
    :type arbe_msgs-msg:arbeTSlamBox
    :initform (cl:make-instance 'arbe_msgs-msg:arbeTSlamBox))
   (age
    :reader age
    :initarg :age
    :type cl:integer
    :initform 0)
   (last_frame_update
    :reader last_frame_update
    :initarg :last_frame_update
    :type cl:fixnum
    :initform 0))
)

(cl:defclass arbeTSlamObj (<arbeTSlamObj>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeTSlamObj>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeTSlamObj)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeTSlamObj> is deprecated: use arbe_msgs-msg:arbeTSlamObj instead.")))

(cl:ensure-generic-function 'ID-val :lambda-list '(m))
(cl:defmethod ID-val ((m <arbeTSlamObj>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:ID-val is deprecated.  Use arbe_msgs-msg:ID instead.")
  (ID m))

(cl:ensure-generic-function 'obj_class-val :lambda-list '(m))
(cl:defmethod obj_class-val ((m <arbeTSlamObj>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:obj_class-val is deprecated.  Use arbe_msgs-msg:obj_class instead.")
  (obj_class m))

(cl:ensure-generic-function 'class_conf-val :lambda-list '(m))
(cl:defmethod class_conf-val ((m <arbeTSlamObj>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:class_conf-val is deprecated.  Use arbe_msgs-msg:class_conf instead.")
  (class_conf m))

(cl:ensure-generic-function 'position-val :lambda-list '(m))
(cl:defmethod position-val ((m <arbeTSlamObj>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:position-val is deprecated.  Use arbe_msgs-msg:position instead.")
  (position m))

(cl:ensure-generic-function 'velocity-val :lambda-list '(m))
(cl:defmethod velocity-val ((m <arbeTSlamObj>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:velocity-val is deprecated.  Use arbe_msgs-msg:velocity instead.")
  (velocity m))

(cl:ensure-generic-function 'bounding_box-val :lambda-list '(m))
(cl:defmethod bounding_box-val ((m <arbeTSlamObj>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:bounding_box-val is deprecated.  Use arbe_msgs-msg:bounding_box instead.")
  (bounding_box m))

(cl:ensure-generic-function 'age-val :lambda-list '(m))
(cl:defmethod age-val ((m <arbeTSlamObj>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:age-val is deprecated.  Use arbe_msgs-msg:age instead.")
  (age m))

(cl:ensure-generic-function 'last_frame_update-val :lambda-list '(m))
(cl:defmethod last_frame_update-val ((m <arbeTSlamObj>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:last_frame_update-val is deprecated.  Use arbe_msgs-msg:last_frame_update instead.")
  (last_frame_update m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeTSlamObj>) ostream)
  "Serializes a message object of type '<arbeTSlamObj>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'ID)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'ID)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_class)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_class)) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'class_conf))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'position) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'velocity) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'bounding_box) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'age)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'age)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'age)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'age)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'last_frame_update)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'last_frame_update)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeTSlamObj>) istream)
  "Deserializes a message object of type '<arbeTSlamObj>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'ID)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'ID)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_class)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_class)) (cl:read-byte istream))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'class_conf) (roslisp-utils:decode-single-float-bits bits)))
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'position) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'velocity) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'bounding_box) istream)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'age)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'age)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'age)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'age)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'last_frame_update)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'last_frame_update)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeTSlamObj>)))
  "Returns string type for a message object of type '<arbeTSlamObj>"
  "arbe_msgs/arbeTSlamObj")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeTSlamObj)))
  "Returns string type for a message object of type 'arbeTSlamObj"
  "arbe_msgs/arbeTSlamObj")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeTSlamObj>)))
  "Returns md5sum for a message object of type '<arbeTSlamObj>"
  "1eb101416f9437d15058033f4585fe65")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeTSlamObj)))
  "Returns md5sum for a message object of type 'arbeTSlamObj"
  "1eb101416f9437d15058033f4585fe65")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeTSlamObj>)))
  "Returns full string definition for message of type '<arbeTSlamObj>"
  (cl:format cl:nil "uint16              ID~%uint16              obj_class~%float32             class_conf~%arbeTSlamPos        position~%arbeTSlamVelocity   velocity ~%arbeTSlamBox        bounding_box~%uint32              age~%uint16              last_frame_update~%~%~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamPos~%float32 x ~%float32 y ~%float32 z ~%float32  dx ~%float32  dy ~%float32  dz~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamVelocity~%float32     x_dot ~%float32     y_dot ~%float32      dx_dot ~%float32      dy_dot ~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamBox~%float32 scale_x     ~%float32 scale_y     ~%float32 scale_z     ~%float32 scale_x_unc ~%float32 scale_y_unc ~%float32 scale_z_unc ~%float32 orientation_unc  ~%float32 orientation  ~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeTSlamObj)))
  "Returns full string definition for message of type 'arbeTSlamObj"
  (cl:format cl:nil "uint16              ID~%uint16              obj_class~%float32             class_conf~%arbeTSlamPos        position~%arbeTSlamVelocity   velocity ~%arbeTSlamBox        bounding_box~%uint32              age~%uint16              last_frame_update~%~%~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamPos~%float32 x ~%float32 y ~%float32 z ~%float32  dx ~%float32  dy ~%float32  dz~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamVelocity~%float32     x_dot ~%float32     y_dot ~%float32      dx_dot ~%float32      dy_dot ~%~%================================================================================~%MSG: arbe_msgs/arbeTSlamBox~%float32 scale_x     ~%float32 scale_y     ~%float32 scale_z     ~%float32 scale_x_unc ~%float32 scale_y_unc ~%float32 scale_z_unc ~%float32 orientation_unc  ~%float32 orientation  ~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeTSlamObj>))
  (cl:+ 0
     2
     2
     4
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'position))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'velocity))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'bounding_box))
     4
     2
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeTSlamObj>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeTSlamObj
    (cl:cons ':ID (ID msg))
    (cl:cons ':obj_class (obj_class msg))
    (cl:cons ':class_conf (class_conf msg))
    (cl:cons ':position (position msg))
    (cl:cons ':velocity (velocity msg))
    (cl:cons ':bounding_box (bounding_box msg))
    (cl:cons ':age (age msg))
    (cl:cons ':last_frame_update (last_frame_update msg))
))
