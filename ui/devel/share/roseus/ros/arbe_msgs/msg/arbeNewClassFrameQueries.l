;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeNewClassFrameQueries)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeNewClassFrameQueries (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBENEWCLASSFRAMEQUERIES")
  (make-package "ARBE_MSGS::ARBENEWCLASSFRAMEQUERIES"))

(in-package "ROS")
;;//! \htmlinclude arbeNewClassFrameQueries.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeNewClassFrameQueries
  :super ros::object
  :slots (_Header _class_metadata _frame_counter _queries _mutual_len ))

(defmethod arbe_msgs::arbeNewClassFrameQueries
  (:init
   (&key
    ((:Header __Header) (instance std_msgs::Header :init))
    ((:class_metadata __class_metadata) (instance arbe_msgs::arbeClassificationMetadata :init))
    ((:frame_counter __frame_counter) 0)
    ((:queries __queries) (let (r) (dotimes (i 0) (push (instance arbe_msgs::arbeClassSingleQuery :init) r)) r))
    ((:mutual_len __mutual_len) 0)
    )
   (send-super :init)
   (setq _Header __Header)
   (setq _class_metadata __class_metadata)
   (setq _frame_counter (round __frame_counter))
   (setq _queries __queries)
   (setq _mutual_len (round __mutual_len))
   self)
  (:Header
   (&rest __Header)
   (if (keywordp (car __Header))
       (send* _Header __Header)
     (progn
       (if __Header (setq _Header (car __Header)))
       _Header)))
  (:class_metadata
   (&rest __class_metadata)
   (if (keywordp (car __class_metadata))
       (send* _class_metadata __class_metadata)
     (progn
       (if __class_metadata (setq _class_metadata (car __class_metadata)))
       _class_metadata)))
  (:frame_counter
   (&optional __frame_counter)
   (if __frame_counter (setq _frame_counter __frame_counter)) _frame_counter)
  (:queries
   (&rest __queries)
   (if (keywordp (car __queries))
       (send* _queries __queries)
     (progn
       (if __queries (setq _queries (car __queries)))
       _queries)))
  (:mutual_len
   (&optional __mutual_len)
   (if __mutual_len (setq _mutual_len __mutual_len)) _mutual_len)
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _Header
    (send _Header :serialization-length)
    ;; arbe_msgs/arbeClassificationMetadata _class_metadata
    (send _class_metadata :serialization-length)
    ;; uint16 _frame_counter
    2
    ;; arbe_msgs/arbeClassSingleQuery[] _queries
    (apply #'+ (send-all _queries :serialization-length)) 4
    ;; uint16 _mutual_len
    2
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _Header
       (send _Header :serialize s)
     ;; arbe_msgs/arbeClassificationMetadata _class_metadata
       (send _class_metadata :serialize s)
     ;; uint16 _frame_counter
       (write-word _frame_counter s)
     ;; arbe_msgs/arbeClassSingleQuery[] _queries
     (write-long (length _queries) s)
     (dolist (elem _queries)
       (send elem :serialize s)
       )
     ;; uint16 _mutual_len
       (write-word _mutual_len s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _Header
     (send _Header :deserialize buf ptr-) (incf ptr- (send _Header :serialization-length))
   ;; arbe_msgs/arbeClassificationMetadata _class_metadata
     (send _class_metadata :deserialize buf ptr-) (incf ptr- (send _class_metadata :serialization-length))
   ;; uint16 _frame_counter
     (setq _frame_counter (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; arbe_msgs/arbeClassSingleQuery[] _queries
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _queries (let (r) (dotimes (i n) (push (instance arbe_msgs::arbeClassSingleQuery :init) r)) r))
     (dolist (elem- _queries)
     (send elem- :deserialize buf ptr-) (incf ptr- (send elem- :serialization-length))
     ))
   ;; uint16 _mutual_len
     (setq _mutual_len (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeNewClassFrameQueries :md5sum-) "01f7dd27368405062b7c22b947b01851")
(setf (get arbe_msgs::arbeNewClassFrameQueries :datatype-) "arbe_msgs/arbeNewClassFrameQueries")
(setf (get arbe_msgs::arbeNewClassFrameQueries :definition-)
      "std_msgs/Header Header
arbeClassificationMetadata class_metadata
uint16 frame_counter
arbeClassSingleQuery[] queries
uint16 mutual_len


================================================================================
MSG: std_msgs/Header
# Standard metadata for higher-level stamped data types.
# This is generally used to communicate timestamped data 
# in a particular coordinate frame.
# 
# sequence ID: consecutively increasing ID 
uint32 seq
#Two-integer timestamp that is expressed as:
# * stamp.sec: seconds (stamp_secs) since epoch (in Python the variable is called 'secs')
# * stamp.nsec: nanoseconds since stamp_secs (in Python the variable is called 'nsecs')
# time-handling sugar is provided by the client library
time stamp
#Frame this data is associated with
string frame_id

================================================================================
MSG: arbe_msgs/arbeClassificationMetadata
bool enable_tta
bool post_factum_mode
bool average_mode
bool tta_decay
float32 tta_momentum
float32 conf_TH

================================================================================
MSG: arbe_msgs/arbeClassSingleQuery
uint16 obj_id
uint16 obj_index
uint16 n_points
float32[] xyzvp
float32[] xyzvp_center
float32[] tta_scores
uint32 tta_count


")



(provide :arbe_msgs/arbeNewClassFrameQueries "01f7dd27368405062b7c22b947b01851")


