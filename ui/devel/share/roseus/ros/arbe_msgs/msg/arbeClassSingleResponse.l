;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeClassSingleResponse)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeClassSingleResponse (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBECLASSSINGLERESPONSE")
  (make-package "ARBE_MSGS::ARBECLASSSINGLERESPONSE"))

(in-package "ROS")
;;//! \htmlinclude arbeClassSingleResponse.msg.html


(defclass arbe_msgs::arbeClassSingleResponse
  :super ros::object
  :slots (_valid _obj_id _obj_index _inferredClass _conf _class_str _fusionClasses _tta_scores _tta_count ))

(defmethod arbe_msgs::arbeClassSingleResponse
  (:init
   (&key
    ((:valid __valid) nil)
    ((:obj_id __obj_id) 0)
    ((:obj_index __obj_index) 0)
    ((:inferredClass __inferredClass) 0)
    ((:conf __conf) 0.0)
    ((:class_str __class_str) "")
    ((:fusionClasses __fusionClasses) (instance arbe_msgs::arbeFusionClasses :init))
    ((:tta_scores __tta_scores) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:tta_count __tta_count) 0)
    )
   (send-super :init)
   (setq _valid __valid)
   (setq _obj_id (round __obj_id))
   (setq _obj_index (round __obj_index))
   (setq _inferredClass (round __inferredClass))
   (setq _conf (float __conf))
   (setq _class_str (string __class_str))
   (setq _fusionClasses __fusionClasses)
   (setq _tta_scores __tta_scores)
   (setq _tta_count (round __tta_count))
   self)
  (:valid
   (&optional __valid)
   (if __valid (setq _valid __valid)) _valid)
  (:obj_id
   (&optional __obj_id)
   (if __obj_id (setq _obj_id __obj_id)) _obj_id)
  (:obj_index
   (&optional __obj_index)
   (if __obj_index (setq _obj_index __obj_index)) _obj_index)
  (:inferredClass
   (&optional __inferredClass)
   (if __inferredClass (setq _inferredClass __inferredClass)) _inferredClass)
  (:conf
   (&optional __conf)
   (if __conf (setq _conf __conf)) _conf)
  (:class_str
   (&optional __class_str)
   (if __class_str (setq _class_str __class_str)) _class_str)
  (:fusionClasses
   (&rest __fusionClasses)
   (if (keywordp (car __fusionClasses))
       (send* _fusionClasses __fusionClasses)
     (progn
       (if __fusionClasses (setq _fusionClasses (car __fusionClasses)))
       _fusionClasses)))
  (:tta_scores
   (&optional __tta_scores)
   (if __tta_scores (setq _tta_scores __tta_scores)) _tta_scores)
  (:tta_count
   (&optional __tta_count)
   (if __tta_count (setq _tta_count __tta_count)) _tta_count)
  (:serialization-length
   ()
   (+
    ;; bool _valid
    1
    ;; uint16 _obj_id
    2
    ;; uint16 _obj_index
    2
    ;; uint8 _inferredClass
    1
    ;; float32 _conf
    4
    ;; string _class_str
    4 (length _class_str)
    ;; arbe_msgs/arbeFusionClasses _fusionClasses
    (send _fusionClasses :serialization-length)
    ;; float32[] _tta_scores
    (* 4    (length _tta_scores)) 4
    ;; uint32 _tta_count
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; bool _valid
       (if _valid (write-byte -1 s) (write-byte 0 s))
     ;; uint16 _obj_id
       (write-word _obj_id s)
     ;; uint16 _obj_index
       (write-word _obj_index s)
     ;; uint8 _inferredClass
       (write-byte _inferredClass s)
     ;; float32 _conf
       (sys::poke _conf (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; string _class_str
       (write-long (length _class_str) s) (princ _class_str s)
     ;; arbe_msgs/arbeFusionClasses _fusionClasses
       (send _fusionClasses :serialize s)
     ;; float32[] _tta_scores
     (write-long (length _tta_scores) s)
     (dotimes (i (length _tta_scores))
       (sys::poke (elt _tta_scores i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; uint32 _tta_count
       (write-long _tta_count s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; bool _valid
     (setq _valid (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; uint16 _obj_id
     (setq _obj_id (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _obj_index
     (setq _obj_index (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint8 _inferredClass
     (setq _inferredClass (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; float32 _conf
     (setq _conf (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; string _class_str
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _class_str (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; arbe_msgs/arbeFusionClasses _fusionClasses
     (send _fusionClasses :deserialize buf ptr-) (incf ptr- (send _fusionClasses :serialization-length))
   ;; float32[] _tta_scores
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _tta_scores (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _tta_scores i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; uint32 _tta_count
     (setq _tta_count (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeClassSingleResponse :md5sum-) "1b400a19f8488a87ab3155d78880124a")
(setf (get arbe_msgs::arbeClassSingleResponse :datatype-) "arbe_msgs/arbeClassSingleResponse")
(setf (get arbe_msgs::arbeClassSingleResponse :definition-)
      "bool valid
uint16 obj_id
uint16 obj_index
uint8 inferredClass
float32 conf
string class_str
arbeFusionClasses fusionClasses
float32[] tta_scores
uint32 tta_count

================================================================================
MSG: arbe_msgs/arbeFusionClasses
uint16 FC_BIKE = 0
uint16 FC_TRUCK_BUS = 1
uint16 FC_VRU = 2
uint16 FC_CAR = 3
uint16 FC_NOT_CLASSIFIED = 4
uint16 NUM_CLASSES = 4

")



(provide :arbe_msgs/arbeClassSingleResponse "1b400a19f8488a87ab3155d78880124a")


