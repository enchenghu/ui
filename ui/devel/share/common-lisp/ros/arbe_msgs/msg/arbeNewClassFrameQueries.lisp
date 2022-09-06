; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeNewClassFrameQueries.msg.html

(cl:defclass <arbeNewClassFrameQueries> (roslisp-msg-protocol:ros-message)
  ((Header
    :reader Header
    :initarg :Header
    :type std_msgs-msg:Header
    :initform (cl:make-instance 'std_msgs-msg:Header))
   (class_metadata
    :reader class_metadata
    :initarg :class_metadata
    :type arbe_msgs-msg:arbeClassificationMetadata
    :initform (cl:make-instance 'arbe_msgs-msg:arbeClassificationMetadata))
   (frame_counter
    :reader frame_counter
    :initarg :frame_counter
    :type cl:fixnum
    :initform 0)
   (queries
    :reader queries
    :initarg :queries
    :type (cl:vector arbe_msgs-msg:arbeClassSingleQuery)
   :initform (cl:make-array 0 :element-type 'arbe_msgs-msg:arbeClassSingleQuery :initial-element (cl:make-instance 'arbe_msgs-msg:arbeClassSingleQuery)))
   (mutual_len
    :reader mutual_len
    :initarg :mutual_len
    :type cl:fixnum
    :initform 0))
)

(cl:defclass arbeNewClassFrameQueries (<arbeNewClassFrameQueries>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeNewClassFrameQueries>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeNewClassFrameQueries)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeNewClassFrameQueries> is deprecated: use arbe_msgs-msg:arbeNewClassFrameQueries instead.")))

(cl:ensure-generic-function 'Header-val :lambda-list '(m))
(cl:defmethod Header-val ((m <arbeNewClassFrameQueries>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:Header-val is deprecated.  Use arbe_msgs-msg:Header instead.")
  (Header m))

(cl:ensure-generic-function 'class_metadata-val :lambda-list '(m))
(cl:defmethod class_metadata-val ((m <arbeNewClassFrameQueries>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:class_metadata-val is deprecated.  Use arbe_msgs-msg:class_metadata instead.")
  (class_metadata m))

(cl:ensure-generic-function 'frame_counter-val :lambda-list '(m))
(cl:defmethod frame_counter-val ((m <arbeNewClassFrameQueries>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:frame_counter-val is deprecated.  Use arbe_msgs-msg:frame_counter instead.")
  (frame_counter m))

(cl:ensure-generic-function 'queries-val :lambda-list '(m))
(cl:defmethod queries-val ((m <arbeNewClassFrameQueries>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:queries-val is deprecated.  Use arbe_msgs-msg:queries instead.")
  (queries m))

(cl:ensure-generic-function 'mutual_len-val :lambda-list '(m))
(cl:defmethod mutual_len-val ((m <arbeNewClassFrameQueries>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:mutual_len-val is deprecated.  Use arbe_msgs-msg:mutual_len instead.")
  (mutual_len m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeNewClassFrameQueries>) ostream)
  "Serializes a message object of type '<arbeNewClassFrameQueries>"
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'Header) ostream)
  (roslisp-msg-protocol:serialize (cl:slot-value msg 'class_metadata) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_counter)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_counter)) ostream)
  (cl:let ((__ros_arr_len (cl:length (cl:slot-value msg 'queries))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) __ros_arr_len) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) __ros_arr_len) ostream))
  (cl:map cl:nil #'(cl:lambda (ele) (roslisp-msg-protocol:serialize ele ostream))
   (cl:slot-value msg 'queries))
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'mutual_len)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'mutual_len)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeNewClassFrameQueries>) istream)
  "Deserializes a message object of type '<arbeNewClassFrameQueries>"
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'Header) istream)
  (roslisp-msg-protocol:deserialize (cl:slot-value msg 'class_metadata) istream)
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_counter)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_counter)) (cl:read-byte istream))
  (cl:let ((__ros_arr_len 0))
    (cl:setf (cl:ldb (cl:byte 8 0) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) __ros_arr_len) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) __ros_arr_len) (cl:read-byte istream))
  (cl:setf (cl:slot-value msg 'queries) (cl:make-array __ros_arr_len))
  (cl:let ((vals (cl:slot-value msg 'queries)))
    (cl:dotimes (i __ros_arr_len)
    (cl:setf (cl:aref vals i) (cl:make-instance 'arbe_msgs-msg:arbeClassSingleQuery))
  (roslisp-msg-protocol:deserialize (cl:aref vals i) istream))))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'mutual_len)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'mutual_len)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeNewClassFrameQueries>)))
  "Returns string type for a message object of type '<arbeNewClassFrameQueries>"
  "arbe_msgs/arbeNewClassFrameQueries")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeNewClassFrameQueries)))
  "Returns string type for a message object of type 'arbeNewClassFrameQueries"
  "arbe_msgs/arbeNewClassFrameQueries")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeNewClassFrameQueries>)))
  "Returns md5sum for a message object of type '<arbeNewClassFrameQueries>"
  "01f7dd27368405062b7c22b947b01851")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeNewClassFrameQueries)))
  "Returns md5sum for a message object of type 'arbeNewClassFrameQueries"
  "01f7dd27368405062b7c22b947b01851")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeNewClassFrameQueries>)))
  "Returns full string definition for message of type '<arbeNewClassFrameQueries>"
  (cl:format cl:nil "std_msgs/Header Header~%arbeClassificationMetadata class_metadata~%uint16 frame_counter~%arbeClassSingleQuery[] queries~%uint16 mutual_len~%~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeClassificationMetadata~%bool enable_tta~%bool post_factum_mode~%bool average_mode~%bool tta_decay~%float32 tta_momentum~%float32 conf_TH~%~%================================================================================~%MSG: arbe_msgs/arbeClassSingleQuery~%uint16 obj_id~%uint16 obj_index~%uint16 n_points~%float32[] xyzvp~%float32[] xyzvp_center~%float32[] tta_scores~%uint32 tta_count~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeNewClassFrameQueries)))
  "Returns full string definition for message of type 'arbeNewClassFrameQueries"
  (cl:format cl:nil "std_msgs/Header Header~%arbeClassificationMetadata class_metadata~%uint16 frame_counter~%arbeClassSingleQuery[] queries~%uint16 mutual_len~%~%~%================================================================================~%MSG: std_msgs/Header~%# Standard metadata for higher-level stamped data types.~%# This is generally used to communicate timestamped data ~%# in a particular coordinate frame.~%# ~%# sequence ID: consecutively increasing ID ~%uint32 seq~%#Two-integer timestamp that is expressed as:~%# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')~%# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')~%# time-handling sugar is provided by the client library~%time stamp~%#Frame this data is associated with~%string frame_id~%~%================================================================================~%MSG: arbe_msgs/arbeClassificationMetadata~%bool enable_tta~%bool post_factum_mode~%bool average_mode~%bool tta_decay~%float32 tta_momentum~%float32 conf_TH~%~%================================================================================~%MSG: arbe_msgs/arbeClassSingleQuery~%uint16 obj_id~%uint16 obj_index~%uint16 n_points~%float32[] xyzvp~%float32[] xyzvp_center~%float32[] tta_scores~%uint32 tta_count~%~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeNewClassFrameQueries>))
  (cl:+ 0
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'Header))
     (roslisp-msg-protocol:serialization-length (cl:slot-value msg 'class_metadata))
     2
     4 (cl:reduce #'cl:+ (cl:slot-value msg 'queries) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ (roslisp-msg-protocol:serialization-length ele))))
     2
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeNewClassFrameQueries>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeNewClassFrameQueries
    (cl:cons ':Header (Header msg))
    (cl:cons ':class_metadata (class_metadata msg))
    (cl:cons ':frame_counter (frame_counter msg))
    (cl:cons ':queries (queries msg))
    (cl:cons ':mutual_len (mutual_len msg))
))
