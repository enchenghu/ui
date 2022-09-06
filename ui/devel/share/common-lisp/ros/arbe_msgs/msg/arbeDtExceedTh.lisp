; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeDtExceedTh.msg.html

(cl:defclass <arbeDtExceedTh> (roslisp-msg-protocol:ros-message)
  ((frame_counter
    :reader frame_counter
    :initarg :frame_counter
    :type cl:fixnum
    :initform 0)
   (dt
    :reader dt
    :initarg :dt
    :type cl:integer
    :initform 0))
)

(cl:defclass arbeDtExceedTh (<arbeDtExceedTh>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeDtExceedTh>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeDtExceedTh)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeDtExceedTh> is deprecated: use arbe_msgs-msg:arbeDtExceedTh instead.")))

(cl:ensure-generic-function 'frame_counter-val :lambda-list '(m))
(cl:defmethod frame_counter-val ((m <arbeDtExceedTh>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:frame_counter-val is deprecated.  Use arbe_msgs-msg:frame_counter instead.")
  (frame_counter m))

(cl:ensure-generic-function 'dt-val :lambda-list '(m))
(cl:defmethod dt-val ((m <arbeDtExceedTh>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader arbe_msgs-msg:dt-val is deprecated.  Use arbe_msgs-msg:dt instead.")
  (dt m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeDtExceedTh>) ostream)
  "Serializes a message object of type '<arbeDtExceedTh>"
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_counter)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_counter)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'dt)) ostream)
  (cl:write-byte (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'dt)) ostream)
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeDtExceedTh>) istream)
  "Deserializes a message object of type '<arbeDtExceedTh>"
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'frame_counter)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'frame_counter)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 0) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 8) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 16) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 24) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 32) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 40) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 48) (cl:slot-value msg 'dt)) (cl:read-byte istream))
    (cl:setf (cl:ldb (cl:byte 8 56) (cl:slot-value msg 'dt)) (cl:read-byte istream))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeDtExceedTh>)))
  "Returns string type for a message object of type '<arbeDtExceedTh>"
  "arbe_msgs/arbeDtExceedTh")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeDtExceedTh)))
  "Returns string type for a message object of type 'arbeDtExceedTh"
  "arbe_msgs/arbeDtExceedTh")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeDtExceedTh>)))
  "Returns md5sum for a message object of type '<arbeDtExceedTh>"
  "b3c2831d3d6ab52b0524ebb559a84899")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeDtExceedTh)))
  "Returns md5sum for a message object of type 'arbeDtExceedTh"
  "b3c2831d3d6ab52b0524ebb559a84899")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeDtExceedTh>)))
  "Returns full string definition for message of type '<arbeDtExceedTh>"
  (cl:format cl:nil "uint16 frame_counter~%uint64 dt~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeDtExceedTh)))
  "Returns full string definition for message of type 'arbeDtExceedTh"
  (cl:format cl:nil "uint16 frame_counter~%uint64 dt~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeDtExceedTh>))
  (cl:+ 0
     2
     8
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeDtExceedTh>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeDtExceedTh
    (cl:cons ':frame_counter (frame_counter msg))
    (cl:cons ':dt (dt msg))
))
