; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeClassificationMetadata.msg.html

(cl:defclass <arbeClassificationMetadata> (roslisp-msg-protocol:ros-message)
  ((enable_tta
    :reader enable_tta
    :initarg :enable_tta
    :type cl:boolean
    :initform cl:nil)
   (post_factum_mode
    :reader post_factum_mode
    :initarg :post_factum_mode
    :type cl:boolean
    :initform cl:nil)
   (average_mode
    :reader average_mode
    :initarg :average_mode
    :type cl:boolean
    :initform cl:nil)
   (tta_decay
    :reader tta_decay
    :initarg :tta_decay
    :type cl:boolean
    :initform cl:nil)
   (tta_momentum
    :reader tta_momentum
    :initarg :tta_momentum
    :type cl:float
    :initform 0.0)
   (conf_TH
    :reader conf_TH
    :initarg :conf_TH
    :type cl:float
    :initform 0.0))
)

(cl:defclass arbeClassificationMetadata (<arbeClassificationMetadata>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeClassificationMetadata>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeClassificationMetadata)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeClassificationMetadata> is deprecated: use arbe_msgs-msg:arbeClassificationMetadata instead.")))

(cl:ensure-generic-function 'enable_tta-val :lambda-list '(m))
(cl:defmethod enable_tta-val ((m <arbeClassificationMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:enable_tta-val is deprecated.  Use arbe_msgs-msg:enable_tta instead.")
  (enable_tta m))

(cl:ensure-generic-function 'post_factum_mode-val :lambda-list '(m))
(cl:defmethod post_factum_mode-val ((m <arbeClassificationMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:post_factum_mode-val is deprecated.  Use arbe_msgs-msg:post_factum_mode instead.")
  (post_factum_mode m))

(cl:ensure-generic-function 'average_mode-val :lambda-list '(m))
(cl:defmethod average_mode-val ((m <arbeClassificationMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:average_mode-val is deprecated.  Use arbe_msgs-msg:average_mode instead.")
  (average_mode m))

(cl:ensure-generic-function 'tta_decay-val :lambda-list '(m))
(cl:defmethod tta_decay-val ((m <arbeClassificationMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tta_decay-val is deprecated.  Use arbe_msgs-msg:tta_decay instead.")
  (tta_decay m))

(cl:ensure-generic-function 'tta_momentum-val :lambda-list '(m))
(cl:defmethod tta_momentum-val ((m <arbeClassificationMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tta_momentum-val is deprecated.  Use arbe_msgs-msg:tta_momentum instead.")
  (tta_momentum m))

(cl:ensure-generic-function 'conf_TH-val :lambda-list '(m))
(cl:defmethod conf_TH-val ((m <arbeClassificationMetadata>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:conf_TH-val is deprecated.  Use arbe_msgs-msg:conf_TH instead.")
  (conf_TH m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeClassificationMetadata>) ostream)
  "Serializes a message object of type '<arbeClassificationMetadata>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'enable_tta) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'post_factum_mode) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'average_mode) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'tta_decay) 1 0)) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'tta_momentum))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'conf_TH))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeClassificationMetadata>) istream)
  "Deserializes a message object of type '<arbeClassificationMetadata>"
    (cl:setf (cl:slot-value msg 'enable_tta) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'post_factum_mode) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'average_mode) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:slot-value msg 'tta_decay) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'tta_momentum) (roslisp-utils:decode-single-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'conf_TH) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeClassificationMetadata>)))
  "Returns string type for a message object of type '<arbeClassificationMetadata>"
  "arbe_msgs/arbeClassificationMetadata")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeClassificationMetadata)))
  "Returns string type for a message object of type 'arbeClassificationMetadata"
  "arbe_msgs/arbeClassificationMetadata")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeClassificationMetadata>)))
  "Returns md5sum for a message object of type '<arbeClassificationMetadata>"
  "2def84262941647c9070003f42808b6a")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeClassificationMetadata)))
  "Returns md5sum for a message object of type 'arbeClassificationMetadata"
  "2def84262941647c9070003f42808b6a")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeClassificationMetadata>)))
  "Returns full string definition for message of type '<arbeClassificationMetadata>"
  (cl:format cl:nil "bool enable_tta~%bool post_factum_mode~%bool average_mode~%bool tta_decay~%float32 tta_momentum~%float32 conf_TH~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeClassificationMetadata)))
  "Returns full string definition for message of type 'arbeClassificationMetadata"
  (cl:format cl:nil "bool enable_tta~%bool post_factum_mode~%bool average_mode~%bool tta_decay~%float32 tta_momentum~%float32 conf_TH~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeClassificationMetadata>))
  (cl:+ 0
     1
     1
     1
     1
     4
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeClassificationMetadata>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeClassificationMetadata
    (cl:cons ':enable_tta (enable_tta msg))
    (cl:cons ':post_factum_mode (post_factum_mode msg))
    (cl:cons ':average_mode (average_mode msg))
    (cl:cons ':tta_decay (tta_decay msg))
    (cl:cons ':tta_momentum (tta_momentum msg))
    (cl:cons ':conf_TH (conf_TH msg))
))
