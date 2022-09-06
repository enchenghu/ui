; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeFusionClasses.msg.html

(cl:defclass <arbeFusionClasses> (roslisp-msg-protocol:ros-message)
  ()
)

(cl:defclass arbeFusionClasses (<arbeFusionClasses>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeFusionClasses>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeFusionClasses)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeFusionClasses> is deprecated: use arbe_msgs-msg:arbeFusionClasses instead.")))
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql '<arbeFusionClasses>)))
    "Constants for message type '<arbeFusionClasses>"
  '((:FC_BIKE . 0)
    (:FC_TRUCK_BUS . 1)
    (:FC_VRU . 2)
    (:FC_CAR . 3)
    (:FC_NOT_CLASSIFIED . 4)
    (:NUM_CLASSES . 4))
)
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql 'arbeFusionClasses)))
    "Constants for message type 'arbeFusionClasses"
  '((:FC_BIKE . 0)
    (:FC_TRUCK_BUS . 1)
    (:FC_VRU . 2)
    (:FC_CAR . 3)
    (:FC_NOT_CLASSIFIED . 4)
    (:NUM_CLASSES . 4))
)
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeFusionClasses>) ostream)
  "Serializes a message object of type '<arbeFusionClasses>"
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeFusionClasses>) istream)
  "Deserializes a message object of type '<arbeFusionClasses>"
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeFusionClasses>)))
  "Returns string type for a message object of type '<arbeFusionClasses>"
  "arbe_msgs/arbeFusionClasses")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeFusionClasses)))
  "Returns string type for a message object of type 'arbeFusionClasses"
  "arbe_msgs/arbeFusionClasses")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeFusionClasses>)))
  "Returns md5sum for a message object of type '<arbeFusionClasses>"
  "c824281429998cf35dc2364ca411d089")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeFusionClasses)))
  "Returns md5sum for a message object of type 'arbeFusionClasses"
  "c824281429998cf35dc2364ca411d089")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeFusionClasses>)))
  "Returns full string definition for message of type '<arbeFusionClasses>"
  (cl:format cl:nil "uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeFusionClasses)))
  "Returns full string definition for message of type 'arbeFusionClasses"
  (cl:format cl:nil "uint16 FC_BIKE = 0~%uint16 FC_TRUCK_BUS = 1~%uint16 FC_VRU = 2~%uint16 FC_CAR = 3~%uint16 FC_NOT_CLASSIFIED = 4~%uint16 NUM_CLASSES = 4~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeFusionClasses>))
  (cl:+ 0
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeFusionClasses>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeFusionClasses
))
