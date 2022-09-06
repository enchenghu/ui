;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeRdInclination)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeRdInclination (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBERDINCLINATION")
  (make-package "ARBE_MSGS::ARBERDINCLINATION"))

(in-package "ROS")
;;//! \htmlinclude arbeRdInclination.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeRdInclination
  :super ros::object
  :slots (_header _ant_height _ant_tilt ))

(defmethod arbe_msgs::arbeRdInclination
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:ant_height __ant_height) 0.0)
    ((:ant_tilt __ant_tilt) 0.0)
    )
   (send-super :init)
   (setq _header __header)
   (setq _ant_height (float __ant_height))
   (setq _ant_tilt (float __ant_tilt))
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:ant_height
   (&optional __ant_height)
   (if __ant_height (setq _ant_height __ant_height)) _ant_height)
  (:ant_tilt
   (&optional __ant_tilt)
   (if __ant_tilt (setq _ant_tilt __ant_tilt)) _ant_tilt)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; float32 _ant_height
    4
    ;; float32 _ant_tilt
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; float32 _ant_height
       (sys::poke _ant_height (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _ant_tilt
       (sys::poke _ant_tilt (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; float32 _ant_height
     (setq _ant_height (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _ant_tilt
     (setq _ant_tilt (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeRdInclination :md5sum-) "401e82b1a597051854c2fb29fdf032ba")
(setf (get arbe_msgs::arbeRdInclination :datatype-) "arbe_msgs/arbeRdInclination")
(setf (get arbe_msgs::arbeRdInclination :definition-)
      "std_msgs/Header header
float32 ant_height
float32 ant_tilt

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



(provide :arbe_msgs/arbeRdInclination "401e82b1a597051854c2fb29fdf032ba")


