;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeThresholds)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeThresholds (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBETHRESHOLDS")
  (make-package "ARBE_MSGS::ARBETHRESHOLDS"))

(in-package "ROS")
;;//! \htmlinclude arbeThresholds.msg.html


(defclass arbe_msgs::arbeThresholds
  :super ros::object
  :slots (_Header _Info ))

(defmethod arbe_msgs::arbeThresholds
  (:init
   (&key
    ((:Header __Header) (make-array 0 :initial-element 0 :element-type :char))
    ((:Info __Info) (make-array 0 :initial-element 0 :element-type :char))
    )
   (send-super :init)
   (setq _Header __Header)
   (setq _Info __Info)
   self)
  (:Header
   (&optional __Header)
   (if __Header (setq _Header __Header)) _Header)
  (:Info
   (&optional __Info)
   (if __Info (setq _Info __Info)) _Info)
  (:serialization-length
   ()
   (+
    ;; uint8[] _Header
    (* 1    (length _Header)) 4
    ;; uint8[] _Info
    (* 1    (length _Info)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint8[] _Header
     (write-long (length _Header) s)
     (princ _Header s)
     ;; uint8[] _Info
     (write-long (length _Info) s)
     (princ _Info s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint8[] _Header
   (let ((n (sys::peek buf ptr- :integer))) (incf ptr- 4)
     (setq _Header (make-array n :element-type :char))
     (replace _Header buf :start2 ptr-) (incf ptr- n))
   ;; uint8[] _Info
   (let ((n (sys::peek buf ptr- :integer))) (incf ptr- 4)
     (setq _Info (make-array n :element-type :char))
     (replace _Info buf :start2 ptr-) (incf ptr- n))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeThresholds :md5sum-) "8d95b90f43b0c5ea15e47b0302160dcc")
(setf (get arbe_msgs::arbeThresholds :datatype-) "arbe_msgs/arbeThresholds")
(setf (get arbe_msgs::arbeThresholds :definition-)
      "uint8[] Header
uint8[] Info

")



(provide :arbe_msgs/arbeThresholds "8d95b90f43b0c5ea15e47b0302160dcc")


