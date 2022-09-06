;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeTAlgoDetectorThrInfo)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeTAlgoDetectorThrInfo (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBETALGODETECTORTHRINFO")
  (make-package "ARBE_MSGS::ARBETALGODETECTORTHRINFO"))

(in-package "ROS")
;;//! \htmlinclude arbeTAlgoDetectorThrInfo.msg.html


(defclass arbe_msgs::arbeTAlgoDetectorThrInfo
  :super ros::object
  :slots (_unFrameNumber _eAlgoOutput _eThresholdType _eFrameType _Data ))

(defmethod arbe_msgs::arbeTAlgoDetectorThrInfo
  (:init
   (&key
    ((:unFrameNumber __unFrameNumber) 0)
    ((:eAlgoOutput __eAlgoOutput) 0)
    ((:eThresholdType __eThresholdType) 0)
    ((:eFrameType __eFrameType) 0)
    ((:Data __Data) (make-array 0 :initial-element 0 :element-type :integer))
    )
   (send-super :init)
   (setq _unFrameNumber (round __unFrameNumber))
   (setq _eAlgoOutput (round __eAlgoOutput))
   (setq _eThresholdType (round __eThresholdType))
   (setq _eFrameType (round __eFrameType))
   (setq _Data __Data)
   self)
  (:unFrameNumber
   (&optional __unFrameNumber)
   (if __unFrameNumber (setq _unFrameNumber __unFrameNumber)) _unFrameNumber)
  (:eAlgoOutput
   (&optional __eAlgoOutput)
   (if __eAlgoOutput (setq _eAlgoOutput __eAlgoOutput)) _eAlgoOutput)
  (:eThresholdType
   (&optional __eThresholdType)
   (if __eThresholdType (setq _eThresholdType __eThresholdType)) _eThresholdType)
  (:eFrameType
   (&optional __eFrameType)
   (if __eFrameType (setq _eFrameType __eFrameType)) _eFrameType)
  (:Data
   (&optional __Data)
   (if __Data (setq _Data __Data)) _Data)
  (:serialization-length
   ()
   (+
    ;; uint32 _unFrameNumber
    4
    ;; uint8 _eAlgoOutput
    1
    ;; uint8 _eThresholdType
    1
    ;; uint8 _eFrameType
    1
    ;; uint32[] _Data
    (* 4    (length _Data)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint32 _unFrameNumber
       (write-long _unFrameNumber s)
     ;; uint8 _eAlgoOutput
       (write-byte _eAlgoOutput s)
     ;; uint8 _eThresholdType
       (write-byte _eThresholdType s)
     ;; uint8 _eFrameType
       (write-byte _eFrameType s)
     ;; uint32[] _Data
     (write-long (length _Data) s)
     (dotimes (i (length _Data))
       (write-long (elt _Data i) s)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint32 _unFrameNumber
     (setq _unFrameNumber (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; uint8 _eAlgoOutput
     (setq _eAlgoOutput (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; uint8 _eThresholdType
     (setq _eThresholdType (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; uint8 _eFrameType
     (setq _eFrameType (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; uint32[] _Data
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _Data (instantiate integer-vector n))
     (dotimes (i n)
     (setf (elt _Data i) (sys::peek buf ptr- :integer)) (incf ptr- 4)
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeTAlgoDetectorThrInfo :md5sum-) "5f9f71c2214552c0e7f4ddc765c4621d")
(setf (get arbe_msgs::arbeTAlgoDetectorThrInfo :datatype-) "arbe_msgs/arbeTAlgoDetectorThrInfo")
(setf (get arbe_msgs::arbeTAlgoDetectorThrInfo :definition-)
      "uint32 unFrameNumber
uint8 eAlgoOutput
uint8 eThresholdType
uint8 eFrameType
uint32[] Data

")



(provide :arbe_msgs/arbeTAlgoDetectorThrInfo "5f9f71c2214552c0e7f4ddc765c4621d")


