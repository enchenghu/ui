;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeBookmarkMsg)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeBookmarkMsg (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEBOOKMARKMSG")
  (make-package "ARBE_MSGS::ARBEBOOKMARKMSG"))

(in-package "ROS")
;;//! \htmlinclude arbeBookmarkMsg.msg.html


(defclass arbe_msgs::arbeBookmarkMsg
  :super ros::object
  :slots (_frame_num _egoVel ))

(defmethod arbe_msgs::arbeBookmarkMsg
  (:init
   (&key
    ((:frame_num __frame_num) 0)
    ((:egoVel __egoVel) 0.0)
    )
   (send-super :init)
   (setq _frame_num (round __frame_num))
   (setq _egoVel (float __egoVel))
   self)
  (:frame_num
   (&optional __frame_num)
   (if __frame_num (setq _frame_num __frame_num)) _frame_num)
  (:egoVel
   (&optional __egoVel)
   (if __egoVel (setq _egoVel __egoVel)) _egoVel)
  (:serialization-length
   ()
   (+
    ;; uint16 _frame_num
    2
    ;; float32 _egoVel
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint16 _frame_num
       (write-word _frame_num s)
     ;; float32 _egoVel
       (sys::poke _egoVel (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint16 _frame_num
     (setq _frame_num (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; float32 _egoVel
     (setq _egoVel (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeBookmarkMsg :md5sum-) "968e0faad3956fde6fdb61e0a1f108a6")
(setf (get arbe_msgs::arbeBookmarkMsg :datatype-) "arbe_msgs/arbeBookmarkMsg")
(setf (get arbe_msgs::arbeBookmarkMsg :definition-)
      "uint16 frame_num
float32 egoVel

")



(provide :arbe_msgs/arbeBookmarkMsg "968e0faad3956fde6fdb61e0a1f108a6")


