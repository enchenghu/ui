;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeBoolWithTime)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeBoolWithTime (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEBOOLWITHTIME")
  (make-package "ARBE_MSGS::ARBEBOOLWITHTIME"))

(in-package "ROS")
;;//! \htmlinclude arbeBoolWithTime.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeBoolWithTime
  :super ros::object
  :slots (_header _flag ))

(defmethod arbe_msgs::arbeBoolWithTime
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:flag __flag) nil)
    )
   (send-super :init)
   (setq _header __header)
   (setq _flag __flag)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:flag
   (&optional __flag)
   (if __flag (setq _flag __flag)) _flag)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; bool _flag
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; bool _flag
       (if _flag (write-byte -1 s) (write-byte 0 s))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; bool _flag
     (setq _flag (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeBoolWithTime :md5sum-) "1faeb665af69705369ce1bafa748a16c")
(setf (get arbe_msgs::arbeBoolWithTime :datatype-) "arbe_msgs/arbeBoolWithTime")
(setf (get arbe_msgs::arbeBoolWithTime :definition-)
      "std_msgs/Header header
bool flag


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



(provide :arbe_msgs/arbeBoolWithTime "1faeb665af69705369ce1bafa748a16c")


