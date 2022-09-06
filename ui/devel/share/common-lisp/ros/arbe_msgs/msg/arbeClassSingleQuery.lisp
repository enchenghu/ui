; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeClassSingleQuery.msg.html

(cl:defclass <arbeClassSingleQuery> (roslisp-msg-protocol:ros-message)
  ((obj_id
    :reader obj_id
    :initarg :obj_id
    :type cl:fixnum
    :initform 0)
   (obj_index
    :reader obj_index
    :initarg :obj_index
    :type cl:fixnum
    :initform 0)
   (n_points
    :reader n_points
    :initarg :n_points
    :type cl:fixnum
    :initform 0)
   (xyzvp
    :reader xyzvp
    :initarg :xyzvp
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (xyzvp_center
    :reader xyzvp_center
    :initarg :xyzvp_center
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (tta_scores
    :reader tta_scores
    :initarg :tta_scores
    :type (cl:vector cl:float)
   :initform (cl:make-array 0 :element-type 'cl:float :initial-element 0.0))
   (tta_count
    :reader tta_count
    :initarg :tta_count
    :type cl:integer
    :initform 0))
)

(cl:defclass arbeClassSingleQuery (<arbeClassSingleQuery>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeClassSingleQuery>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeClassSingleQuery)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeClassSingleQuery> is deprecated: use arbe_msgs-msg:arbeClassSingleQuery instead.")))

(cl:ensure-generic-function 'obj_id-val :lambda-list '(m))
(cl:defmethod obj_id-val ((m <arbeClassSingleQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:obj_id-val is deprecated.  Use arbe_msgs-msg:obj_id instead.")
  (obj_id m))

(cl:ensure-generic-function 'obj_index-val :lambda-list '(m))
(cl:defmethod obj_index-val ((m <arbeClassSingleQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:obj_index-val is deprecated.  Use arbe_msgs-msg:obj_index instead.")
  (obj_index m))

(cl:ensure-generic-function 'n_points-val :lambda-list '(m))
(cl:defmethod n_points-val ((m <arbeClassSingleQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:n_points-val is deprecated.  Use arbe_msgs-msg:n_points instead.")
  (n_points m))

(cl:ensure-generic-function 'xyzvp-val :lambda-list '(m))
(cl:defmethod xyzvp-val ((m <arbeClassSingleQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:xyzvp-val is deprecated.  Use arbe_msgs-msg:xyzvp instead.")
  (xyzvp m))

(cl:ensure-generic-function 'xyzvp_center-val :lambda-list '(m))
(cl:defmethod xyzvp_center-val ((m <arbeClassSingleQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:xyzvp_center-val is deprecated.  Use arbe_msgs-msg:xyzvp_center instead.")
  (xyzvp_center m))

(cl:ensure-generic-function 'tta_scores-val :lambda-list '(m))
(cl:defmethod tta_scores-val ((m <arbeClassSingleQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tta_scores-val is deprecated.  Use arbe_msgs-msg:tta_scores instead.")
  (tta_scores m))

(cl:ensure-generic-function 'tta_count-val :lambda-list '(m))
(cl:defmethod tta_count-val ((m <arbeClassSingleQuery>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:tta_count-val is deprecated.  Use arbe_msgs-msg:tta_count instead.")
  (tta_count m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeClassSingleQuery>) ostream)
  "Serializes a message object of type '<arbeClassSingleQuery>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_id)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_index)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_index)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'n_points)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'n_points)) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'xyzvp))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'xyzvp))
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'xyzvp_center))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let ((bits (roslisp-utils:encode-single-float-bits ele)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)))
   (cl:slot-value msg 'xyzvp_center))
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
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'tta_count)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'tta_count)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'tta_count)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'tta_count)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeClassSingleQuery>) istream)
  "Deserializes a message object of type '<arbeClassSingleQuery>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_id)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'obj_index)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'obj_index)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'n_points)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'n_points)) (cl:read-byte istream))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'xyzvp) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'xyzvp)))
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
  (cl:setf (cl:slot-value msg 'xyzvp_center) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'xyzvp_center)))
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
  (cl:setf (cl:slot-value msg 'tta_scores) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'tta_scores)))
    (cl:dotimes (i __ros_arr_len)
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:aref vals i) (roslisp-utils:decode-single-float-bits bits))))))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'tta_count)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'tta_count)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'tta_count)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'tta_count)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeClassSingleQuery>)))
  "Returns string type for a message object of type '<arbeClassSingleQuery>"
  "arbe_msgs/arbeClassSingleQuery")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeClassSingleQuery)))
  "Returns string type for a message object of type 'arbeClassSingleQuery"
  "arbe_msgs/arbeClassSingleQuery")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeClassSingleQuery>)))
  "Returns md5sum for a message object of type '<arbeClassSingleQuery>"
  "64f8fe3f54997e174c21d53624f859dd")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeClassSingleQuery)))
  "Returns md5sum for a message object of type 'arbeClassSingleQuery"
  "64f8fe3f54997e174c21d53624f859dd")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeClassSingleQuery>)))
  "Returns full string definition for message of type '<arbeClassSingleQuery>"
  (cl:format cl:nil "uint16 obj_id~%uint16 obj_index~%uint16 n_points~%float32[] xyzvp~%float32[] xyzvp_center~%float32[] tta_scores~%uint32 tta_count~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeClassSingleQuery)))
  "Returns full string definition for message of type 'arbeClassSingleQuery"
  (cl:format cl:nil "uint16 obj_id~%uint16 obj_index~%uint16 n_points~%float32[] xyzvp~%float32[] xyzvp_center~%float32[] tta_scores~%uint32 tta_count~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeClassSingleQuery>))
  (cl:+ 0
     2
     2
     2
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'xyzvp) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'xyzvp_center) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'tta_scores) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 4)))
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeClassSingleQuery>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeClassSingleQuery
    (cl:cons ':obj_id (obj_id msg))
    (cl:cons ':obj_index (obj_index msg))
    (cl:cons ':n_points (n_points msg))
    (cl:cons ':xyzvp (xyzvp msg))
    (cl:cons ':xyzvp_center (xyzvp_center msg))
    (cl:cons ':tta_scores (tta_scores msg))
    (cl:cons ':tta_count (tta_count msg))
))
