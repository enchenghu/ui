;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeDtExceedTh)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeDtExceedTh (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEDTEXCEEDTH")
  (make-package "ARBE_MSGS::ARBEDTEXCEEDTH"))

(in-package "ROS")
;;//! \htmlinclude arbeDtExceedTh.msg.html


(defclass arbe_msgs::arbeDtExceedTh
  :super ros::object
  :slots (_frame_counter _dt ))

(defmethod arbe_msgs::arbeDtExceedTh
  (:init
   (&key
    ((:frame_counter __frame_counter) 0)
    ((:dt __dt) 0)
    )
   (send-super :init)
   (setq _frame_counter (round __frame_counter))
   (setq _dt (round __dt))
   self)
  (:frame_counter
   (&optional __frame_counter)
   (if __frame_counter (setq _frame_counter __frame_counter)) _frame_counter)
  (:dt
   (&optional __dt)
   (if __dt (setq _dt __dt)) _dt)
  (:serialization-length
   ()
   (+
    ;; uint16 _frame_counter
    2
    ;; uint64 _dt
    8
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint16 _frame_counter
       (write-word _frame_counter s)
     ;; uint64 _dt
#+(or :alpha :irix6 :x86_64)
       (progn (sys::poke _dt (send s :buffer) (send s :count) :long) (incf (stream-count s) 8))
#-(or :alpha :irix6 :x86_64)
       (cond ((and (class _dt) (= (length (_dt . bv)) 2)) ;; bignum
              (write-long (ash (elt (_dt . bv) 0) 0) s)
              (write-long (ash (elt (_dt . bv) 1) -1) s))
             ((and (class _dt) (= (length (_dt . bv)) 1)) ;; big1
              (write-long (elt (_dt . bv) 0) s)
              (write-long (if (>= _dt 0) 0 #xffffffff) s))
             (t                                         ;; integer
              (write-long _dt s)(write-long (if (>= _dt 0) 0 #xffffffff) s)))
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint16 _frame_counter
     (setq _frame_counter (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint64 _dt
     
#+(or :alpha :irix6 :x86_64)
      (setf _dt (prog1 (sys::peek buf ptr- :long) (incf ptr- 8)))
#-(or :alpha :irix6 :x86_64)
      (setf _dt (let ((b0 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4)))
                  (b1 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4))))
              (cond ((= b1 -1) b0)
                     ((and (= b1  0)
                           (<= lisp::most-negative-fixnum b0 lisp::most-positive-fixnum))
                      b0)
                    ((= b1  0) (make-instance bignum :size 1 :bv (integer-vector b0)))
                    (t (make-instance bignum :size 2 :bv (integer-vector b0 (ash b1 1)))))))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeDtExceedTh :md5sum-) "b3c2831d3d6ab52b0524ebb559a84899")
(setf (get arbe_msgs::arbeDtExceedTh :datatype-) "arbe_msgs/arbeDtExceedTh")
(setf (get arbe_msgs::arbeDtExceedTh :definition-)
      "uint16 frame_counter
uint64 dt

")



(provide :arbe_msgs/arbeDtExceedTh "b3c2831d3d6ab52b0524ebb559a84899")


