; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeCameraInstallationParams.msg.html

(cl:defclass <arbeCameraInstallationParams> (roslisp-msg-protocol:ros-message)
  ((intrinsic
    :reader intrinsic
    :initarg :intrinsic
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (extrinsic_trans
    :reader extrinsic_trans
    :initarg :extrinsic_trans
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (euler_a_b_g
    :reader euler_a_b_g
    :initarg :euler_a_b_g
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0)))
)

(cl:defclass arbeCameraInstallationParams (<arbeCameraInstallationParams>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeCameraInstallationParams>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeCameraInstallationParams)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeCameraInstallationParams> is deprecated: use arbe_msgs-msg:arbeCameraInstallationParams instead.")))

(cl:ensure-generic-function 'intrinsic-val :lambda-list '(m))
(cl:defmethod intrinsic-val ((m <arbeCameraInstallationParams>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:intrinsic-val is deprecated.  Use arbe_msgs-msg:intrinsic instead.")
  (intrinsic m))

(cl:ensure-generic-function 'extrinsic_trans-val :lambda-list '(m))
(cl:defmethod extrinsic_trans-val ((m <arbeCameraInstallationParams>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:extrinsic_trans-val is deprecated.  Use arbe_msgs-msg:extrinsic_trans instead.")
  (extrinsic_trans m))

(cl:ensure-generic-function 'euler_a_b_g-val :lambda-list '(m))
(cl:defmethod euler_a_b_g-val ((m <arbeCameraInstallationParams>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:euler_a_b_g-val is deprecated.  Use arbe_msgs-msg:euler_a_b_g instead.")
  (euler_a_b_g m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeCameraInstallationParams>) ostream)
  "Serializes a message object of type '<arbeCameraInstallationParams>"
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'intrinsic))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'intrinsic))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'extrinsic_trans))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'extrinsic_trans))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'euler_a_b_g))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'euler_a_b_g))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeCameraInstallationParams>) istream)
  "Deserializes a message object of type '<arbeCameraInstallationParams>"
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'intrinsic) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'intrinsic)))
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
  (cl:setf (cl:slot-value msg 'extrinsic_trans) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'extrinsic_trans)))
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
  (cl:setf (cl:slot-value msg 'euler_a_b_g) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'euler_a_b_g)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeCameraInstallationParams>)))
  "Returns string type for a message object of type '<arbeCameraInstallationParams>"
  "arbe_msgs/arbeCameraInstallationParams")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeCameraInstallationParams)))
  "Returns string type for a message object of type 'arbeCameraInstallationParams"
  "arbe_msgs/arbeCameraInstallationParams")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeCameraInstallationParams>)))
  "Returns md5sum for a message object of type '<arbeCameraInstallationParams>"
  "5d1e488e854ebf395d5c87a2670d2622")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeCameraInstallationParams)))
  "Returns md5sum for a message object of type 'arbeCameraInstallationParams"
  "5d1e488e854ebf395d5c87a2670d2622")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeCameraInstallationParams>)))
  "Returns full string definition for message of type '<arbeCameraInstallationParams>"
  (cl:format cl:nil "float32[] intrinsic~%float32[] extrinsic_trans~%float32[] euler_a_b_g~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeCameraInstallationParams)))
  "Returns full string definition for message of type 'arbeCameraInstallationParams"
  (cl:format cl:nil "float32[] intrinsic~%float32[] extrinsic_trans~%float32[] euler_a_b_g~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeCameraInstallationParams>))
  (cl:+ 0
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'intrinsic) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'extrinsic_trans) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'euler_a_b_g) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeCameraInstallationParams>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeCameraInstallationParams
    (cl:cons ':intrinsic (intrinsic msg))
    (cl:cons ':extrinsic_trans (extrinsic_trans msg))
    (cl:cons ':euler_a_b_g (euler_a_b_g msg))
))
