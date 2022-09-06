;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeTSlamVelocity)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeTSlamVelocity (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBETSLAMVELOCITY")
  (make-package "ARBE_MSGS::ARBETSLAMVELOCITY"))

(in-package "ROS")
;;//! \htmlinclude arbeTSlamVelocity.msg.html


(defclass arbe_msgs::arbeTSlamVelocity
  :super ros::object
  :slots (_x_dot _y_dot _dx_dot _dy_dot ))

(defmethod arbe_msgs::arbeTSlamVelocity
  (:init
   (&key
    ((:x_dot __x_dot) 0.0)
    ((:y_dot __y_dot) 0.0)
    ((:dx_dot __dx_dot) 0.0)
    ((:dy_dot __dy_dot) 0.0)
    )
   (send-super :init)
   (setq _x_dot (float __x_dot))
   (setq _y_dot (float __y_dot))
   (setq _dx_dot (float __dx_dot))
   (setq _dy_dot (float __dy_dot))
   self)
  (:x_dot
   (&optional __x_dot)
   (if __x_dot (setq _x_dot __x_dot)) _x_dot)
  (:y_dot
   (&optional __y_dot)
   (if __y_dot (setq _y_dot __y_dot)) _y_dot)
  (:dx_dot
   (&optional __dx_dot)
   (if __dx_dot (setq _dx_dot __dx_dot)) _dx_dot)
  (:dy_dot
   (&optional __dy_dot)
   (if __dy_dot (setq _dy_dot __dy_dot)) _dy_dot)
  (:serialization-length
   ()
   (+
    ;; float32 _x_dot
    4
    ;; float32 _y_dot
    4
    ;; float32 _dx_dot
    4
    ;; float32 _dy_dot
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _x_dot
       (sys::poke _x_dot (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _y_dot
       (sys::poke _y_dot (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _dx_dot
       (sys::poke _dx_dot (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _dy_dot
       (sys::poke _dy_dot (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _x_dot
     (setq _x_dot (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _y_dot
     (setq _y_dot (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _dx_dot
     (setq _dx_dot (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _dy_dot
     (setq _dy_dot (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeTSlamVelocity :md5sum-) "01747869987f0c9150027780a275e95f")
(setf (get arbe_msgs::arbeTSlamVelocity :datatype-) "arbe_msgs/arbeTSlamVelocity")
(setf (get arbe_msgs::arbeTSlamVelocity :definition-)
      "float32     x_dot 
float32     y_dot 
float32      dx_dot 
float32      dy_dot 

")



(provide :arbe_msgs/arbeTSlamVelocity "01747869987f0c9150027780a275e95f")


