;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeTSlamPos)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeTSlamPos (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBETSLAMPOS")
  (make-package "ARBE_MSGS::ARBETSLAMPOS"))

(in-package "ROS")
;;//! \htmlinclude arbeTSlamPos.msg.html


(defclass arbe_msgs::arbeTSlamPos
  :super ros::object
  :slots (_x _y _z _dx _dy _dz ))

(defmethod arbe_msgs::arbeTSlamPos
  (:init
   (&key
    ((:x __x) 0.0)
    ((:y __y) 0.0)
    ((:z __z) 0.0)
    ((:dx __dx) 0.0)
    ((:dy __dy) 0.0)
    ((:dz __dz) 0.0)
    )
   (send-super :init)
   (setq _x (float __x))
   (setq _y (float __y))
   (setq _z (float __z))
   (setq _dx (float __dx))
   (setq _dy (float __dy))
   (setq _dz (float __dz))
   self)
  (:x
   (&optional __x)
   (if __x (setq _x __x)) _x)
  (:y
   (&optional __y)
   (if __y (setq _y __y)) _y)
  (:z
   (&optional __z)
   (if __z (setq _z __z)) _z)
  (:dx
   (&optional __dx)
   (if __dx (setq _dx __dx)) _dx)
  (:dy
   (&optional __dy)
   (if __dy (setq _dy __dy)) _dy)
  (:dz
   (&optional __dz)
   (if __dz (setq _dz __dz)) _dz)
  (:serialization-length
   ()
   (+
    ;; float32 _x
    4
    ;; float32 _y
    4
    ;; float32 _z
    4
    ;; float32 _dx
    4
    ;; float32 _dy
    4
    ;; float32 _dz
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _x
       (sys::poke _x (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _y
       (sys::poke _y (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _z
       (sys::poke _z (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _dx
       (sys::poke _dx (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _dy
       (sys::poke _dy (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _dz
       (sys::poke _dz (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _x
     (setq _x (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _y
     (setq _y (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _z
     (setq _z (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _dx
     (setq _dx (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _dy
     (setq _dy (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _dz
     (setq _dz (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeTSlamPos :md5sum-) "7ebdf4fc16c563585927be0d949cdc5a")
(setf (get arbe_msgs::arbeTSlamPos :datatype-) "arbe_msgs/arbeTSlamPos")
(setf (get arbe_msgs::arbeTSlamPos :definition-)
      "float32 x 
float32 y 
float32 z 
float32  dx 
float32  dy 
float32  dz

")



(provide :arbe_msgs/arbeTSlamPos "7ebdf4fc16c563585927be0d949cdc5a")


