;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbePcWithValid)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbePcWithValid (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEPCWITHVALID")
  (make-package "ARBE_MSGS::ARBEPCWITHVALID"))

(in-package "ROS")
;;//! \htmlinclude arbePcWithValid.msg.html


(defclass arbe_msgs::arbePcWithValid
  :super ros::object
  :slots (_pc _valid ))

(defmethod arbe_msgs::arbePcWithValid
  (:init
   (&key
    ((:pc __pc) (instance arbe_msgs::arbeNewPcMsg :init))
    ((:valid __valid) (let (r) (dotimes (i 0) (push nil r)) r))
    )
   (send-super :init)
   (setq _pc __pc)
   (setq _valid __valid)
   self)
  (:pc
   (&rest __pc)
   (if (keywordp (car __pc))
       (send* _pc __pc)
     (progn
       (if __pc (setq _pc (car __pc)))
       _pc)))
  (:valid
   (&optional __valid)
   (if __valid (setq _valid __valid)) _valid)
  (:serialization-length
   ()
   (+
    ;; arbe_msgs/arbeNewPcMsg _pc
    (send _pc :serialization-length)
    ;; bool[] _valid
    (* 1    (length _valid)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; arbe_msgs/arbeNewPcMsg _pc
       (send _pc :serialize s)
     ;; bool[] _valid
     (write-long (length _valid) s)
     (dotimes (i (length _valid))
       (if (elt _valid i) (write-byte -1 s) (write-byte 0 s))
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; arbe_msgs/arbeNewPcMsg _pc
     (send _pc :deserialize buf ptr-) (incf ptr- (send _pc :serialization-length))
   ;; bool[] _valid
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _valid (make-list n))
     (dotimes (i n)
     (setf (elt _valid i) (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbePcWithValid :md5sum-) "e4ea8e0ece0df084b29db472b924eebf")
(setf (get arbe_msgs::arbePcWithValid :datatype-) "arbe_msgs/arbePcWithValid")
(setf (get arbe_msgs::arbePcWithValid :definition-)
      "arbeNewPcMsg pc
bool[] valid

================================================================================
MSG: arbe_msgs/arbeNewPcMsg
std_msgs/Header RosHeader
arbePcFrameHeader PcHeader
arbePcMetadata PcMetadata
arbePcBins Points

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
MSG: arbe_msgs/arbePcFrameHeader
uint64 time
uint16 frame_counter
uint8  is_4d
uint8  frame_type
uint16 udp_lost_packages
uint32 number_of_points
uint16 crd_count
arbeFrameTypes frame_types

================================================================================
MSG: arbe_msgs/arbeFrameTypes
uint8 Frame_Type_3d_short = 1
uint8 Frame_Type_3d_mid = 2
uint8 Frame_Type_3d_long = 3
uint8 Frame_Type_3d_ultra_long = 13
uint8 Frame_Type_4d_short = 4
uint8 Frame_Type_4d_mid = 5
uint8 Frame_Type_4d_long = 6
uint8 Frame_Type_4d_ultra_long = 16

================================================================================
MSG: arbe_msgs/arbePcMetadata
arbePcRes PcResolution
float32 range_offset
float32 range_res_in_db
uint32 azimuth_fft_size
uint32 elevation_fft_size
uint32 azimuth_fft_padding
uint32 elevation_fft_padding


================================================================================
MSG: arbe_msgs/arbePcRes
float32 range_resolution
float32 doppler_resolution
float32 azimuth_coefficient
float32 elevation_coefficient



================================================================================
MSG: arbe_msgs/arbePcBins
uint16[] range_bin
int16[] doppler_signed_bin
int8[] azimuth_signed_bin
int8[] elevation_signed_bin
int16[] phase
int16[] power



")



(provide :arbe_msgs/arbePcWithValid "e4ea8e0ece0df084b29db472b924eebf")


