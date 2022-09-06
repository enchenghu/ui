;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbePcMetadata)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbePcMetadata (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEPCMETADATA")
  (make-package "ARBE_MSGS::ARBEPCMETADATA"))

(in-package "ROS")
;;//! \htmlinclude arbePcMetadata.msg.html


(defclass arbe_msgs::arbePcMetadata
  :super ros::object
  :slots (_PcResolution _range_offset _range_res_in_db _azimuth_fft_size _elevation_fft_size _azimuth_fft_padding _elevation_fft_padding ))

(defmethod arbe_msgs::arbePcMetadata
  (:init
   (&key
    ((:PcResolution __PcResolution) (instance arbe_msgs::arbePcRes :init))
    ((:range_offset __range_offset) 0.0)
    ((:range_res_in_db __range_res_in_db) 0.0)
    ((:azimuth_fft_size __azimuth_fft_size) 0)
    ((:elevation_fft_size __elevation_fft_size) 0)
    ((:azimuth_fft_padding __azimuth_fft_padding) 0)
    ((:elevation_fft_padding __elevation_fft_padding) 0)
    )
   (send-super :init)
   (setq _PcResolution __PcResolution)
   (setq _range_offset (float __range_offset))
   (setq _range_res_in_db (float __range_res_in_db))
   (setq _azimuth_fft_size (round __azimuth_fft_size))
   (setq _elevation_fft_size (round __elevation_fft_size))
   (setq _azimuth_fft_padding (round __azimuth_fft_padding))
   (setq _elevation_fft_padding (round __elevation_fft_padding))
   self)
  (:PcResolution
   (&rest __PcResolution)
   (if (keywordp (car __PcResolution))
       (send* _PcResolution __PcResolution)
     (progn
       (if __PcResolution (setq _PcResolution (car __PcResolution)))
       _PcResolution)))
  (:range_offset
   (&optional __range_offset)
   (if __range_offset (setq _range_offset __range_offset)) _range_offset)
  (:range_res_in_db
   (&optional __range_res_in_db)
   (if __range_res_in_db (setq _range_res_in_db __range_res_in_db)) _range_res_in_db)
  (:azimuth_fft_size
   (&optional __azimuth_fft_size)
   (if __azimuth_fft_size (setq _azimuth_fft_size __azimuth_fft_size)) _azimuth_fft_size)
  (:elevation_fft_size
   (&optional __elevation_fft_size)
   (if __elevation_fft_size (setq _elevation_fft_size __elevation_fft_size)) _elevation_fft_size)
  (:azimuth_fft_padding
   (&optional __azimuth_fft_padding)
   (if __azimuth_fft_padding (setq _azimuth_fft_padding __azimuth_fft_padding)) _azimuth_fft_padding)
  (:elevation_fft_padding
   (&optional __elevation_fft_padding)
   (if __elevation_fft_padding (setq _elevation_fft_padding __elevation_fft_padding)) _elevation_fft_padding)
  (:serialization-length
   ()
   (+
    ;; arbe_msgs/arbePcRes _PcResolution
    (send _PcResolution :serialization-length)
    ;; float32 _range_offset
    4
    ;; float32 _range_res_in_db
    4
    ;; uint32 _azimuth_fft_size
    4
    ;; uint32 _elevation_fft_size
    4
    ;; uint32 _azimuth_fft_padding
    4
    ;; uint32 _elevation_fft_padding
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; arbe_msgs/arbePcRes _PcResolution
       (send _PcResolution :serialize s)
     ;; float32 _range_offset
       (sys::poke _range_offset (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _range_res_in_db
       (sys::poke _range_res_in_db (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; uint32 _azimuth_fft_size
       (write-long _azimuth_fft_size s)
     ;; uint32 _elevation_fft_size
       (write-long _elevation_fft_size s)
     ;; uint32 _azimuth_fft_padding
       (write-long _azimuth_fft_padding s)
     ;; uint32 _elevation_fft_padding
       (write-long _elevation_fft_padding s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; arbe_msgs/arbePcRes _PcResolution
     (send _PcResolution :deserialize buf ptr-) (incf ptr- (send _PcResolution :serialization-length))
   ;; float32 _range_offset
     (setq _range_offset (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _range_res_in_db
     (setq _range_res_in_db (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; uint32 _azimuth_fft_size
     (setq _azimuth_fft_size (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; uint32 _elevation_fft_size
     (setq _elevation_fft_size (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; uint32 _azimuth_fft_padding
     (setq _azimuth_fft_padding (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; uint32 _elevation_fft_padding
     (setq _elevation_fft_padding (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbePcMetadata :md5sum-) "07014ddb83398e57a7856835fad2cfca")
(setf (get arbe_msgs::arbePcMetadata :datatype-) "arbe_msgs/arbePcMetadata")
(setf (get arbe_msgs::arbePcMetadata :definition-)
      "arbePcRes PcResolution
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



")



(provide :arbe_msgs/arbePcMetadata "07014ddb83398e57a7856835fad2cfca")


