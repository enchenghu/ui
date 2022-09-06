;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeTSlamBox)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeTSlamBox (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBETSLAMBOX")
  (make-package "ARBE_MSGS::ARBETSLAMBOX"))

(in-package "ROS")
;;//! \htmlinclude arbeTSlamBox.msg.html


(defclass arbe_msgs::arbeTSlamBox
  :super ros::object
  :slots (_scale_x _scale_y _scale_z _scale_x_unc _scale_y_unc _scale_z_unc _orientation_unc _orientation ))

(defmethod arbe_msgs::arbeTSlamBox
  (:init
   (&key
    ((:scale_x __scale_x) 0.0)
    ((:scale_y __scale_y) 0.0)
    ((:scale_z __scale_z) 0.0)
    ((:scale_x_unc __scale_x_unc) 0.0)
    ((:scale_y_unc __scale_y_unc) 0.0)
    ((:scale_z_unc __scale_z_unc) 0.0)
    ((:orientation_unc __orientation_unc) 0.0)
    ((:orientation __orientation) 0.0)
    )
   (send-super :init)
   (setq _scale_x (float __scale_x))
   (setq _scale_y (float __scale_y))
   (setq _scale_z (float __scale_z))
   (setq _scale_x_unc (float __scale_x_unc))
   (setq _scale_y_unc (float __scale_y_unc))
   (setq _scale_z_unc (float __scale_z_unc))
   (setq _orientation_unc (float __orientation_unc))
   (setq _orientation (float __orientation))
   self)
  (:scale_x
   (&optional __scale_x)
   (if __scale_x (setq _scale_x __scale_x)) _scale_x)
  (:scale_y
   (&optional __scale_y)
   (if __scale_y (setq _scale_y __scale_y)) _scale_y)
  (:scale_z
   (&optional __scale_z)
   (if __scale_z (setq _scale_z __scale_z)) _scale_z)
  (:scale_x_unc
   (&optional __scale_x_unc)
   (if __scale_x_unc (setq _scale_x_unc __scale_x_unc)) _scale_x_unc)
  (:scale_y_unc
   (&optional __scale_y_unc)
   (if __scale_y_unc (setq _scale_y_unc __scale_y_unc)) _scale_y_unc)
  (:scale_z_unc
   (&optional __scale_z_unc)
   (if __scale_z_unc (setq _scale_z_unc __scale_z_unc)) _scale_z_unc)
  (:orientation_unc
   (&optional __orientation_unc)
   (if __orientation_unc (setq _orientation_unc __orientation_unc)) _orientation_unc)
  (:orientation
   (&optional __orientation)
   (if __orientation (setq _orientation __orientation)) _orientation)
  (:serialization-length
   ()
   (+
    ;; float32 _scale_x
    4
    ;; float32 _scale_y
    4
    ;; float32 _scale_z
    4
    ;; float32 _scale_x_unc
    4
    ;; float32 _scale_y_unc
    4
    ;; float32 _scale_z_unc
    4
    ;; float32 _orientation_unc
    4
    ;; float32 _orientation
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _scale_x
       (sys::poke _scale_x (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _scale_y
       (sys::poke _scale_y (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _scale_z
       (sys::poke _scale_z (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _scale_x_unc
       (sys::poke _scale_x_unc (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _scale_y_unc
       (sys::poke _scale_y_unc (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _scale_z_unc
       (sys::poke _scale_z_unc (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _orientation_unc
       (sys::poke _orientation_unc (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _orientation
       (sys::poke _orientation (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _scale_x
     (setq _scale_x (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _scale_y
     (setq _scale_y (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _scale_z
     (setq _scale_z (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _scale_x_unc
     (setq _scale_x_unc (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _scale_y_unc
     (setq _scale_y_unc (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _scale_z_unc
     (setq _scale_z_unc (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _orientation_unc
     (setq _orientation_unc (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _orientation
     (setq _orientation (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeTSlamBox :md5sum-) "e36bb424db9b07f679bc8512896c93f7")
(setf (get arbe_msgs::arbeTSlamBox :datatype-) "arbe_msgs/arbeTSlamBox")
(setf (get arbe_msgs::arbeTSlamBox :definition-)
      "float32 scale_x     
float32 scale_y     
float32 scale_z     
float32 scale_x_unc 
float32 scale_y_unc 
float32 scale_z_unc 
float32 orientation_unc  
float32 orientation  

")



(provide :arbe_msgs/arbeTSlamBox "e36bb424db9b07f679bc8512896c93f7")


