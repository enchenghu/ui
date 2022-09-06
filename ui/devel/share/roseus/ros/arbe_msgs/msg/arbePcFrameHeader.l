;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbePcFrameHeader)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbePcFrameHeader (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEPCFRAMEHEADER")
  (make-package "ARBE_MSGS::ARBEPCFRAMEHEADER"))

(in-package "ROS")
;;//! \htmlinclude arbePcFrameHeader.msg.html


(defclass arbe_msgs::arbePcFrameHeader
  :super ros::object
  :slots (_time _frame_counter _is_4d _frame_type _udp_lost_packages _number_of_points _crd_count _frame_types ))

(defmethod arbe_msgs::arbePcFrameHeader
  (:init
   (&key
    ((:time __time) 0)
    ((:frame_counter __frame_counter) 0)
    ((:is_4d __is_4d) 0)
    ((:frame_type __frame_type) 0)
    ((:udp_lost_packages __udp_lost_packages) 0)
    ((:number_of_points __number_of_points) 0)
    ((:crd_count __crd_count) 0)
    ((:frame_types __frame_types) (instance arbe_msgs::arbeFrameTypes :init))
    )
   (send-super :init)
   (setq _time (round __time))
   (setq _frame_counter (round __frame_counter))
   (setq _is_4d (round __is_4d))
   (setq _frame_type (round __frame_type))
   (setq _udp_lost_packages (round __udp_lost_packages))
   (setq _number_of_points (round __number_of_points))
   (setq _crd_count (round __crd_count))
   (setq _frame_types __frame_types)
   self)
  (:time
   (&optional __time)
   (if __time (setq _time __time)) _time)
  (:frame_counter
   (&optional __frame_counter)
   (if __frame_counter (setq _frame_counter __frame_counter)) _frame_counter)
  (:is_4d
   (&optional __is_4d)
   (if __is_4d (setq _is_4d __is_4d)) _is_4d)
  (:frame_type
   (&optional __frame_type)
   (if __frame_type (setq _frame_type __frame_type)) _frame_type)
  (:udp_lost_packages
   (&optional __udp_lost_packages)
   (if __udp_lost_packages (setq _udp_lost_packages __udp_lost_packages)) _udp_lost_packages)
  (:number_of_points
   (&optional __number_of_points)
   (if __number_of_points (setq _number_of_points __number_of_points)) _number_of_points)
  (:crd_count
   (&optional __crd_count)
   (if __crd_count (setq _crd_count __crd_count)) _crd_count)
  (:frame_types
   (&rest __frame_types)
   (if (keywordp (car __frame_types))
       (send* _frame_types __frame_types)
     (progn
       (if __frame_types (setq _frame_types (car __frame_types)))
       _frame_types)))
  (:serialization-length
   ()
   (+
    ;; uint64 _time
    8
    ;; uint16 _frame_counter
    2
    ;; uint8 _is_4d
    1
    ;; uint8 _frame_type
    1
    ;; uint16 _udp_lost_packages
    2
    ;; uint32 _number_of_points
    4
    ;; uint16 _crd_count
    2
    ;; arbe_msgs/arbeFrameTypes _frame_types
    (send _frame_types :serialization-length)
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint64 _time
#+(or :alpha :irix6 :x86_64)
       (progn (sys::poke _time (send s :buffer) (send s :count) :long) (incf (stream-count s) 8))
#-(or :alpha :irix6 :x86_64)
       (cond ((and (class _time) (= (length (_time . bv)) 2)) ;; bignum
              (write-long (ash (elt (_time . bv) 0) 0) s)
              (write-long (ash (elt (_time . bv) 1) -1) s))
             ((and (class _time) (= (length (_time . bv)) 1)) ;; big1
              (write-long (elt (_time . bv) 0) s)
              (write-long (if (>= _time 0) 0 #xffffffff) s))
             (t                                         ;; integer
              (write-long _time s)(write-long (if (>= _time 0) 0 #xffffffff) s)))
     ;; uint16 _frame_counter
       (write-word _frame_counter s)
     ;; uint8 _is_4d
       (write-byte _is_4d s)
     ;; uint8 _frame_type
       (write-byte _frame_type s)
     ;; uint16 _udp_lost_packages
       (write-word _udp_lost_packages s)
     ;; uint32 _number_of_points
       (write-long _number_of_points s)
     ;; uint16 _crd_count
       (write-word _crd_count s)
     ;; arbe_msgs/arbeFrameTypes _frame_types
       (send _frame_types :serialize s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint64 _time
     
#+(or :alpha :irix6 :x86_64)
      (setf _time (prog1 (sys::peek buf ptr- :long) (incf ptr- 8)))
#-(or :alpha :irix6 :x86_64)
      (setf _time (let ((b0 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4)))
                  (b1 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4))))
              (cond ((= b1 -1) b0)
                     ((and (= b1  0)
                           (<= lisp::most-negative-fixnum b0 lisp::most-positive-fixnum))
                      b0)
                    ((= b1  0) (make-instance bignum :size 1 :bv (integer-vector b0)))
                    (t (make-instance bignum :size 2 :bv (integer-vector b0 (ash b1 1)))))))
   ;; uint16 _frame_counter
     (setq _frame_counter (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint8 _is_4d
     (setq _is_4d (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; uint8 _frame_type
     (setq _frame_type (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; uint16 _udp_lost_packages
     (setq _udp_lost_packages (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint32 _number_of_points
     (setq _number_of_points (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; uint16 _crd_count
     (setq _crd_count (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; arbe_msgs/arbeFrameTypes _frame_types
     (send _frame_types :deserialize buf ptr-) (incf ptr- (send _frame_types :serialization-length))
   ;;
   self)
  )

(setf (get arbe_msgs::arbePcFrameHeader :md5sum-) "ea7e8a42429d20d4636665fbdb9eab60")
(setf (get arbe_msgs::arbePcFrameHeader :datatype-) "arbe_msgs/arbePcFrameHeader")
(setf (get arbe_msgs::arbePcFrameHeader :definition-)
      "uint64 time
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

")



(provide :arbe_msgs/arbePcFrameHeader "ea7e8a42429d20d4636665fbdb9eab60")


