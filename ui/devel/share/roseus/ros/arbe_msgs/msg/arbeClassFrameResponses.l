;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeClassFrameResponses)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeClassFrameResponses (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBECLASSFRAMERESPONSES")
  (make-package "ARBE_MSGS::ARBECLASSFRAMERESPONSES"))

(in-package "ROS")
;;//! \htmlinclude arbeClassFrameResponses.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeClassFrameResponses
  :super ros::object
  :slots (_header _frame_counter _process_time_ms _responses ))

(defmethod arbe_msgs::arbeClassFrameResponses
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:frame_counter __frame_counter) 0)
    ((:process_time_ms __process_time_ms) 0)
    ((:responses __responses) (let (r) (dotimes (i 0) (push (instance arbe_msgs::arbeClassSingleResponse :init) r)) r))
    )
   (send-super :init)
   (setq _header __header)
   (setq _frame_counter (round __frame_counter))
   (setq _process_time_ms (round __process_time_ms))
   (setq _responses __responses)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:frame_counter
   (&optional __frame_counter)
   (if __frame_counter (setq _frame_counter __frame_counter)) _frame_counter)
  (:process_time_ms
   (&optional __process_time_ms)
   (if __process_time_ms (setq _process_time_ms __process_time_ms)) _process_time_ms)
  (:responses
   (&rest __responses)
   (if (keywordp (car __responses))
       (send* _responses __responses)
     (progn
       (if __responses (setq _responses (car __responses)))
       _responses)))
  (:serialization-length
   ()
   (+
    ;; std_msgs/Header _header
    (send _header :serialization-length)
    ;; uint16 _frame_counter
    2
    ;; uint64 _process_time_ms
    8
    ;; arbe_msgs/arbeClassSingleResponse[] _responses
    (apply #'+ (send-all _responses :serialization-length)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; uint16 _frame_counter
       (write-word _frame_counter s)
     ;; uint64 _process_time_ms
#+(or :alpha :irix6 :x86_64)
       (progn (sys::poke _process_time_ms (send s :buffer) (send s :count) :long) (incf (stream-count s) 8))
#-(or :alpha :irix6 :x86_64)
       (cond ((and (class _process_time_ms) (= (length (_process_time_ms . bv)) 2)) ;; bignum
              (write-long (ash (elt (_process_time_ms . bv) 0) 0) s)
              (write-long (ash (elt (_process_time_ms . bv) 1) -1) s))
             ((and (class _process_time_ms) (= (length (_process_time_ms . bv)) 1)) ;; big1
              (write-long (elt (_process_time_ms . bv) 0) s)
              (write-long (if (>= _process_time_ms 0) 0 #xffffffff) s))
             (t                                         ;; integer
              (write-long _process_time_ms s)(write-long (if (>= _process_time_ms 0) 0 #xffffffff) s)))
     ;; arbe_msgs/arbeClassSingleResponse[] _responses
     (write-long (length _responses) s)
     (dolist (elem _responses)
       (send elem :serialize s)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; std_msgs/Header _header
     (send _header :deserialize buf ptr-) (incf ptr- (send _header :serialization-length))
   ;; uint16 _frame_counter
     (setq _frame_counter (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint64 _process_time_ms
     
#+(or :alpha :irix6 :x86_64)
      (setf _process_time_ms (prog1 (sys::peek buf ptr- :long) (incf ptr- 8)))
#-(or :alpha :irix6 :x86_64)
      (setf _process_time_ms (let ((b0 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4)))
                  (b1 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4))))
              (cond ((= b1 -1) b0)
                     ((and (= b1  0)
                           (<= lisp::most-negative-fixnum b0 lisp::most-positive-fixnum))
                      b0)
                    ((= b1  0) (make-instance bignum :size 1 :bv (integer-vector b0)))
                    (t (make-instance bignum :size 2 :bv (integer-vector b0 (ash b1 1)))))))
   ;; arbe_msgs/arbeClassSingleResponse[] _responses
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _responses (let (r) (dotimes (i n) (push (instance arbe_msgs::arbeClassSingleResponse :init) r)) r))
     (dolist (elem- _responses)
     (send elem- :deserialize buf ptr-) (incf ptr- (send elem- :serialization-length))
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeClassFrameResponses :md5sum-) "5abc2de3f3441b5705fbb97c192204d4")
(setf (get arbe_msgs::arbeClassFrameResponses :datatype-) "arbe_msgs/arbeClassFrameResponses")
(setf (get arbe_msgs::arbeClassFrameResponses :definition-)
      "std_msgs/Header header
uint16 frame_counter
uint64 process_time_ms
arbeClassSingleResponse[] responses

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
MSG: arbe_msgs/arbeClassSingleResponse
bool valid
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



(provide :arbe_msgs/arbeClassFrameResponses "5abc2de3f3441b5705fbb97c192204d4")


