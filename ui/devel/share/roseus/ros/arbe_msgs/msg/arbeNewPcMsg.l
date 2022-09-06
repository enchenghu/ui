;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeNewPcMsg)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeNewPcMsg (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBENEWPCMSG")
  (make-package "ARBE_MSGS::ARBENEWPCMSG"))

(in-package "ROS")
;;//! \htmlinclude arbeNewPcMsg.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeNewPcMsg
  :super ros::object
  :slots (_RosHeader _PcHeader _PcMetadata _Points ))

(defmethod arbe_msgs::arbeNewPcMsg
  (:init
   (&key
    ((:RosHeader __RosHeader) (instance std_msgs::Header :init))
    ((:PcHeader __PcHeader) (instance arbe_msgs::arbePcFrameHeader :init))
    ((:PcMetadata __PcMetadata) (instance arbe_msgs::arbePcMetadata :init))
    ((:Points __Points) (instance arbe_msgs::arbePcBins :init))
    )
   (send-super :init)
   (setq _RosHeader __RosHeader)
   (setq _PcHeader __PcHeader)
   (setq _PcMetadata __PcMetadata)
   (setq _Points __Points)
   self)
  (:RosHeader
   (&rest __RosHeader)
   (if (keywordp (car __RosHeader))
       (send* _RosHeader __RosHeader)
     (progn
       (if __RosHeader (setq _RosHeader (car __RosHeader)))
       _RosHeader)))
  (:PcHeader
   (&rest __PcHeader)
   (if (keywordp (car __PcHeader))
       (send* _PcHeader __PcHeader)
     (progn
       (if __PcHeader (setq _PcHeader (car __PcHeader)))
       _PcHeader)))
  (:PcMetadata
   (&rest __PcMetadata)
   (if (keywordp (car __PcMetadata))
       (send* _PcMetadata __PcMetadata)
     (progn
       (if __PcMetadata (setq _PcMetadata (car __PcMetadata)))
       _PcMetadata)))
  (:Points
   (&rest __Points)
   (if (keywordp (car __Points))
       (send* _Points __Points)
     (progn
       (if __Points (setq _Points (car __Points)))
       _Points)))
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _RosHeader
    (send _RosHeader :serialization-length)
    ;; arbe_msgs/arbePcFrameHeader _PcHeader
    (send _PcHeader :serialization-length)
    ;; arbe_msgs/arbePcMetadata _PcMetadata
    (send _PcMetadata :serialization-length)
    ;; arbe_msgs/arbePcBins _Points
    (send _Points :serialization-length)
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _RosHeader
       (send _RosHeader :serialize s)
     ;; arbe_msgs/arbePcFrameHeader _PcHeader
       (send _PcHeader :serialize s)
     ;; arbe_msgs/arbePcMetadata _PcMetadata
       (send _PcMetadata :serialize s)
     ;; arbe_msgs/arbePcBins _Points
       (send _Points :serialize s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _RosHeader
     (send _RosHeader :deserialize buf ptr-) (incf ptr- (send _RosHeader :serialization-length))
   ;; arbe_msgs/arbePcFrameHeader _PcHeader
     (send _PcHeader :deserialize buf ptr-) (incf ptr- (send _PcHeader :serialization-length))
   ;; arbe_msgs/arbePcMetadata _PcMetadata
     (send _PcMetadata :deserialize buf ptr-) (incf ptr- (send _PcMetadata :serialization-length))
   ;; arbe_msgs/arbePcBins _Points
     (send _Points :deserialize buf ptr-) (incf ptr- (send _Points :serialization-length))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeNewPcMsg :md5sum-) "6537ae358990354b634606bcfb0fa7db")
(setf (get arbe_msgs::arbeNewPcMsg :datatype-) "arbe_msgs/arbeNewPcMsg")
(setf (get arbe_msgs::arbeNewPcMsg :definition-)
      "std_msgs/Header RosHeader
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



(provide :arbe_msgs/arbeNewPcMsg "6537ae358990354b634606bcfb0fa7db")


