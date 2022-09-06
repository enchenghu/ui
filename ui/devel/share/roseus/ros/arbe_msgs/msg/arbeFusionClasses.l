;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeFusionClasses)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeFusionClasses (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEFUSIONCLASSES")
  (make-package "ARBE_MSGS::ARBEFUSIONCLASSES"))

(in-package "ROS")
;;//! \htmlinclude arbeFusionClasses.msg.html


(intern "*FC_BIKE*" (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(shadow '*FC_BIKE* (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(defconstant arbe_msgs::arbeFusionClasses::*FC_BIKE* 0)
(intern "*FC_TRUCK_BUS*" (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(shadow '*FC_TRUCK_BUS* (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(defconstant arbe_msgs::arbeFusionClasses::*FC_TRUCK_BUS* 1)
(intern "*FC_VRU*" (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(shadow '*FC_VRU* (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(defconstant arbe_msgs::arbeFusionClasses::*FC_VRU* 2)
(intern "*FC_CAR*" (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(shadow '*FC_CAR* (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(defconstant arbe_msgs::arbeFusionClasses::*FC_CAR* 3)
(intern "*FC_NOT_CLASSIFIED*" (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(shadow '*FC_NOT_CLASSIFIED* (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(defconstant arbe_msgs::arbeFusionClasses::*FC_NOT_CLASSIFIED* 4)
(intern "*NUM_CLASSES*" (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(shadow '*NUM_CLASSES* (find-package "ARBE_MSGS::ARBEFUSIONCLASSES"))
(defconstant arbe_msgs::arbeFusionClasses::*NUM_CLASSES* 4)
(defclass arbe_msgs::arbeFusionClasses
  :super ros::object
  :slots ())

(defmethod arbe_msgs::arbeFusionClasses
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

(setf (get arbe_msgs::arbeFusionClasses :md5sum-) "c824281429998cf35dc2364ca411d089")
(setf (get arbe_msgs::arbeFusionClasses :datatype-) "arbe_msgs/arbeFusionClasses")
(setf (get arbe_msgs::arbeFusionClasses :definition-)
      "uint16 FC_BIKE = 0
uint16 FC_TRUCK_BUS = 1
uint16 FC_VRU = 2
uint16 FC_CAR = 3
uint16 FC_NOT_CLASSIFIED = 4
uint16 NUM_CLASSES = 4

")



(provide :arbe_msgs/arbeFusionClasses "c824281429998cf35dc2364ca411d089")


