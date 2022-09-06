;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeClassificationMetadata)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeClassificationMetadata (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBECLASSIFICATIONMETADATA")
  (make-package "ARBE_MSGS::ARBECLASSIFICATIONMETADATA"))

(in-package "ROS")
;;//! \htmlinclude arbeClassificationMetadata.msg.html


(defclass arbe_msgs::arbeClassificationMetadata
  :super ros::object
  :slots (_enable_tta _post_factum_mode _average_mode _tta_decay _tta_momentum _conf_TH ))

(defmethod arbe_msgs::arbeClassificationMetadata
  (:init
   (&key
    ((:enable_tta __enable_tta) nil)
    ((:post_factum_mode __post_factum_mode) nil)
    ((:average_mode __average_mode) nil)
    ((:tta_decay __tta_decay) nil)
    ((:tta_momentum __tta_momentum) 0.0)
    ((:conf_TH __conf_TH) 0.0)
    )
   (send-super :init)
   (setq _enable_tta __enable_tta)
   (setq _post_factum_mode __post_factum_mode)
   (setq _average_mode __average_mode)
   (setq _tta_decay __tta_decay)
   (setq _tta_momentum (float __tta_momentum))
   (setq _conf_TH (float __conf_TH))
   self)
  (:enable_tta
   (&optional __enable_tta)
   (if __enable_tta (setq _enable_tta __enable_tta)) _enable_tta)
  (:post_factum_mode
   (&optional __post_factum_mode)
   (if __post_factum_mode (setq _post_factum_mode __post_factum_mode)) _post_factum_mode)
  (:average_mode
   (&optional __average_mode)
   (if __average_mode (setq _average_mode __average_mode)) _average_mode)
  (:tta_decay
   (&optional __tta_decay)
   (if __tta_decay (setq _tta_decay __tta_decay)) _tta_decay)
  (:tta_momentum
   (&optional __tta_momentum)
   (if __tta_momentum (setq _tta_momentum __tta_momentum)) _tta_momentum)
  (:conf_TH
   (&optional __conf_TH)
   (if __conf_TH (setq _conf_TH __conf_TH)) _conf_TH)
  (:serialization-length
   ()
   (+
    ;; bool _enable_tta
    1
    ;; bool _post_factum_mode
    1
    ;; bool _average_mode
    1
    ;; bool _tta_decay
    1
    ;; float32 _tta_momentum
    4
    ;; float32 _conf_TH
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; bool _enable_tta
       (if _enable_tta (write-byte -1 s) (write-byte 0 s))
     ;; bool _post_factum_mode
       (if _post_factum_mode (write-byte -1 s) (write-byte 0 s))
     ;; bool _average_mode
       (if _average_mode (write-byte -1 s) (write-byte 0 s))
     ;; bool _tta_decay
       (if _tta_decay (write-byte -1 s) (write-byte 0 s))
     ;; float32 _tta_momentum
       (sys::poke _tta_momentum (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _conf_TH
       (sys::poke _conf_TH (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; bool _enable_tta
     (setq _enable_tta (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _post_factum_mode
     (setq _post_factum_mode (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _average_mode
     (setq _average_mode (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; bool _tta_decay
     (setq _tta_decay (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; float32 _tta_momentum
     (setq _tta_momentum (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _conf_TH
     (setq _conf_TH (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeClassificationMetadata :md5sum-) "2def84262941647c9070003f42808b6a")
(setf (get arbe_msgs::arbeClassificationMetadata :datatype-) "arbe_msgs/arbeClassificationMetadata")
(setf (get arbe_msgs::arbeClassificationMetadata :definition-)
      "bool enable_tta
bool post_factum_mode
bool average_mode
bool tta_decay
float32 tta_momentum
float32 conf_TH

")



(provide :arbe_msgs/arbeClassificationMetadata "2def84262941647c9070003f42808b6a")


