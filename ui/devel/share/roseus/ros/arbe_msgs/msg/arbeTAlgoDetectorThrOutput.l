;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeTAlgoDetectorThrOutput)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeTAlgoDetectorThrOutput (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBETALGODETECTORTHROUTPUT")
  (make-package "ARBE_MSGS::ARBETALGODETECTORTHROUTPUT"))

(in-package "ROS")
;;//! \htmlinclude arbeTAlgoDetectorThrOutput.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeTAlgoDetectorThrOutput
  :super ros::object
  :slots (_RosHeader _tinfo ))

(defmethod arbe_msgs::arbeTAlgoDetectorThrOutput
  (:init
   (&key
    ((:RosHeader __RosHeader) (instance std_msgs::Header :init))
    ((:tinfo __tinfo) (instance arbe_msgs::arbeTAlgoDetectorThrInfo :init))
    )
   (send-super :init)
   (setq _RosHeader __RosHeader)
   (setq _tinfo __tinfo)
   self)
  (:RosHeader
   (&rest __RosHeader)
   (if (keywordp (car __RosHeader))
       (send* _RosHeader __RosHeader)
     (progn
       (if __RosHeader (setq _RosHeader (car __RosHeader)))
       _RosHeader)))
  (:tinfo
   (&rest __tinfo)
   (if (keywordp (car __tinfo))
       (send* _tinfo __tinfo)
     (progn
       (if __tinfo (setq _tinfo (car __tinfo)))
       _tinfo)))
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _RosHeader
    (send _RosHeader :serialization-length)
    ;; arbe_msgs/arbeTAlgoDetectorThrInfo _tinfo
    (send _tinfo :serialization-length)
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _RosHeader
       (send _RosHeader :serialize s)
     ;; arbe_msgs/arbeTAlgoDetectorThrInfo _tinfo
       (send _tinfo :serialize s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _RosHeader
     (send _RosHeader :deserialize buf ptr-) (incf ptr- (send _RosHeader :serialization-length))
   ;; arbe_msgs/arbeTAlgoDetectorThrInfo _tinfo
     (send _tinfo :deserialize buf ptr-) (incf ptr- (send _tinfo :serialization-length))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeTAlgoDetectorThrOutput :md5sum-) "888d5fc086e6ad00dbaf413f9befa62b")
(setf (get arbe_msgs::arbeTAlgoDetectorThrOutput :datatype-) "arbe_msgs/arbeTAlgoDetectorThrOutput")
(setf (get arbe_msgs::arbeTAlgoDetectorThrOutput :definition-)
      "std_msgs/Header RosHeader
arbeTAlgoDetectorThrInfo tinfo

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
MSG: arbe_msgs/arbeTAlgoDetectorThrInfo
uint32 unFrameNumber
uint8 eAlgoOutput
uint8 eThresholdType
uint8 eFrameType
uint32[] Data

")



(provide :arbe_msgs/arbeTAlgoDetectorThrOutput "888d5fc086e6ad00dbaf413f9befa62b")


