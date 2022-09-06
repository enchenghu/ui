;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeCameraInstallationParams)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeCameraInstallationParams (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBECAMERAINSTALLATIONPARAMS")
  (make-package "ARBE_MSGS::ARBECAMERAINSTALLATIONPARAMS"))

(in-package "ROS")
;;//! \htmlinclude arbeCameraInstallationParams.msg.html


(defclass arbe_msgs::arbeCameraInstallationParams
  :super ros::object
  :slots (_intrinsic _extrinsic_trans _euler_a_b_g ))

(defmethod arbe_msgs::arbeCameraInstallationParams
  (:init
   (&key
    ((:intrinsic __intrinsic) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:extrinsic_trans __extrinsic_trans) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:euler_a_b_g __euler_a_b_g) (make-array 0 :initial-element 0.0 :element-type :float))
    )
   (send-super :init)
   (setq _intrinsic __intrinsic)
   (setq _extrinsic_trans __extrinsic_trans)
   (setq _euler_a_b_g __euler_a_b_g)
   self)
  (:intrinsic
   (&optional __intrinsic)
   (if __intrinsic (setq _intrinsic __intrinsic)) _intrinsic)
  (:extrinsic_trans
   (&optional __extrinsic_trans)
   (if __extrinsic_trans (setq _extrinsic_trans __extrinsic_trans)) _extrinsic_trans)
  (:euler_a_b_g
   (&optional __euler_a_b_g)
   (if __euler_a_b_g (setq _euler_a_b_g __euler_a_b_g)) _euler_a_b_g)
  (:serialization-length
   ()
   (+
    ;; float32[] _intrinsic
    (* 4    (length _intrinsic)) 4
    ;; float32[] _extrinsic_trans
    (* 4    (length _extrinsic_trans)) 4
    ;; float32[] _euler_a_b_g
    (* 4    (length _euler_a_b_g)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32[] _intrinsic
     (write-long (length _intrinsic) s)
     (dotimes (i (length _intrinsic))
       (sys::poke (elt _intrinsic i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _extrinsic_trans
     (write-long (length _extrinsic_trans) s)
     (dotimes (i (length _extrinsic_trans))
       (sys::poke (elt _extrinsic_trans i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _euler_a_b_g
     (write-long (length _euler_a_b_g) s)
     (dotimes (i (length _euler_a_b_g))
       (sys::poke (elt _euler_a_b_g i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32[] _intrinsic
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _intrinsic (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _intrinsic i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _extrinsic_trans
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _extrinsic_trans (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _extrinsic_trans i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _euler_a_b_g
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _euler_a_b_g (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _euler_a_b_g i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeCameraInstallationParams :md5sum-) "5d1e488e854ebf395d5c87a2670d2622")
(setf (get arbe_msgs::arbeCameraInstallationParams :datatype-) "arbe_msgs/arbeCameraInstallationParams")
(setf (get arbe_msgs::arbeCameraInstallationParams :definition-)
      "float32[] intrinsic
float32[] extrinsic_trans
float32[] euler_a_b_g

")



(provide :arbe_msgs/arbeCameraInstallationParams "5d1e488e854ebf395d5c87a2670d2622")


