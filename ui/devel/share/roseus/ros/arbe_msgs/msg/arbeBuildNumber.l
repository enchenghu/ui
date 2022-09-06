;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeBuildNumber)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeBuildNumber (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEBUILDNUMBER")
  (make-package "ARBE_MSGS::ARBEBUILDNUMBER"))

(in-package "ROS")
;;//! \htmlinclude arbeBuildNumber.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeBuildNumber
  :super ros::object
  :slots (_header _slam _arbe_gui _arbe_api_common _arbe_msgs _radar ))

(defmethod arbe_msgs::arbeBuildNumber
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:slam __slam) 0)
    ((:arbe_gui __arbe_gui) 0)
    ((:arbe_api_common __arbe_api_common) 0)
    ((:arbe_msgs __arbe_msgs) 0)
    ((:radar __radar) 0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _slam (round __slam))
   (setq _arbe_gui (round __arbe_gui))
   (setq _arbe_api_common (round __arbe_api_common))
   (setq _arbe_msgs (round __arbe_msgs))
   (setq _radar (round __radar))
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:slam
   (&optional __slam)
   (if __slam (setq _slam __slam)) _slam)
  (:arbe_gui
   (&optional __arbe_gui)
   (if __arbe_gui (setq _arbe_gui __arbe_gui)) _arbe_gui)
  (:arbe_api_common
   (&optional __arbe_api_common)
   (if __arbe_api_common (setq _arbe_api_common __arbe_api_common)) _arbe_api_common)
  (:arbe_msgs
   (&optional __arbe_msgs)
   (if __arbe_msgs (setq _arbe_msgs __arbe_msgs)) _arbe_msgs)
  (:radar
   (&optional __radar)
   (if __radar (setq _radar __radar)) _radar)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; int32 _slam
    4
    ;; int32 _arbe_gui
    4
    ;; int32 _arbe_api_common
    4
    ;; int32 _arbe_msgs
    4
    ;; int32 _radar
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; int32 _slam
       (write-long _slam s)
     ;; int32 _arbe_gui
       (write-long _arbe_gui s)
     ;; int32 _arbe_api_common
       (write-long _arbe_api_common s)
     ;; int32 _arbe_msgs
       (write-long _arbe_msgs s)
     ;; int32 _radar
       (write-long _radar s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; int32 _slam
     (setq _slam (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _arbe_gui
     (setq _arbe_gui (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _arbe_api_common
     (setq _arbe_api_common (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _arbe_msgs
     (setq _arbe_msgs (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; int32 _radar
     (setq _radar (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeBuildNumber :md5sum-) "45c45703c45e662d3433cd39dca68964")
(setf (get arbe_msgs::arbeBuildNumber :datatype-) "arbe_msgs/arbeBuildNumber")
(setf (get arbe_msgs::arbeBuildNumber :definition-)
      "std_msgs/Header header
int32 slam
int32 arbe_gui
int32 arbe_api_common
int32 arbe_msgs
int32 radar

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

")



(provide :arbe_msgs/arbeBuildNumber "45c45703c45e662d3433cd39dca68964")


