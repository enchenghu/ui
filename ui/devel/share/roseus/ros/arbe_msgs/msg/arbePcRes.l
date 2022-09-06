;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbePcRes)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbePcRes (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEPCRES")
  (make-package "ARBE_MSGS::ARBEPCRES"))

(in-package "ROS")
;;//! \htmlinclude arbePcRes.msg.html


(defclass arbe_msgs::arbePcRes
  :super ros::object
  :slots (_range_resolution _doppler_resolution _azimuth_coefficient _elevation_coefficient ))

(defmethod arbe_msgs::arbePcRes
  (:init
   (&key
    ((:range_resolution __range_resolution) 0.0)
    ((:doppler_resolution __doppler_resolution) 0.0)
    ((:azimuth_coefficient __azimuth_coefficient) 0.0)
    ((:elevation_coefficient __elevation_coefficient) 0.0)
    )
   (send-super :init)
   (setq _range_resolution (float __range_resolution))
   (setq _doppler_resolution (float __doppler_resolution))
   (setq _azimuth_coefficient (float __azimuth_coefficient))
   (setq _elevation_coefficient (float __elevation_coefficient))
   self)
  (:range_resolution
   (&optional __range_resolution)
   (if __range_resolution (setq _range_resolution __range_resolution)) _range_resolution)
  (:doppler_resolution
   (&optional __doppler_resolution)
   (if __doppler_resolution (setq _doppler_resolution __doppler_resolution)) _doppler_resolution)
  (:azimuth_coefficient
   (&optional __azimuth_coefficient)
   (if __azimuth_coefficient (setq _azimuth_coefficient __azimuth_coefficient)) _azimuth_coefficient)
  (:elevation_coefficient
   (&optional __elevation_coefficient)
   (if __elevation_coefficient (setq _elevation_coefficient __elevation_coefficient)) _elevation_coefficient)
  (:serialization-length
   ()
   (+
    ;; float32 _range_resolution
    4
    ;; float32 _doppler_resolution
    4
    ;; float32 _azimuth_coefficient
    4
    ;; float32 _elevation_coefficient
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _range_resolution
       (sys::poke _range_resolution (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _doppler_resolution
       (sys::poke _doppler_resolution (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _azimuth_coefficient
       (sys::poke _azimuth_coefficient (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _elevation_coefficient
       (sys::poke _elevation_coefficient (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _range_resolution
     (setq _range_resolution (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _doppler_resolution
     (setq _doppler_resolution (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _azimuth_coefficient
     (setq _azimuth_coefficient (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _elevation_coefficient
     (setq _elevation_coefficient (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbePcRes :md5sum-) "84103936b409bd62d8ea8aada9826bd5")
(setf (get arbe_msgs::arbePcRes :datatype-) "arbe_msgs/arbePcRes")
(setf (get arbe_msgs::arbePcRes :definition-)
      "float32 range_resolution
float32 doppler_resolution
float32 azimuth_coefficient
float32 elevation_coefficient



")



(provide :arbe_msgs/arbePcRes "84103936b409bd62d8ea8aada9826bd5")


