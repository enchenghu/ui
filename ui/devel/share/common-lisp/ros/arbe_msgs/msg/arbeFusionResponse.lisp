; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeFusionResponse.msg.html

(cl:defclass <arbeFusionResponse> (roslisp-msg-protocol:ros-message)
  ((valid
    :reader valid
    :initarg :valid
    :type cl:boolean
    :initform cl:nil)
   (obj_id
    :reader obj_id
    :initarg :obj_id
    :type cl:fixnum
    :initform 0)
   (query_id
    :reader query_id
    :initarg :query_id
    :type cl:integer
    :initform 0)
   (fusionClass
    :reader fusionClass
    :initarg :fusionClass
    :type cl:fixnum
    :initform 0)
   (obj_index
    :reader obj_index
    :initarg :obj_index
    :type cl:fixnum
    :initform 0)
   (queryTime
    :reader queryTime
    :initarg :queryTime
    :type cl:integer
    :initform 0)
   (conf
    :reader conf
    :initarg :conf
    :type cl:float
    :initform 0.0)
   (tta_count
    :reader tta_count
    :initarg :tta_count
    :type cl:fixnum
    :initform 0)
   (tta_scores
    :reader tta_scores
    :initarg :tta_scores
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (class_str
    :reader class_str
    :initarg :class_str
    :type cl:string
    :initform "")
   (fusionClasses
    :reader fusionClasses
    :initarg :fusionClasses
    :type arbe_msgs-msg:arbeFusionClasses
    :initform (cl:make-instance 'arbe_msgs-msg:arbeFusionClasses))
   (top
    :reader top
    :initarg :top
    :type cl:fixnum
    :initform 0)
   (left
    :reader left
    :initarg :left
    :type cl:fixnum
    :initform 0)
   (bottom
    :reader bottom
    :initarg :bottom
    :type cl:fixnum
    :initform 0)
   (right
    :reader right
    :initarg :right
    :type cl:fixnum
    :initform 0))
)

(cl:defclass arbeFusionResponse (<arbeFusionResponse>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeFusionResponse>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeFusionResponse)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeFusionResponse> is deprecated: use arbe_msgs-msg:arbeFusionResponse instead.")))

(cl:ensure-generic-function 'valid-val :lambda-list '(m))
(cl:defmethod valid-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:valid-val is deprecated.  Use arbe_msgs-msg:valid instead.")
  (valid m))

(cl:ensure-generic-function 'obj_id-val :lambda-list '(m))
(cl:defmethod obj_id-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:obj_id-val is deprecated.  Use arbe_msgs-msg:obj_id instead.")
  (obj_id m))

(cl:ensure-generic-function 'query_id-val :lambda-list '(m))
(cl:defmethod query_id-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:query_id-val is deprecated.  Use arbe_msgs-msg:query_id instead.")
  (query_id m))

(cl:ensure-generic-function 'fusionClass-val :lambda-list '(m))
(cl:defmethod fusionClass-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:fusionClass-val is deprecated.  Use arbe_msgs-msg:fusionClass instead.")
  (fusionClass m))

(cl:ensure-generic-function 'obj_index-val :lambda-list '(m))
(cl:defmethod obj_index-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:obj_index-val is deprecated.  Use arbe_msgs-msg:obj_index instead.")
  (obj_index m))

(cl:ensure-generic-function 'queryTime-val :lambda-list '(m))
(cl:defmethod queryTime-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:queryTime-val is deprecated.  Use arbe_msgs-msg:queryTime instead.")
  (queryTime m))

(cl:ensure-generic-function 'conf-val :lambda-list '(m))
(cl:defmethod conf-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:conf-val is deprecated.  Use arbe_msgs-msg:conf instead.")
  (conf m))

(cl:ensure-generic-function 'tta_count-val :lambda-list '(m))
(cl:defmethod tta_count-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tta_count-val is deprecated.  Use arbe_msgs-msg:tta_count instead.")
  (tta_count m))

(cl:ensure-generic-function 'tta_scores-val :lambda-list '(m))
(cl:defmethod tta_scores-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tta_scores-val is deprecated.  Use arbe_msgs-msg:tta_scores instead.")
  (tta_scores m))

(cl:ensure-generic-function 'class_str-val :lambda-list '(m))
(cl:defmethod class_str-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:class_str-val is deprecated.  Use arbe_msgs-msg:class_str instead.")
  (class_str m))

(cl:ensure-generic-function 'fusionClasses-val :lambda-list '(m))
(cl:defmethod fusionClasses-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:fusionClasses-val is deprecated.  Use arbe_msgs-msg:fusionClasses instead.")
  (fusionClasses m))

(cl:ensure-generic-function 'top-val :lambda-list '(m))
(cl:defmethod top-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:top-val is deprecated.  Use arbe_msgs-msg:top instead.")
  (top m))

(cl:ensure-generic-function 'left-val :lambda-list '(m))
(cl:defmethod left-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:left-val is deprecated.  Use arbe_msgs-msg:left instead.")
  (left m))

(cl:ensure-generic-function 'bottom-val :lambda-list '(m))
(cl:defmethod bottom-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:bottom-val is deprecated.  Use arbe_msgs-msg:bottom instead.")
  (bottom m))

(cl:ensure-generic-function 'right-val :lambda-list '(m))
(cl:defmethod right-val ((m <arbeFusionResponse>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:right-val is deprecated.  Use arbe_msgs-msg:right instead.")
  (right m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeFusionResponse>) ostream)
  "Serializes a message object of type '<arbeFusionResponse>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:if (cl:slot-value msg 'valid) 1 0)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'query_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'query_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'query_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'query_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'fusionClass)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_index)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_index)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'queryTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'queryTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'queryTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'queryTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'queryTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'queryTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'queryTime)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'queryTime)) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'conf))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'tta_count)) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'tta_scores))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'tta_scores))
  (cl:let ((__ros_str_len (cl:length (cl:slot-value msg 'class_str))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_str_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_str_len) ostream))
  (cl:map cl:nil #'(cl:lambda (c) (cl:write-byte (cl:char-code c) ostream)) (cl:slot-value msg 'class_str))
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'fusionClasses) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'top)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'top)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'left)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'left)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'bottom)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'bottom)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'right)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'right)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeFusionResponse>) istream)
  "Deserializes a message object of type '<arbeFusionResponse>"
    (cl:setf (cl:slot-value msg 'valid) (cl:not (cl:zerop (cl:read-byte istream))))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'query_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'query_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'query_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'query_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'fusionClass)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_index)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_index)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'queryTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'queryTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'queryTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'queryTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'queryTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'queryTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'queryTime)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'queryTime)) (cl:read-byte istream))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'conf) (roslisp-utils:decode-single-float-bits bits)))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'tta_count)) (cl:read-byte istream))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'tta_scores) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'tta_scores)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
    (cl:let ((__ros_str_len 0))
      (cl:setf (cl:ldb (cl:byte 8 0) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) __ros_str_len) (cl:read-byte istream))
      (cl:setf (cl:slot-value msg 'class_str) (cl:make-string __ros_str_len))
      (cl:dotimes (__ros_str_idx __ros_str_len msg)
        (cl:setf (cl:char (cl:slot-value msg 'class_str) __ros_str_idx) (cl:code-char (cl:read-byte istream)))))
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'fusionClasses) istream)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'top)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'top)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'left)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'left)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'bottom)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'bottom)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'right)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'right)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeFusionResponse>)))
  "Returns string type for a message object of type '<arbeFusionResponse>"
  "arbe_msgs/arbeFusionResponse")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeFusionResponse)))
  "Returns string type for a message object of type 'arbeFusionResponse"
  "arbe_msgs/arbeFusionResponse")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeFusionResponse>)))
  "Returns md5sum for a message object of type '<arbeFusionResponse>"
  "9d16e7674f130b5c070b9bc414f29ed5")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeFusionResponse)))
  "Returns md5sum for a message object of type 'arbeFusionResponse"
  "9d16e7674f130b5c070b9bc414f29ed5")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeFusionResponse>)))
  "Returns full string definition for message of type '<arbeFusionResponse>"
  (cl:format cl:nil "bool valid~%uint16 obj_id~%uint32 query_id~%uint8 fusionClass~%uint16 obj_index~%uint64 queryTime~%float32 conf~%uint8 tta_count~%float32[] tta_scores~%string class_str~%arbeFusionClasses fusionClasses~%~%uint16  top~%uint16  left~%uint16  bottom~%uint16  right~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeFusionResponse)))
  "Returns full string definition for message of type 'arbeFusionResponse"
  (cl:format cl:nil "bool valid~%uint16 obj_id~%uint32 query_id~%uint8 fusionClass~%uint16 obj_index~%uint64 queryTime~%float32 conf~%uint8 tta_count~%float32[] tta_scores~%string class_str~%arbeFusionClasses fusionClasses~%~%uint16  top~%uint16  left~%uint16  bottom~%uint16  right~%~%================================================================================~%MSG: arbe_msgs/arbeFusionClasses~%uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeFusionResponse>))
  (cl:+ 0
     1
     2
     4
     1
     2
     8
     4
     1
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'tta_scores) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:length (cl:slot-value msg 'class_str))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'fusionClasses))
     2
     2
     2
     2
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeFusionResponse>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeFusionResponse
    (cl:cons ':valid (valid msg))
    (cl:cons ':obj_id (obj_id msg))
    (cl:cons ':query_id (query_id msg))
    (cl:cons ':fusionClass (fusionClass msg))
    (cl:cons ':obj_index (obj_index msg))
    (cl:cons ':queryTime (queryTime msg))
    (cl:cons ':conf (conf msg))
    (cl:cons ':tta_count (tta_count msg))
    (cl:cons ':tta_scores (tta_scores msg))
    (cl:cons ':class_str (class_str msg))
    (cl:cons ':fusionClasses (fusionClasses msg))
    (cl:cons ':top (top msg))
    (cl:cons ':left (left msg))
    (cl:cons ':bottom (bottom msg))
    (cl:cons ':right (right msg))
))
