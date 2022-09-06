;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbePolarFreeSpacePoly)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbePolarFreeSpacePoly (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEPOLARFREESPACEPOLY")
  (make-package "ARBE_MSGS::ARBEPOLARFREESPACEPOLY"))

(in-package "ROS")
;;//! \htmlinclude arbePolarFreeSpacePoly.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbePolarFreeSpacePoly
  :super ros::object
  :slots (_header _frameCounter _rPerAzimuth ))

(defmethod arbe_msgs::arbePolarFreeSpacePoly
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:frameCounter __frameCounter) 0)
    ((:rPerAzimuth __rPerAzimuth) (make-array 0 :initial-element 0.0 :element-type :float))
    )
   (send-super :init)
   (setq _header __header)
   (setq _frameCounter (round __frameCounter))
   (setq _rPerAzimuth __rPerAzimuth)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:frameCounter
   (&optional __frameCounter)
   (if __frameCounter (setq _frameCounter __frameCounter)) _frameCounter)
  (:rPerAzimuth
   (&optional __rPerAzimuth)
   (if __rPerAzimuth (setq _rPerAzimuth __rPerAzimuth)) _rPerAzimuth)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; uint16 _frameCounter
    2
    ;; float32[] _rPerAzimuth
    (* 4    (length _rPerAzimuth)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; uint16 _frameCounter
       (write-word _frameCounter s)
     ;; float32[] _rPerAzimuth
     (write-long (length _rPerAzimuth) s)
     (dotimes (i (length _rPerAzimuth))
       (sys::poke (elt _rPerAzimuth i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; uint16 _frameCounter
     (setq _frameCounter (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; float32[] _rPerAzimuth
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _rPerAzimuth (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _rPerAzimuth i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbePolarFreeSpacePoly :md5sum-) "3e4fe2f26c99bce1f836beb8bd1176f9")
(setf (get arbe_msgs::arbePolarFreeSpacePoly :datatype-) "arbe_msgs/arbePolarFreeSpacePoly")
(setf (get arbe_msgs::arbePolarFreeSpacePoly :definition-)
      "std_msgs/Header header
uint16 frameCounter
float32[] rPerAzimuth

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



(provide :arbe_msgs/arbePolarFreeSpacePoly "3e4fe2f26c99bce1f836beb8bd1176f9")


