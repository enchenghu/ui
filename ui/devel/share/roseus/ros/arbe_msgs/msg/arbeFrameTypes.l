;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeFrameTypes)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeFrameTypes (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEFRAMETYPES")
  (make-package "ARBE_MSGS::ARBEFRAMETYPES"))

(in-package "ROS")
;;//! \htmlinclude arbeFrameTypes.msg.html


(intern "*FRAME_TYPE_3D_SHORT*" (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(shadow '*FRAME_TYPE_3D_SHORT* (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(defconstant arbe_msgs::arbeFrameTypes::*FRAME_TYPE_3D_SHORT* 1)
(intern "*FRAME_TYPE_3D_MID*" (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(shadow '*FRAME_TYPE_3D_MID* (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(defconstant arbe_msgs::arbeFrameTypes::*FRAME_TYPE_3D_MID* 2)
(intern "*FRAME_TYPE_3D_LONG*" (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(shadow '*FRAME_TYPE_3D_LONG* (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(defconstant arbe_msgs::arbeFrameTypes::*FRAME_TYPE_3D_LONG* 3)
(intern "*FRAME_TYPE_3D_ULTRA_LONG*" (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(shadow '*FRAME_TYPE_3D_ULTRA_LONG* (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(defconstant arbe_msgs::arbeFrameTypes::*FRAME_TYPE_3D_ULTRA_LONG* 13)
(intern "*FRAME_TYPE_4D_SHORT*" (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(shadow '*FRAME_TYPE_4D_SHORT* (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(defconstant arbe_msgs::arbeFrameTypes::*FRAME_TYPE_4D_SHORT* 4)
(intern "*FRAME_TYPE_4D_MID*" (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(shadow '*FRAME_TYPE_4D_MID* (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(defconstant arbe_msgs::arbeFrameTypes::*FRAME_TYPE_4D_MID* 5)
(intern "*FRAME_TYPE_4D_LONG*" (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(shadow '*FRAME_TYPE_4D_LONG* (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(defconstant arbe_msgs::arbeFrameTypes::*FRAME_TYPE_4D_LONG* 6)
(intern "*FRAME_TYPE_4D_ULTRA_LONG*" (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(shadow '*FRAME_TYPE_4D_ULTRA_LONG* (find-package "ARBE_MSGS::ARBEFRAMETYPES"))
(defconstant arbe_msgs::arbeFrameTypes::*FRAME_TYPE_4D_ULTRA_LONG* 16)
(defclass arbe_msgs::arbeFrameTypes
  :super ros::object
  :slots ())

(defmethod arbe_msgs::arbeFrameTypes
  (:init
   (&key
    )
   (send-super :init)
   self)
  (:serialization-length
   ()
   (+
    0
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeFrameTypes :md5sum-) "60a899865eb222277c4a73fdc879e6e4")
(setf (get arbe_msgs::arbeFrameTypes :datatype-) "arbe_msgs/arbeFrameTypes")
(setf (get arbe_msgs::arbeFrameTypes :definition-)
      "uint8 Frame_Type_3d_short = 1
uint8 Frame_Type_3d_mid = 2
uint8 Frame_Type_3d_long = 3
uint8 Frame_Type_3d_ultra_long = 13
uint8 Frame_Type_4d_short = 4
uint8 Frame_Type_4d_mid = 5
uint8 Frame_Type_4d_long = 6
uint8 Frame_Type_4d_ultra_long = 16

")



(provide :arbe_msgs/arbeFrameTypes "60a899865eb222277c4a73fdc879e6e4")


