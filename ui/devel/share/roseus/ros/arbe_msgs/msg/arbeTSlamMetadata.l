;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeTSlamMetadata)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeTSlamMetadata (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBETSLAMMETADATA")
  (make-package "ARBE_MSGS::ARBETSLAMMETADATA"))

(in-package "ROS")
;;//! \htmlinclude arbeTSlamMetadata.msg.html


(defclass arbe_msgs::arbeTSlamMetadata
  :super ros::object
  :slots (_CorresPcTime _FrameNumber _HostHeading _HostHeadingUnc _HostVelocity _HostVelocityUnc _HostOmega _HostOmegaUnc _NumberOfObjects _local_catr_x _local_catr_y _location_valid _is_reset_geo_location ))

(defmethod arbe_msgs::arbeTSlamMetadata
  (:init
   (&key
    ((:CorresPcTime __CorresPcTime) 0)
    ((:FrameNumber __FrameNumber) 0)
    ((:HostHeading __HostHeading) 0.0)
    ((:HostHeadingUnc __HostHeadingUnc) 0.0)
    ((:HostVelocity __HostVelocity) 0.0)
    ((:HostVelocityUnc __HostVelocityUnc) 0.0)
    ((:HostOmega __HostOmega) 0.0)
    ((:HostOmegaUnc __HostOmegaUnc) 0.0)
    ((:NumberOfObjects __NumberOfObjects) 0)
    ((:local_catr_x __local_catr_x) 0.0)
    ((:local_catr_y __local_catr_y) 0.0)
    ((:location_valid __location_valid) nil)
    ((:is_reset_geo_location __is_reset_geo_location) nil)
    )
   (send-super :init)
   (setq _CorresPcTime (round __CorresPcTime))
   (setq _FrameNumber (round __FrameNumber))
   (setq _HostHeading (float __HostHeading))
   (setq _HostHeadingUnc (float __HostHeadingUnc))
   (setq _HostVelocity (float __HostVelocity))
   (setq _HostVelocityUnc (float __HostVelocityUnc))
   (setq _HostOmega (float __HostOmega))
   (setq _HostOmegaUnc (float __HostOmegaUnc))
   (setq _NumberOfObjects (round __NumberOfObjects))
   (setq _local_catr_x (float __local_catr_x))
   (setq _local_catr_y (float __local_catr_y))
   (setq _location_valid __location_valid)
   (setq _is_reset_geo_location __is_reset_geo_location)
   self)
  (:CorresPcTime
   (&optional __CorresPcTime)
   (if __CorresPcTime (setq _CorresPcTime __CorresPcTime)) _CorresPcTime)
  (:FrameNumber
   (&optional __FrameNumber)
   (if __FrameNumber (setq _FrameNumber __FrameNumber)) _FrameNumber)
  (:HostHeading
   (&optional __HostHeading)
   (if __HostHeading (setq _HostHeading __HostHeading)) _HostHeading)
  (:HostHeadingUnc
   (&optional __HostHeadingUnc)
   (if __HostHeadingUnc (setq _HostHeadingUnc __HostHeadingUnc)) _HostHeadingUnc)
  (:HostVelocity
   (&optional __HostVelocity)
   (if __HostVelocity (setq _HostVelocity __HostVelocity)) _HostVelocity)
  (:HostVelocityUnc
   (&optional __HostVelocityUnc)
   (if __HostVelocityUnc (setq _HostVelocityUnc __HostVelocityUnc)) _HostVelocityUnc)
  (:HostOmega
   (&optional __HostOmega)
   (if __HostOmega (setq _HostOmega __HostOmega)) _HostOmega)
  (:HostOmegaUnc
   (&optional __HostOmegaUnc)
   (if __HostOmegaUnc (setq _HostOmegaUnc __HostOmegaUnc)) _HostOmegaUnc)
  (:NumberOfObjects
   (&optional __NumberOfObjects)
   (if __NumberOfObjects (setq _NumberOfObjects __NumberOfObjects)) _NumberOfObjects)
  (:local_catr_x
   (&optional __local_catr_x)
   (if __local_catr_x (setq _local_catr_x __local_catr_x)) _local_catr_x)
  (:local_catr_y
   (&optional __local_catr_y)
   (if __local_catr_y (setq _local_catr_y __local_catr_y)) _local_catr_y)
  (:location_valid
   (&optional __location_valid)
   (if __location_valid (setq _location_valid __location_valid)) _location_valid)
  (:is_reset_geo_location
   (&optional __is_reset_geo_location)
   (if __is_reset_geo_location (setq _is_reset_geo_location __is_reset_geo_location)) _is_reset_geo_location)
  (:serialization-length
   ()
   (+
    ;; uint64 _CorresPcTime
    8
    ;; uint16 _FrameNumber
    2
    ;; float32 _HostHeading
    4
    ;; float32 _HostHeadingUnc
    4
    ;; float32 _HostVelocity
    4
    ;; float32 _HostVelocityUnc
    4
    ;; float32 _HostOmega
    4
    ;; float32 _HostOmegaUnc
    4
    ;; uint16 _NumberOfObjects
    2
    ;; float32 _local_catr_x
    4
    ;; float32 _local_catr_y
    4
    ;; bool _location_valid
    1
    ;; bool _is_reset_geo_location
    1
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint64 _CorresPcTime
#+(or :alpha :irix6 :x86_64)
       (progn (sys::poke _CorresPcTime (send s :buffer) (send s :count) :long) (incf (stream-count s) 8))
#-(or :alpha :irix6 :x86_64)
       (cond ((and (class _CorresPcTime) (= (length (_CorresPcTime . bv)) 2)) ;; bignum
              (write-long (ash (elt (_CorresPcTime . bv) 0) 0) s)
              (write-long (ash (elt (_CorresPcTime . bv) 1) -1) s))
             ((and (class _CorresPcTime) (= (length (_CorresPcTime . bv)) 1)) ;; big1
              (write-long (elt (_CorresPcTime . bv) 0) s)
              (write-long (if (>= _CorresPcTime 0) 0 #xffffffff) s))
             (t                                         ;; integer
              (write-long _CorresPcTime s)(write-long (if (>= _CorresPcTime 0) 0 #xffffffff) s)))
     ;; uint16 _FrameNumber
       (write-word _FrameNumber s)
     ;; float32 _HostHeading
       (sys::poke _HostHeading (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _HostHeadingUnc
       (sys::poke _HostHeadingUnc (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _HostVelocity
       (sys::poke _HostVelocity (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _HostVelocityUnc
       (sys::poke _HostVelocityUnc (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _HostOmega
       (sys::poke _HostOmega (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _HostOmegaUnc
       (sys::poke _HostOmegaUnc (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; uint16 _NumberOfObjects
       (write-word _NumberOfObjects s)
     ;; float32 _local_catr_x
       (sys::poke _local_catr_x (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _local_catr_y
       (sys::poke _local_catr_y (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; bool _location_valid
       (if _location_valid (write-byte -1 s) (write-byte 0 s))
     ;; bool _is_reset_geo_location
       (if _is_reset_geo_location (write-byte -1 s) (write-byte 0 s))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint64 _CorresPcTime
     
#+(or :alpha :irix6 :x86_64)
      (setf _CorresPcTime (prog1 (sys::peek buf ptr- :long) (incf ptr- 8)))
#-(or :alpha :irix6 :x86_64)
      (setf _CorresPcTime (let ((b0 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4)))
                  (b1 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4))))
              (cond ((= b1 -1) b0)
                     ((and (= b1  0)
                           (<= lisp::most-negative-fixnum b0 lisp::most-positive-fixnum))
                      b0)
                    ((= b1  0) (make-instance bignum :size 1 :bv (integer-vector b0)))
                    (t (make-instance bignum :size 2 :bv (integer-vector b0 (ash b1 1)))))))
   ;; uint16 _FrameNumber
     (setq _FrameNumber (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; float32 _HostHeading
     (setq _HostHeading (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _HostHeadingUnc
     (setq _HostHeadingUnc (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _HostVelocity
     (setq _HostVelocity (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _HostVelocityUnc
     (setq _HostVelocityUnc (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _HostOmega
     (setq _HostOmega (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _HostOmegaUnc
     (setq _HostOmegaUnc (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; uint16 _NumberOfObjects
     (setq _NumberOfObjects (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; float32 _local_catr_x
     (setq _local_catr_x (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _local_catr_y
     (setq _local_catr_y (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; bool _location_valid
     (setq _location_valid (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _is_reset_geo_location
     (setq _is_reset_geo_location (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeTSlamMetadata :md5sum-) "9f419d3fbb87f9bdc08e8e38e6a62de8")
(setf (get arbe_msgs::arbeTSlamMetadata :datatype-) "arbe_msgs/arbeTSlamMetadata")
(setf (get arbe_msgs::arbeTSlamMetadata :definition-)
      "uint64  CorresPcTime
uint16  FrameNumber
float32 HostHeading
float32 HostHeadingUnc
float32 HostVelocity
float32 HostVelocityUnc
float32 HostOmega		
float32 HostOmegaUnc
uint16  NumberOfObjects
float32 local_catr_x
float32 local_catr_y
bool    location_valid
bool    is_reset_geo_location


")



(provide :arbe_msgs/arbeTSlamMetadata "9f419d3fbb87f9bdc08e8e38e6a62de8")


