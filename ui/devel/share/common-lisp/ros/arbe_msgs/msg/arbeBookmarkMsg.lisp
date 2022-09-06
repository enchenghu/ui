; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeBookmarkMsg.msg.html

(cl:defclass <arbeBookmarkMsg> (roslisp-msg-protocol:ros-message)
  ((frame_num
    :reader frame_num
    :initarg :frame_num
    :type cl:fixnum
    :initform 0)
   (egoVel
    :reader egoVel
    :initarg :egoVel
    :type cl:float
    :initform 0.0))
)

(cl:defclass arbeBookmarkMsg (<arbeBookmarkMsg>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeBookmarkMsg>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeBookmarkMsg)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeBookmarkMsg> is deprecated: use arbe_msgs-msg:arbeBookmarkMsg instead.")))

(cl:ensure-generic-function 'frame_num-val :lambda-list '(m))
(cl:defmethod frame_num-val ((m <arbeBookmarkMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:frame_num-val is deprecated.  Use arbe_msgs-msg:frame_num instead.")
  (frame_num m))

(cl:ensure-generic-function 'egoVel-val :lambda-list '(m))
(cl:defmethod egoVel-val ((m <arbeBookmarkMsg>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:egoVel-val is deprecated.  Use arbe_msgs-msg:egoVel instead.")
  (egoVel m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeBookmarkMsg>) ostream)
  "Serializes a message object of type '<arbeBookmarkMsg>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_num)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_num)) ostream)
  (cl:let ((bits (roslisp-utils:encode-single-float-bits (cl:slot-value msg 'egoVel))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeBookmarkMsg>) istream)
  "Deserializes a message object of type '<arbeBookmarkMsg>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_num)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_num)) (cl:read-byte istream))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'egoVel) (roslisp-utils:decode-single-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeBookmarkMsg>)))
  "Returns string type for a message object of type '<arbeBookmarkMsg>"
  "arbe_msgs/arbeBookmarkMsg")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeBookmarkMsg)))
  "Returns string type for a message object of type 'arbeBookmarkMsg"
  "arbe_msgs/arbeBookmarkMsg")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeBookmarkMsg>)))
  "Returns md5sum for a message object of type '<arbeBookmarkMsg>"
  "968e0faad3956fde6fdb61e0a1f108a6")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeBookmarkMsg)))
  "Returns md5sum for a message object of type 'arbeBookmarkMsg"
  "968e0faad3956fde6fdb61e0a1f108a6")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeBookmarkMsg>)))
  "Returns full string definition for message of type '<arbeBookmarkMsg>"
  (cl:format cl:nil "uint16 frame_num~%float32 egoVel~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeBookmarkMsg)))
  "Returns full string definition for message of type 'arbeBookmarkMsg"
  (cl:format cl:nil "uint16 frame_num~%float32 egoVel~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeBookmarkMsg>))
  (cl:+ 0
     2
     4
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeBookmarkMsg>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeBookmarkMsg
    (cl:cons ':frame_num (frame_num msg))
    (cl:cons ':egoVel (egoVel msg))
))
