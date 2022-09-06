;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeFusionResponsesVec)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeFusionResponsesVec (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEFUSIONRESPONSESVEC")
  (make-package "ARBE_MSGS::ARBEFUSIONRESPONSESVEC"))

(in-package "ROS")
;;//! \htmlinclude arbeFusionResponsesVec.msg.html
(if (not (find-package "STD_MSGS"))
  (ros::roseus-add-msgs "std_msgs"))


(defclass arbe_msgs::arbeFusionResponsesVec
  :super ros::object
  :slots (_header _dt _responses ))

(defmethod arbe_msgs::arbeFusionResponsesVec
  (:init
   (&key
    ((:header __header) (instance std_msgs::Header :init))
    ((:dt __dt) 0)
    ((:responses __responses) (let (r) (dotimes (i 0) (push (instance arbe_msgs::arbeFusionResponse :init) r)) r))
    )
   (send-super :init)
   (setq _header __header)
   (setq _dt (round __dt))
   (setq _responses __responses)
   self)
  (:header
   (&rest __header)
   (if (keywordp (car __header))
       (send* _header __header)
     (progn
       (if __header (setq _header (car __header)))
       _header)))
  (:dt
   (&optional __dt)
   (if __dt (setq _dt __dt)) _dt)
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
    ;; uint64 _dt
    8
    ;; arbe_msgs/arbeFusionResponse[] _responses
    (apply #'+ (send-all _responses :serialization-length)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; std_msgs/Header _header
       (send _header :serialize s)
     ;; uint64 _dt
#+(or :alpha :irix6 :x86_64)
       (progn (sys::poke _dt (send s :buffer) (send s :count) :long) (incf (stream-count s) 8))
#-(or :alpha :irix6 :x86_64)
       (cond ((and (class _dt) (= (length (_dt . bv)) 2)) ;; bignum
              (write-long (ash (elt (_dt . bv) 0) 0) s)
              (write-long (ash (elt (_dt . bv) 1) -1) s))
             ((and (class _dt) (= (length (_dt . bv)) 1)) ;; big1
              (write-long (elt (_dt . bv) 0) s)
              (write-long (if (>= _dt 0) 0 #xffffffff) s))
             (t                                         ;; integer
              (write-long _dt s)(write-long (if (>= _dt 0) 0 #xffffffff) s)))
     ;; arbe_msgs/arbeFusionResponse[] _responses
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
   ;; uint64 _dt
     
#+(or :alpha :irix6 :x86_64)
      (setf _dt (prog1 (sys::peek buf ptr- :long) (incf ptr- 8)))
#-(or :alpha :irix6 :x86_64)
      (setf _dt (let ((b0 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4)))
                  (b1 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4))))
              (cond ((= b1 -1) b0)
                     ((and (= b1  0)
                           (<= lisp::most-negative-fixnum b0 lisp::most-positive-fixnum))
                      b0)
                    ((= b1  0) (make-instance bignum :size 1 :bv (integer-vector b0)))
                    (t (make-instance bignum :size 2 :bv (integer-vector b0 (ash b1 1)))))))
   ;; arbe_msgs/arbeFusionResponse[] _responses
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _responses (let (r) (dotimes (i n) (push (instance arbe_msgs::arbeFusionResponse :init) r)) r))
     (dolist (elem- _responses)
     (send elem- :deserialize buf ptr-) (incf ptr- (send elem- :serialization-length))
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeFusionResponsesVec :md5sum-) "e85897c08185dc25f773baf17831351f")
(setf (get arbe_msgs::arbeFusionResponsesVec :datatype-) "arbe_msgs/arbeFusionResponsesVec")
(setf (get arbe_msgs::arbeFusionResponsesVec :definition-)
      "std_msgs/Header header
uint64 dt
arbeFusionResponse[] responses

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
MSG: arbe_msgs/arbeFusionResponse
bool valid
uint16 obj_id
uint32 query_id
uint8 fusionClass
uint16 obj_index
uint64 queryTime
float32 conf
uint8 tta_count
float32[] tta_scores
string class_str
arbeFusionClasses fusionClasses

uint16  top
uint16  left
uint16  bottom
uint16  right

================================================================================
MSG: arbe_msgs/arbeFusionClasses
uint16 FC_BIKE = 0
uint16 FC_TRUCK_BUS = 1
uint16 FC_VRU = 2
uint16 FC_CAR = 3
uint16 FC_NOT_CLASSIFIED = 4
uint16 NUM_CLASSES = 4

")



(provide :arbe_msgs/arbeFusionResponsesVec "e85897c08185dc25f773baf17831351f")


