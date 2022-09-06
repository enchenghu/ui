; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeSettingsPerRadar.msg.html

(cl:defclass <arbeSettingsPerRadar> (roslisp-msg-protocol:ros-message)
  ((n_radars
    :reader n_radars
    :initarg :n_radars
    :type cl:fixnum
    :initform 0)
   (radar_for_text
    :reader radar_for_text
    :initarg :radar_for_text
    :type cl:fixnum
    :initform 0)
   (ant_height
    :reader ant_height
    :initarg :ant_height
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (ant_pitch
    :reader ant_pitch
    :initarg :ant_pitch
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (cam_euler_a
    :reader cam_euler_a
    :initarg :cam_euler_a
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (cam_euler_b
    :reader cam_euler_b
    :initarg :cam_euler_b
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (cam_euler_g
    :reader cam_euler_g
    :initarg :cam_euler_g
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0)))
)

(cl:defclass arbeSettingsPerRadar (<arbeSettingsPerRadar>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeSettingsPerRadar>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeSettingsPerRadar)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeSettingsPerRadar> is deprecated: use arbe_msgs-msg:arbeSettingsPerRadar instead.")))

(cl:ensure-generic-function 'n_radars-val :lambda-list '(m))
(cl:defmethod n_radars-val ((m <arbeSettingsPerRadar>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:n_radars-val is deprecated.  Use arbe_msgs-msg:n_radars instead.")
  (n_radars m))

(cl:ensure-generic-function 'radar_for_text-val :lambda-list '(m))
(cl:defmethod radar_for_text-val ((m <arbeSettingsPerRadar>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:radar_for_text-val is deprecated.  Use arbe_msgs-msg:radar_for_text instead.")
  (radar_for_text m))

(cl:ensure-generic-function 'ant_height-val :lambda-list '(m))
(cl:defmethod ant_height-val ((m <arbeSettingsPerRadar>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:ant_height-val is deprecated.  Use arbe_msgs-msg:ant_height instead.")
  (ant_height m))

(cl:ensure-generic-function 'ant_pitch-val :lambda-list '(m))
(cl:defmethod ant_pitch-val ((m <arbeSettingsPerRadar>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:ant_pitch-val is deprecated.  Use arbe_msgs-msg:ant_pitch instead.")
  (ant_pitch m))

(cl:ensure-generic-function 'cam_euler_a-val :lambda-list '(m))
(cl:defmethod cam_euler_a-val ((m <arbeSettingsPerRadar>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:cam_euler_a-val is deprecated.  Use arbe_msgs-msg:cam_euler_a instead.")
  (cam_euler_a m))

(cl:ensure-generic-function 'cam_euler_b-val :lambda-list '(m))
(cl:defmethod cam_euler_b-val ((m <arbeSettingsPerRadar>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:cam_euler_b-val is deprecated.  Use arbe_msgs-msg:cam_euler_b instead.")
  (cam_euler_b m))

(cl:ensure-generic-function 'cam_euler_g-val :lambda-list '(m))
(cl:defmethod cam_euler_g-val ((m <arbeSettingsPerRadar>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:cam_euler_g-val is deprecated.  Use arbe_msgs-msg:cam_euler_g instead.")
  (cam_euler_g m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeSettingsPerRadar>) ostream)
  "Serializes a message object of type '<arbeSettingsPerRadar>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'n_radars)) ostream)
  (cl:let* ((signed (cl:slot-value msg 'radar_for_text)) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    )
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'ant_height))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'ant_height))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'ant_pitch))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'ant_pitch))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'cam_euler_a))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'cam_euler_a))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'cam_euler_b))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'cam_euler_b))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'cam_euler_g))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'cam_euler_g))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeSettingsPerRadar>) istream)
  "Deserializes a message object of type '<arbeSettingsPerRadar>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'n_radars)) (cl:read-byte istream))
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'radar_for_text) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'ant_height) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'ant_height)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'ant_pitch) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'ant_pitch)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'cam_euler_a) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'cam_euler_a)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'cam_euler_b) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'cam_euler_b)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'cam_euler_g) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'cam_euler_g)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeSettingsPerRadar>)))
  "Returns string type for a message object of type '<arbeSettingsPerRadar>"
  "arbe_msgs/arbeSettingsPerRadar")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeSettingsPerRadar)))
  "Returns string type for a message object of type 'arbeSettingsPerRadar"
  "arbe_msgs/arbeSettingsPerRadar")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeSettingsPerRadar>)))
  "Returns md5sum for a message object of type '<arbeSettingsPerRadar>"
  "0ee305f5b1c77996631bbad5b0e5663d")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeSettingsPerRadar)))
  "Returns md5sum for a message object of type 'arbeSettingsPerRadar"
  "0ee305f5b1c77996631bbad5b0e5663d")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeSettingsPerRadar>)))
  "Returns full string definition for message of type '<arbeSettingsPerRadar>"
  (cl:format cl:nil "uint8 n_radars~%int8  radar_for_text~%float32[] ant_height~%float32[] ant_pitch~%float32[] cam_euler_a~%float32[] cam_euler_b~%float32[] cam_euler_g~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeSettingsPerRadar)))
  "Returns full string definition for message of type 'arbeSettingsPerRadar"
  (cl:format cl:nil "uint8 n_radars~%int8  radar_for_text~%float32[] ant_height~%float32[] ant_pitch~%float32[] cam_euler_a~%float32[] cam_euler_b~%float32[] cam_euler_g~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeSettingsPerRadar>))
  (cl:+ 0
     1
     1
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'ant_height) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'ant_pitch) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'cam_euler_a) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'cam_euler_b) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'cam_euler_g) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeSettingsPerRadar>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeSettingsPerRadar
    (cl:cons ':n_radars (n_radars msg))
    (cl:cons ':radar_for_text (radar_for_text msg))
    (cl:cons ':ant_height (ant_height msg))
    (cl:cons ':ant_pitch (ant_pitch msg))
    (cl:cons ':cam_euler_a (cam_euler_a msg))
    (cl:cons ':cam_euler_b (cam_euler_b msg))
    (cl:cons ':cam_euler_g (cam_euler_g msg))
))
