;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeSettingsPerRadar)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeSettingsPerRadar (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBESETTINGSPERRADAR")
  (make-package "ARBE_MSGS::ARBESETTINGSPERRADAR"))

(in-package "ROS")
;;//! \htmlinclude arbeSettingsPerRadar.msg.html


(defclass arbe_msgs::arbeSettingsPerRadar
  :super ros::object
  :slots (_n_radars _radar_for_text _ant_height _ant_pitch _cam_euler_a _cam_euler_b _cam_euler_g ))

(defmethod arbe_msgs::arbeSettingsPerRadar
  (:init
   (&key
    ((:n_radars __n_radars) 0)
    ((:radar_for_text __radar_for_text) 0)
    ((:ant_height __ant_height) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:ant_pitch __ant_pitch) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:cam_euler_a __cam_euler_a) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:cam_euler_b __cam_euler_b) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:cam_euler_g __cam_euler_g) (make-array 0 :initial-element 0.0 :element-type :float))
    )
   (send-super :init)
   (setq _n_radars (round __n_radars))
   (setq _radar_for_text (round __radar_for_text))
   (setq _ant_height __ant_height)
   (setq _ant_pitch __ant_pitch)
   (setq _cam_euler_a __cam_euler_a)
   (setq _cam_euler_b __cam_euler_b)
   (setq _cam_euler_g __cam_euler_g)
   self)
  (:n_radars
   (&optional __n_radars)
   (if __n_radars (setq _n_radars __n_radars)) _n_radars)
  (:radar_for_text
   (&optional __radar_for_text)
   (if __radar_for_text (setq _radar_for_text __radar_for_text)) _radar_for_text)
  (:ant_height
   (&optional __ant_height)
   (if __ant_height (setq _ant_height __ant_height)) _ant_height)
  (:ant_pitch
   (&optional __ant_pitch)
   (if __ant_pitch (setq _ant_pitch __ant_pitch)) _ant_pitch)
  (:cam_euler_a
   (&optional __cam_euler_a)
   (if __cam_euler_a (setq _cam_euler_a __cam_euler_a)) _cam_euler_a)
  (:cam_euler_b
   (&optional __cam_euler_b)
   (if __cam_euler_b (setq _cam_euler_b __cam_euler_b)) _cam_euler_b)
  (:cam_euler_g
   (&optional __cam_euler_g)
   (if __cam_euler_g (setq _cam_euler_g __cam_euler_g)) _cam_euler_g)
  (:serialization-length
   ()
   (+
    ;; uint8 _n_radars
    1
    ;; int8 _radar_for_text
    1
    ;; float32[] _ant_height
    (* 4    (length _ant_height)) 4
    ;; float32[] _ant_pitch
    (* 4    (length _ant_pitch)) 4
    ;; float32[] _cam_euler_a
    (* 4    (length _cam_euler_a)) 4
    ;; float32[] _cam_euler_b
    (* 4    (length _cam_euler_b)) 4
    ;; float32[] _cam_euler_g
    (* 4    (length _cam_euler_g)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint8 _n_radars
       (write-byte _n_radars s)
     ;; int8 _radar_for_text
       (write-byte _radar_for_text s)
     ;; float32[] _ant_height
     (write-long (length _ant_height) s)
     (dotimes (i (length _ant_height))
       (sys::poke (elt _ant_height i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _ant_pitch
     (write-long (length _ant_pitch) s)
     (dotimes (i (length _ant_pitch))
       (sys::poke (elt _ant_pitch i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _cam_euler_a
     (write-long (length _cam_euler_a) s)
     (dotimes (i (length _cam_euler_a))
       (sys::poke (elt _cam_euler_a i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _cam_euler_b
     (write-long (length _cam_euler_b) s)
     (dotimes (i (length _cam_euler_b))
       (sys::poke (elt _cam_euler_b i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _cam_euler_g
     (write-long (length _cam_euler_g) s)
     (dotimes (i (length _cam_euler_g))
       (sys::poke (elt _cam_euler_g i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint8 _n_radars
     (setq _n_radars (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; int8 _radar_for_text
     (setq _radar_for_text (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> _radar_for_text 127) (setq _radar_for_text (- _radar_for_text 256)))
   ;; float32[] _ant_height
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _ant_height (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _ant_height i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _ant_pitch
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _ant_pitch (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _ant_pitch i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _cam_euler_a
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _cam_euler_a (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _cam_euler_a i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _cam_euler_b
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _cam_euler_b (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _cam_euler_b i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _cam_euler_g
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _cam_euler_g (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _cam_euler_g i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeSettingsPerRadar :md5sum-) "0ee305f5b1c77996631bbad5b0e5663d")
(setf (get arbe_msgs::arbeSettingsPerRadar :datatype-) "arbe_msgs/arbeSettingsPerRadar")
(setf (get arbe_msgs::arbeSettingsPerRadar :definition-)
      "uint8 n_radars
int8  radar_for_text
float32[] ant_height
float32[] ant_pitch
float32[] cam_euler_a
float32[] cam_euler_b
float32[] cam_euler_g

")



(provide :arbe_msgs/arbeSettingsPerRadar "0ee305f5b1c77996631bbad5b0e5663d")


