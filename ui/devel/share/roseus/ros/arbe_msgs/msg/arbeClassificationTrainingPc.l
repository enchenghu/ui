;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeClassificationTrainingPc)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeClassificationTrainingPc (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBECLASSIFICATIONTRAININGPC")
  (make-package "ARBE_MSGS::ARBECLASSIFICATIONTRAININGPC"))

(in-package "ROS")
;;//! \htmlinclude arbeClassificationTrainingPc.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeClassificationTrainingPc
  :super ros::object
  :slots (_Header _Pc _fusionResponse _matching_com_indices ))

(defmethod arbe_msgs::arbeClassificationTrainingPc
  (:init
   (&key
    ((:Header __Header) (instance std_msgs::Header :init))
    ((:Pc __Pc) (instance arbe_msgs::arbeNewPcMsg :init))
    ((:fusionResponse __fusionResponse) (instance arbe_msgs::arbeFusionResponse :init))
    ((:matching_com_indices __matching_com_indices) (make-array 0 :initial-element 0 :element-type :integer))
    )
   (send-super :init)
   (setq _Header __Header)
   (setq _Pc __Pc)
   (setq _fusionResponse __fusionResponse)
   (setq _matching_com_indices __matching_com_indices)
   self)
  (:Header
   (&rest __Header)
   (if (keywordp (car __Header))
       (send* _Header __Header)
     (progn
       (if __Header (setq _Header (car __Header)))
       _Header)))
  (:Pc
   (&rest __Pc)
   (if (keywordp (car __Pc))
       (send* _Pc __Pc)
     (progn
       (if __Pc (setq _Pc (car __Pc)))
       _Pc)))
  (:fusionResponse
   (&rest __fusionResponse)
   (if (keywordp (car __fusionResponse))
       (send* _fusionResponse __fusionResponse)
     (progn
       (if __fusionResponse (setq _fusionResponse (car __fusionResponse)))
       _fusionResponse)))
  (:matching_com_indices
   (&optional __matching_com_indices)
   (if __matching_com_indices (setq _matching_com_indices __matching_com_indices)) _matching_com_indices)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _Header
    (send _Header :serialization-length)
    ;; arbe_msgs/arbeNewPcMsg _Pc
    (send _Pc :serialization-length)
    ;; arbe_msgs/arbeFusionResponse _fusionResponse
    (send _fusionResponse :serialization-length)
    ;; uint16[] _matching_com_indices
    (* 2    (length _matching_com_indices)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _Header
       (send _Header :serialize s)
     ;; arbe_msgs/arbeNewPcMsg _Pc
       (send _Pc :serialize s)
     ;; arbe_msgs/arbeFusionResponse _fusionResponse
       (send _fusionResponse :serialize s)
     ;; uint16[] _matching_com_indices
     (write-long (length _matching_com_indices) s)
     (dotimes (i (length _matching_com_indices))
       (write-word (elt _matching_com_indices i) s)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _Header
     (send _Header :deserialize buf ptr-) (incf ptr- (send _Header :serialization-length))
   ;; arbe_msgs/arbeNewPcMsg _Pc
     (send _Pc :deserialize buf ptr-) (incf ptr- (send _Pc :serialization-length))
   ;; arbe_msgs/arbeFusionResponse _fusionResponse
     (send _fusionResponse :deserialize buf ptr-) (incf ptr- (send _fusionResponse :serialization-length))
   ;; uint16[] _matching_com_indices
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _matching_com_indices (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _matching_com_indices i) (sys::peek buf ptr- :short)) (incf ptr- 2)
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeClassificationTrainingPc :md5sum-) "e9b7fe5d9944489810bf1fbe1b2cac94")
(setf (get arbe_msgs::arbeClassificationTrainingPc :datatype-) "arbe_msgs/arbeClassificationTrainingPc")
(setf (get arbe_msgs::arbeClassificationTrainingPc :definition-)
      "std_msgs/Header Header
arbeNewPcMsg Pc
arbeFusionResponse fusionResponse
uint16[] matching_com_indices

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
MSG: arbe_msgs/arbeNewPcMsg
std_msgs/Header RosHeader
arbePcFrameHeader PcHeader
arbePcMetadata PcMetadata
arbePcBins Points

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



================================================================================
MSG: arbe_msgs/arbeFusionResponse
bool valid
uint16 obj_id
uint32 query_id
uint8 fusionClass
uint16 obj_index
uint64 queryTime
float32 conf
uint8 tta_count
float32[] tta_scores
string class_str
arbeFusionClasses fusionClasses

uint16  top
uint16  left
uint16  bottom
uint16  right

================================================================================
MSG: arbe_msgs/arbeFusionClasses
uint16 FC_BIKE = 0
uint16 FC_TRUCK_BUS = 1
uint16 FC_VRU = 2
uint16 FC_CAR = 3
uint16 FC_NOT_CLASSIFIED = 4
uint16 NUM_CLASSES = 4

")



(provide :arbe_msgs/arbeClassificationTrainingPc "e9b7fe5d9944489810bf1fbe1b2cac94")


