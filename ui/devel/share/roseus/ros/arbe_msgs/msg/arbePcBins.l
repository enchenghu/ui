;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbePcBins)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbePcBins (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEPCBINS")
  (make-package "ARBE_MSGS::ARBEPCBINS"))

(in-package "ROS")
;;//! \htmlinclude arbePcBins.msg.html


(defclass arbe_msgs::arbePcBins
  :super ros::object
  :slots (_range_bin _doppler_signed_bin _azimuth_signed_bin _elevation_signed_bin _phase _power ))

(defmethod arbe_msgs::arbePcBins
  (:init
   (&key
    ((:range_bin __range_bin) (make-array 0 :initial-element 0 :element-type :integer))
    ((:doppler_signed_bin __doppler_signed_bin) (make-array 0 :initial-element 0 :element-type :integer))
    ((:azimuth_signed_bin __azimuth_signed_bin) (make-array 0 :initial-element 0 :element-type :integer))
    ((:elevation_signed_bin __elevation_signed_bin) (make-array 0 :initial-element 0 :element-type :integer))
    ((:phase __phase) (make-array 0 :initial-element 0 :element-type :integer))
    ((:power __power) (make-array 0 :initial-element 0 :element-type :integer))
    )
   (send-super :init)
   (setq _range_bin __range_bin)
   (setq _doppler_signed_bin __doppler_signed_bin)
   (setq _azimuth_signed_bin __azimuth_signed_bin)
   (setq _elevation_signed_bin __elevation_signed_bin)
   (setq _phase __phase)
   (setq _power __power)
   self)
  (:range_bin
   (&optional __range_bin)
   (if __range_bin (setq _range_bin __range_bin)) _range_bin)
  (:doppler_signed_bin
   (&optional __doppler_signed_bin)
   (if __doppler_signed_bin (setq _doppler_signed_bin __doppler_signed_bin)) _doppler_signed_bin)
  (:azimuth_signed_bin
   (&optional __azimuth_signed_bin)
   (if __azimuth_signed_bin (setq _azimuth_signed_bin __azimuth_signed_bin)) _azimuth_signed_bin)
  (:elevation_signed_bin
   (&optional __elevation_signed_bin)
   (if __elevation_signed_bin (setq _elevation_signed_bin __elevation_signed_bin)) _elevation_signed_bin)
  (:phase
   (&optional __phase)
   (if __phase (setq _phase __phase)) _phase)
  (:power
   (&optional __power)
   (if __power (setq _power __power)) _power)
  (:serialization-length
   ()
   (+
    ;; uint16[] _range_bin
    (* 2    (length _range_bin)) 4
    ;; int16[] _doppler_signed_bin
    (* 2    (length _doppler_signed_bin)) 4
    ;; int8[] _azimuth_signed_bin
    (* 1    (length _azimuth_signed_bin)) 4
    ;; int8[] _elevation_signed_bin
    (* 1    (length _elevation_signed_bin)) 4
    ;; int16[] _phase
    (* 2    (length _phase)) 4
    ;; int16[] _power
    (* 2    (length _power)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint16[] _range_bin
     (write-long (length _range_bin) s)
     (dotimes (i (length _range_bin))
       (write-word (elt _range_bin i) s)
       )
     ;; int16[] _doppler_signed_bin
     (write-long (length _doppler_signed_bin) s)
     (dotimes (i (length _doppler_signed_bin))
       (write-word (elt _doppler_signed_bin i) s)
       )
     ;; int8[] _azimuth_signed_bin
     (write-long (length _azimuth_signed_bin) s)
     (dotimes (i (length _azimuth_signed_bin))
       (write-byte (elt _azimuth_signed_bin i) s)
       )
     ;; int8[] _elevation_signed_bin
     (write-long (length _elevation_signed_bin) s)
     (dotimes (i (length _elevation_signed_bin))
       (write-byte (elt _elevation_signed_bin i) s)
       )
     ;; int16[] _phase
     (write-long (length _phase) s)
     (dotimes (i (length _phase))
       (write-word (elt _phase i) s)
       )
     ;; int16[] _power
     (write-long (length _power) s)
     (dotimes (i (length _power))
       (write-word (elt _power i) s)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint16[] _range_bin
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _range_bin (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _range_bin i) (sys::peek buf ptr- :short)) (incf ptr- 2)
     ))
   ;; int16[] _doppler_signed_bin
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _doppler_signed_bin (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _doppler_signed_bin i) (sys::peek buf ptr- :short)) (incf ptr- 2)
     ))
   ;; int8[] _azimuth_signed_bin
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _azimuth_signed_bin (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _azimuth_signed_bin i) (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> (elt _azimuth_signed_bin i) 127) (setf (elt _azimuth_signed_bin i) (- (elt _azimuth_signed_bin i) 256)))
     ))
   ;; int8[] _elevation_signed_bin
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _elevation_signed_bin (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _elevation_signed_bin i) (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> (elt _elevation_signed_bin i) 127) (setf (elt _elevation_signed_bin i) (- (elt _elevation_signed_bin i) 256)))
     ))
   ;; int16[] _phase
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _phase (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _phase i) (sys::peek buf ptr- :short)) (incf ptr- 2)
     ))
   ;; int16[] _power
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _power (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _power i) (sys::peek buf ptr- :short)) (incf ptr- 2)
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbePcBins :md5sum-) "1053c45295755db3ee1eadc1a2005cf0")
(setf (get arbe_msgs::arbePcBins :datatype-) "arbe_msgs/arbePcBins")
(setf (get arbe_msgs::arbePcBins :definition-)
      "uint16[] range_bin
int16[] doppler_signed_bin
int8[] azimuth_signed_bin
int8[] elevation_signed_bin
int16[] phase
int16[] power



")



(provide :arbe_msgs/arbePcBins "1053c45295755db3ee1eadc1a2005cf0")


