; Auto-generated. Do not edit!


(cl:in-package arbe_msgs-msg)


;//! \htmlinclude arbeFrameTypes.msg.html

(cl:defclass <arbeFrameTypes> (roslisp-msg-protocol:ros-message)
  ()
)

(cl:defclass arbeFrameTypes (<arbeFrameTypes>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <arbeFrameTypes>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'arbeFrameTypes)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name arbe_msgs-msg:<arbeFrameTypes> is deprecated: use arbe_msgs-msg:arbeFrameTypes instead.")))
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql '<arbeFrameTypes>)))
    "Constants for message type '<arbeFrameTypes>"
  '((:FRAME_TYPE_3D_SHORT . 1)
    (:FRAME_TYPE_3D_MID . 2)
    (:FRAME_TYPE_3D_LONG . 3)
    (:FRAME_TYPE_3D_ULTRA_LONG . 13)
    (:FRAME_TYPE_4D_SHORT . 4)
    (:FRAME_TYPE_4D_MID . 5)
    (:FRAME_TYPE_4D_LONG . 6)
    (:FRAME_TYPE_4D_ULTRA_LONG . 16))
)
(cl:defmethod roslisp-msg-protocol:symbol-codes ((msg-type (cl:eql 'arbeFrameTypes)))
    "Constants for message type 'arbeFrameTypes"
  '((:FRAME_TYPE_3D_SHORT . 1)
    (:FRAME_TYPE_3D_MID . 2)
    (:FRAME_TYPE_3D_LONG . 3)
    (:FRAME_TYPE_3D_ULTRA_LONG . 13)
    (:FRAME_TYPE_4D_SHORT . 4)
    (:FRAME_TYPE_4D_MID . 5)
    (:FRAME_TYPE_4D_LONG . 6)
    (:FRAME_TYPE_4D_ULTRA_LONG . 16))
)
(cl:defmethod roslisp-msg-protocol:serialize ((msg <arbeFrameTypes>) ostream)
  "Serializes a message object of type '<arbeFrameTypes>"
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <arbeFrameTypes>) istream)
  "Deserializes a message object of type '<arbeFrameTypes>"
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<arbeFrameTypes>)))
  "Returns string type for a message object of type '<arbeFrameTypes>"
  "arbe_msgs/arbeFrameTypes")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'arbeFrameTypes)))
  "Returns string type for a message object of type 'arbeFrameTypes"
  "arbe_msgs/arbeFrameTypes")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<arbeFrameTypes>)))
  "Returns md5sum for a message object of type '<arbeFrameTypes>"
  "60a899865eb222277c4a73fdc879e6e4")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'arbeFrameTypes)))
  "Returns md5sum for a message object of type 'arbeFrameTypes"
  "60a899865eb222277c4a73fdc879e6e4")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<arbeFrameTypes>)))
  "Returns full string definition for message of type '<arbeFrameTypes>"
  (cl:format cl:nil "uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'arbeFrameTypes)))
  "Returns full string definition for message of type 'arbeFrameTypes"
  (cl:format cl:nil "uint8 Frame_Type_3d_short = 1~%uint8 Frame_Type_3d_mid = 2~%uint8 Frame_Type_3d_long = 3~%uint8 Frame_Type_3d_ultra_long = 13~%uint8 Frame_Type_4d_short = 4~%uint8 Frame_Type_4d_mid = 5~%uint8 Frame_Type_4d_long = 6~%uint8 Frame_Type_4d_ultra_long = 16~%~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <arbeFrameTypes>))
  (cl:+ 0
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <arbeFrameTypes>))
  "Converts a ROS message object to a list"
  (cl:list 'arbeFrameTypes
))
