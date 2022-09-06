;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeFusionQueriesVec)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeFusionQueriesVec (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEFUSIONQUERIESVEC")
  (make-package "ARBE_MSGS::ARBEFUSIONQUERIESVEC"))

(in-package "ROS")
;;//! \htmlinclude arbeFusionQueriesVec.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeFusionQueriesVec
  :super ros::object
  :slots (_header _queries ))

(defmethod arbe_msgs::arbeFusionQueriesVec
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:queries __queries) (let (r) (dotimes (i 0) (push (instance arbe_msgs::arbeFusionQuery :init) r)) r))
    )
   (send-super :init)
   (setq _header __header)
   (setq _queries __queries)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:queries
   (&rest __queries)
   (if (keywordp (car __queries))
       (send* _queries __queries)
     (progn
       (if __queries (setq _queries (car __queries)))
       _queries)))
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; arbe_msgs/arbeFusionQuery[] _queries
    (apply #'+ (send-all _queries :serialization-length)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; arbe_msgs/arbeFusionQuery[] _queries
     (write-long (length _queries) s)
     (dolist (elem _queries)
       (send elem :serialize s)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; arbe_msgs/arbeFusionQuery[] _queries
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _queries (let (r) (dotimes (i n) (push (instance arbe_msgs::arbeFusionQuery :init) r)) r))
     (dolist (elem- _queries)
     (send elem- :deserialize buf ptr-) (incf ptr- (send elem- :serialization-length))
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeFusionQueriesVec :md5sum-) "ba44955926ecbdeed3c3d177166ac4a8")
(setf (get arbe_msgs::arbeFusionQueriesVec :datatype-) "arbe_msgs/arbeFusionQueriesVec")
(setf (get arbe_msgs::arbeFusionQueriesVec :definition-)
      "std_msgs/Header header
arbeFusionQuery[] queries

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
MSG: arbe_msgs/arbeFusionQuery
uint16  	obj_id
uint16  	top
uint16  	left
uint16  	bottom
uint16  	right
uint32  	query_id
uint64	  	query_time
uint16  	obj_index
uint8  	tta_count
float32[] tta_scores


")



(provide :arbe_msgs/arbeFusionQueriesVec "ba44955926ecbdeed3c3d177166ac4a8")


