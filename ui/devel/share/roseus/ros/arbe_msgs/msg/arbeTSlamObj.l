;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeTSlamObj)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeTSlamObj (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBETSLAMOBJ")
  (make-package "ARBE_MSGS::ARBETSLAMOBJ"))

(in-package "ROS")
;;//! \htmlinclude arbeTSlamObj.msg.html


(defclass arbe_msgs::arbeTSlamObj
  :super ros::object
  :slots (_ID _obj_class _class_conf _position _velocity _bounding_box _age _last_frame_update ))

(defmethod arbe_msgs::arbeTSlamObj
  (:init
   (&key
    ((:ID __ID) 0)
    ((:obj_class __obj_class) 0)
    ((:class_conf __class_conf) 0.0)
    ((:position __position) (instance arbe_msgs::arbeTSlamPos :init))
    ((:velocity __velocity) (instance arbe_msgs::arbeTSlamVelocity :init))
    ((:bounding_box __bounding_box) (instance arbe_msgs::arbeTSlamBox :init))
    ((:age __age) 0)
    ((:last_frame_update __last_frame_update) 0)
    )
   (send-super :init)
   (setq _ID (round __ID))
   (setq _obj_class (round __obj_class))
   (setq _class_conf (float __class_conf))
   (setq _position __position)
   (setq _velocity __velocity)
   (setq _bounding_box __bounding_box)
   (setq _age (round __age))
   (setq _last_frame_update (round __last_frame_update))
   self)
  (:ID
   (&optional __ID)
   (if __ID (setq _ID __ID)) _ID)
  (:obj_class
   (&optional __obj_class)
   (if __obj_class (setq _obj_class __obj_class)) _obj_class)
  (:class_conf
   (&optional __class_conf)
   (if __class_conf (setq _class_conf __class_conf)) _class_conf)
  (:position
   (&rest __position)
   (if (keywordp (car __position))
       (send* _position __position)
     (progn
       (if __position (setq _position (car __position)))
       _position)))
  (:velocity
   (&rest __velocity)
   (if (keywordp (car __velocity))
       (send* _velocity __velocity)
     (progn
       (if __velocity (setq _velocity (car __velocity)))
       _velocity)))
  (:bounding_box
   (&rest __bounding_box)
   (if (keywordp (car __bounding_box))
       (send* _bounding_box __bounding_box)
     (progn
       (if __bounding_box (setq _bounding_box (car __bounding_box)))
       _bounding_box)))
  (:age
   (&optional __age)
   (if __age (setq _age __age)) _age)
  (:last_frame_update
   (&optional __last_frame_update)
   (if __last_frame_update (setq _last_frame_update __last_frame_update)) _last_frame_update)
  (:serialization-length
   ()
   (+
    ;; uint16 _ID
    2
    ;; uint16 _obj_class
    2
    ;; float32 _class_conf
    4
    ;; arbe_msgs/arbeTSlamPos _position
    (send _position :serialization-length)
    ;; arbe_msgs/arbeTSlamVelocity _velocity
    (send _velocity :serialization-length)
    ;; arbe_msgs/arbeTSlamBox _bounding_box
    (send _bounding_box :serialization-length)
    ;; uint32 _age
    4
    ;; uint16 _last_frame_update
    2
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint16 _ID
       (write-word _ID s)
     ;; uint16 _obj_class
       (write-word _obj_class s)
     ;; float32 _class_conf
       (sys::poke _class_conf (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; arbe_msgs/arbeTSlamPos _position
       (send _position :serialize s)
     ;; arbe_msgs/arbeTSlamVelocity _velocity
       (send _velocity :serialize s)
     ;; arbe_msgs/arbeTSlamBox _bounding_box
       (send _bounding_box :serialize s)
     ;; uint32 _age
       (write-long _age s)
     ;; uint16 _last_frame_update
       (write-word _last_frame_update s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint16 _ID
     (setq _ID (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _obj_class
     (setq _obj_class (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; float32 _class_conf
     (setq _class_conf (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; arbe_msgs/arbeTSlamPos _position
     (send _position :deserialize buf ptr-) (incf ptr- (send _position :serialization-length))
   ;; arbe_msgs/arbeTSlamVelocity _velocity
     (send _velocity :deserialize buf ptr-) (incf ptr- (send _velocity :serialization-length))
   ;; arbe_msgs/arbeTSlamBox _bounding_box
     (send _bounding_box :deserialize buf ptr-) (incf ptr- (send _bounding_box :serialization-length))
   ;; uint32 _age
     (setq _age (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; uint16 _last_frame_update
     (setq _last_frame_update (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeTSlamObj :md5sum-) "1eb101416f9437d15058033f4585fe65")
(setf (get arbe_msgs::arbeTSlamObj :datatype-) "arbe_msgs/arbeTSlamObj")
(setf (get arbe_msgs::arbeTSlamObj :definition-)
      "uint16              ID
uint16              obj_class
float32             class_conf
arbeTSlamPos        position
arbeTSlamVelocity   velocity 
arbeTSlamBox        bounding_box
uint32              age
uint16              last_frame_update



================================================================================
MSG: arbe_msgs/arbeTSlamPos
float32 x 
float32 y 
float32 z 
float32  dx 
float32  dy 
float32  dz

================================================================================
MSG: arbe_msgs/arbeTSlamVelocity
float32     x_dot 
float32     y_dot 
float32      dx_dot 
float32      dy_dot 

================================================================================
MSG: arbe_msgs/arbeTSlamBox
float32 scale_x     
float32 scale_y     
float32 scale_z     
float32 scale_x_unc 
float32 scale_y_unc 
float32 scale_z_unc 
float32 orientation_unc  
float32 orientation  

")



(provide :arbe_msgs/arbeTSlamObj "1eb101416f9437d15058033f4585fe65")


