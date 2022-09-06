; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeFusionQuery.msg.html

(cl:defclass <arbeFusionQuery> (roslisp-msg-protocol:ros-message)
  ((obj_id
    :reader obj_id
    :initarg :obj_id
    :type cl:fixnum
    :initform 0)
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
    :initform 0)
   (query_id
    :reader query_id
    :initarg :query_id
    :type cl:integer
    :initform 0)
   (query_time
    :reader query_time
    :initarg :query_time
    :type cl:integer
    :initform 0)
   (obj_index
    :reader obj_index
    :initarg :obj_index
    :type cl:fixnum
    :initform 0)
   (tta_count
    :reader tta_count
    :initarg :tta_count
    :type cl:fixnum
    :initform 0)
   (tta_scores
    :reader tta_scores
    :initarg :tta_scores
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0)))
)

(cl:defclass arbeFusionQuery (<arbeFusionQuery>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeFusionQuery>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeFusionQuery)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeFusionQuery> is deprecated: use arbe_msgs-msg:arbeFusionQuery instead.")))

(cl:ensure-generic-function 'obj_id-val :lambda-list '(m))
(cl:defmethod obj_id-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:obj_id-val is deprecated.  Use arbe_msgs-msg:obj_id instead.")
  (obj_id m))

(cl:ensure-generic-function 'top-val :lambda-list '(m))
(cl:defmethod top-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:top-val is deprecated.  Use arbe_msgs-msg:top instead.")
  (top m))

(cl:ensure-generic-function 'left-val :lambda-list '(m))
(cl:defmethod left-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:left-val is deprecated.  Use arbe_msgs-msg:left instead.")
  (left m))

(cl:ensure-generic-function 'bottom-val :lambda-list '(m))
(cl:defmethod bottom-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:bottom-val is deprecated.  Use arbe_msgs-msg:bottom instead.")
  (bottom m))

(cl:ensure-generic-function 'right-val :lambda-list '(m))
(cl:defmethod right-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:right-val is deprecated.  Use arbe_msgs-msg:right instead.")
  (right m))

(cl:ensure-generic-function 'query_id-val :lambda-list '(m))
(cl:defmethod query_id-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:query_id-val is deprecated.  Use arbe_msgs-msg:query_id instead.")
  (query_id m))

(cl:ensure-generic-function 'query_time-val :lambda-list '(m))
(cl:defmethod query_time-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:query_time-val is deprecated.  Use arbe_msgs-msg:query_time instead.")
  (query_time m))

(cl:ensure-generic-function 'obj_index-val :lambda-list '(m))
(cl:defmethod obj_index-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:obj_index-val is deprecated.  Use arbe_msgs-msg:obj_index instead.")
  (obj_index m))

(cl:ensure-generic-function 'tta_count-val :lambda-list '(m))
(cl:defmethod tta_count-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tta_count-val is deprecated.  Use arbe_msgs-msg:tta_count instead.")
  (tta_count m))

(cl:ensure-generic-function 'tta_scores-val :lambda-list '(m))
(cl:defmethod tta_scores-val ((m <arbeFusionQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tta_scores-val is deprecated.  Use arbe_msgs-msg:tta_scores instead.")
  (tta_scores m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeFusionQuery>) ostream)
  "Serializes a message object of type '<arbeFusionQuery>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'top)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'top)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'left)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'left)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'bottom)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'bottom)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'right)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'right)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'query_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'query_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'query_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'query_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'query_time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'query_time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'query_time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'query_time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'query_time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'query_time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'query_time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'query_time)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_index)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_index)) ostream)
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
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeFusionQuery>) istream)
  "Deserializes a message object of type '<arbeFusionQuery>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'top)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'top)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'left)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'left)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'bottom)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'bottom)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'right)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'right)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'query_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'query_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'query_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'query_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'query_time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'query_time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'query_time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'query_time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'query_time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'query_time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'query_time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'query_time)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_index)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_index)) (cl:read-byte istream))
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
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeFusionQuery>)))
  "Returns string type for a message object of type '<arbeFusionQuery>"
  "arbe_msgs/arbeFusionQuery")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeFusionQuery)))
  "Returns string type for a message object of type 'arbeFusionQuery"
  "arbe_msgs/arbeFusionQuery")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeFusionQuery>)))
  "Returns md5sum for a message object of type '<arbeFusionQuery>"
  "d6277f1c47cde1be60fd589e75618a8d")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeFusionQuery)))
  "Returns md5sum for a message object of type 'arbeFusionQuery"
  "d6277f1c47cde1be60fd589e75618a8d")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeFusionQuery>)))
  "Returns full string definition for message of type '<arbeFusionQuery>"
  (cl:format cl:nil "uint16  	obj_id~%uint16  	top~%uint16  	left~%uint16  	bottom~%uint16  	right~%uint32  	query_id~%uint64	  	query_time~%uint16  	obj_index~%uint8  	tta_count~%float32[] tta_scores~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeFusionQuery)))
  "Returns full string definition for message of type 'arbeFusionQuery"
  (cl:format cl:nil "uint16  	obj_id~%uint16  	top~%uint16  	left~%uint16  	bottom~%uint16  	right~%uint32  	query_id~%uint64	  	query_time~%uint16  	obj_index~%uint8  	tta_count~%float32[] tta_scores~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeFusionQuery>))
  (cl:+ 0
     2
     2
     2
     2
     2
     4
     8
     2
     1
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'tta_scores) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeFusionQuery>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeFusionQuery
    (cl:cons ':obj_id (obj_id msg))
    (cl:cons ':top (top msg))
    (cl:cons ':left (left msg))
    (cl:cons ':bottom (bottom msg))
    (cl:cons ':right (right msg))
    (cl:cons ':query_id (query_id msg))
    (cl:cons ':query_time (query_time msg))
    (cl:cons ':obj_index (obj_index msg))
    (cl:cons ':tta_count (tta_count msg))
    (cl:cons ':tta_scores (tta_scores msg))
))
