;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeSlamMsg)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeSlamMsg (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBESLAMMSG")
  (make-package "ARBE_MSGS::ARBESLAMMSG"))

(in-package "ROS")
;;//! \htmlinclude arbeSlamMsg.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(intern "*FC_BIKE*" (find-package "ARBE_MSGS::ARBESLAMMSG"))
(shadow '*FC_BIKE* (find-package "ARBE_MSGS::ARBESLAMMSG"))
(defconstant arbe_msgs::arbeSlamMsg::*FC_BIKE* 0)
(intern "*FC_TRUCK_BUS*" (find-package "ARBE_MSGS::ARBESLAMMSG"))
(shadow '*FC_TRUCK_BUS* (find-package "ARBE_MSGS::ARBESLAMMSG"))
(defconstant arbe_msgs::arbeSlamMsg::*FC_TRUCK_BUS* 1)
(intern "*FC_VRU*" (find-package "ARBE_MSGS::ARBESLAMMSG"))
(shadow '*FC_VRU* (find-package "ARBE_MSGS::ARBESLAMMSG"))
(defconstant arbe_msgs::arbeSlamMsg::*FC_VRU* 2)
(intern "*FC_CAR*" (find-package "ARBE_MSGS::ARBESLAMMSG"))
(shadow '*FC_CAR* (find-package "ARBE_MSGS::ARBESLAMMSG"))
(defconstant arbe_msgs::arbeSlamMsg::*FC_CAR* 3)
(intern "*FC_NOT_CLASSIFIED*" (find-package "ARBE_MSGS::ARBESLAMMSG"))
(shadow '*FC_NOT_CLASSIFIED* (find-package "ARBE_MSGS::ARBESLAMMSG"))
(defconstant arbe_msgs::arbeSlamMsg::*FC_NOT_CLASSIFIED* 4)
(defclass arbe_msgs::arbeSlamMsg
  :super ros::object
  :slots (_header _meta_data _ObjectsBuffer _fusionClasses ))

(defmethod arbe_msgs::arbeSlamMsg
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:meta_data __meta_data) (instance arbe_msgs::arbeTSlamMetadata :init))
    ((:ObjectsBuffer __ObjectsBuffer) (let (r) (dotimes (i 0) (push (instance arbe_msgs::arbeTSlamObj :init) r)) r))
    ((:fusionClasses __fusionClasses) (instance arbe_msgs::arbeFusionClasses :init))
    )
   (send-super :init)
   (setq _header __header)
   (setq _meta_data __meta_data)
   (setq _ObjectsBuffer __ObjectsBuffer)
   (setq _fusionClasses __fusionClasses)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:meta_data
   (&rest __meta_data)
   (if (keywordp (car __meta_data))
       (send* _meta_data __meta_data)
     (progn
       (if __meta_data (setq _meta_data (car __meta_data)))
       _meta_data)))
  (:ObjectsBuffer
   (&rest __ObjectsBuffer)
   (if (keywordp (car __ObjectsBuffer))
       (send* _ObjectsBuffer __ObjectsBuffer)
     (progn
       (if __ObjectsBuffer (setq _ObjectsBuffer (car __ObjectsBuffer)))
       _ObjectsBuffer)))
  (:fusionClasses
   (&rest __fusionClasses)
   (if (keywordp (car __fusionClasses))
       (send* _fusionClasses __fusionClasses)
     (progn
       (if __fusionClasses (setq _fusionClasses (car __fusionClasses)))
       _fusionClasses)))
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; arbe_msgs/arbeTSlamMetadata _meta_data
    (send _meta_data :serialization-length)
    ;; arbe_msgs/arbeTSlamObj[] _ObjectsBuffer
    (apply #'+ (send-all _ObjectsBuffer :serialization-length)) 4
    ;; arbe_msgs/arbeFusionClasses _fusionClasses
    (send _fusionClasses :serialization-length)
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; arbe_msgs/arbeTSlamMetadata _meta_data
       (send _meta_data :serialize s)
     ;; arbe_msgs/arbeTSlamObj[] _ObjectsBuffer
     (write-long (length _ObjectsBuffer) s)
     (dolist (elem _ObjectsBuffer)
       (send elem :serialize s)
       )
     ;; arbe_msgs/arbeFusionClasses _fusionClasses
       (send _fusionClasses :serialize s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; arbe_msgs/arbeTSlamMetadata _meta_data
     (send _meta_data :deserialize buf ptr-) (incf ptr- (send _meta_data :serialization-length))
   ;; arbe_msgs/arbeTSlamObj[] _ObjectsBuffer
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _ObjectsBuffer (let (r) (dotimes (i n) (push (instance arbe_msgs::arbeTSlamObj :init) r)) r))
     (dolist (elem- _ObjectsBuffer)
     (send elem- :deserialize buf ptr-) (incf ptr- (send elem- :serialization-length))
     ))
   ;; arbe_msgs/arbeFusionClasses _fusionClasses
     (send _fusionClasses :deserialize buf ptr-) (incf ptr- (send _fusionClasses :serialization-length))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeSlamMsg :md5sum-) "ad8727f4b769ffeff212c8a8f681cd8a")
(setf (get arbe_msgs::arbeSlamMsg :datatype-) "arbe_msgs/arbeSlamMsg")
(setf (get arbe_msgs::arbeSlamMsg :definition-)
      "std_msgs/Header header
arbeTSlamMetadata  meta_data
arbeTSlamObj[] ObjectsBuffer

arbeFusionClasses fusionClasses
uint16 FC_BIKE = 0
uint16 FC_TRUCK_BUS = 1
uint16 FC_VRU = 2
uint16 FC_CAR = 3
uint16 FC_NOT_CLASSIFIED = 4

================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
string frame_id

================================================================================
MSG: arbe_msgs/arbeTSlamMetadata
uint64  CorresPcTime
uint16  FrameNumber
float32 HostHeading
float32 HostHeadingUnc
float32 HostVelocity
float32 HostVelocityUnc
float32 HostOmega		
float32 HostOmegaUnc
uint16  NumberOfObjects
float32 local_catr_x
float32 local_catr_y
bool    location_valid
bool    is_reset_geo_location


================================================================================
MSG: arbe_msgs/arbeTSlamObj
uint16              ID
uint16              obj_class
float32             class_conf
arbeTSlamPos        position
arbeTSlamVelocity   velocity 
arbeTSlamBox        bounding_box
uint32              age
uint16              last_frame_update



================================================================================
MSG: arbe_msgs/arbeTSlamPos
float32 x 
float32 y 
float32 z 
float32  dx 
float32  dy 
float32  dz

================================================================================
MSG: arbe_msgs/arbeTSlamVelocity
float32     x_dot 
float32     y_dot 
float32      dx_dot 
float32      dy_dot 

================================================================================
MSG: arbe_msgs/arbeTSlamBox
float32 scale_x     
float32 scale_y     
float32 scale_z     
float32 scale_x_unc 
float32 scale_y_unc 
float32 scale_z_unc 
float32 orientation_unc  
float32 orientation  

================================================================================
MSG: arbe_msgs/arbeFusionClasses
uint16 FC_BIKE = 0
uint16 FC_TRUCK_BUS = 1
uint16 FC_VRU = 2
uint16 FC_CAR = 3
uint16 FC_NOT_CLASSIFIED = 4
uint16 NUM_CLASSES = 4

")



(provide :arbe_msgs/arbeSlamMsg "ad8727f4b769ffeff212c8a8f681cd8a")


