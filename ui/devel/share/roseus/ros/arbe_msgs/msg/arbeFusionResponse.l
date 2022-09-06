;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeFusionResponse)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeFusionResponse (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEFUSIONRESPONSE")
  (make-package "ARBE_MSGS::ARBEFUSIONRESPONSE"))

(in-package "ROS")
;;//! \htmlinclude arbeFusionResponse.msg.html


(defclass arbe_msgs::arbeFusionResponse
  :super ros::object
  :slots (_valid _obj_id _query_id _fusionClass _obj_index _queryTime _conf _tta_count _tta_scores _class_str _fusionClasses _top _left _bottom _right ))

(defmethod arbe_msgs::arbeFusionResponse
  (:init
   (&key
    ((:valid __valid) nil)
    ((:obj_id __obj_id) 0)
    ((:query_id __query_id) 0)
    ((:fusionClass __fusionClass) 0)
    ((:obj_index __obj_index) 0)
    ((:queryTime __queryTime) 0)
    ((:conf __conf) 0.0)
    ((:tta_count __tta_count) 0)
    ((:tta_scores __tta_scores) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:class_str __class_str) "")
    ((:fusionClasses __fusionClasses) (instance arbe_msgs::arbeFusionClasses :init))
    ((:top __top) 0)
    ((:left __left) 0)
    ((:bottom __bottom) 0)
    ((:right __right) 0)
    )
   (send-super :init)
   (setq _valid __valid)
   (setq _obj_id (round __obj_id))
   (setq _query_id (round __query_id))
   (setq _fusionClass (round __fusionClass))
   (setq _obj_index (round __obj_index))
   (setq _queryTime (round __queryTime))
   (setq _conf (float __conf))
   (setq _tta_count (round __tta_count))
   (setq _tta_scores __tta_scores)
   (setq _class_str (string __class_str))
   (setq _fusionClasses __fusionClasses)
   (setq _top (round __top))
   (setq _left (round __left))
   (setq _bottom (round __bottom))
   (setq _right (round __right))
   self)
  (:valid
   (&optional __valid)
   (if __valid (setq _valid __valid)) _valid)
  (:obj_id
   (&optional __obj_id)
   (if __obj_id (setq _obj_id __obj_id)) _obj_id)
  (:query_id
   (&optional __query_id)
   (if __query_id (setq _query_id __query_id)) _query_id)
  (:fusionClass
   (&optional __fusionClass)
   (if __fusionClass (setq _fusionClass __fusionClass)) _fusionClass)
  (:obj_index
   (&optional __obj_index)
   (if __obj_index (setq _obj_index __obj_index)) _obj_index)
  (:queryTime
   (&optional __queryTime)
   (if __queryTime (setq _queryTime __queryTime)) _queryTime)
  (:conf
   (&optional __conf)
   (if __conf (setq _conf __conf)) _conf)
  (:tta_count
   (&optional __tta_count)
   (if __tta_count (setq _tta_count __tta_count)) _tta_count)
  (:tta_scores
   (&optional __tta_scores)
   (if __tta_scores (setq _tta_scores __tta_scores)) _tta_scores)
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
  (:top
   (&optional __top)
   (if __top (setq _top __top)) _top)
  (:left
   (&optional __left)
   (if __left (setq _left __left)) _left)
  (:bottom
   (&optional __bottom)
   (if __bottom (setq _bottom __bottom)) _bottom)
  (:right
   (&optional __right)
   (if __right (setq _right __right)) _right)
  (:serialization-length
   ()
   (+
    ;; bool _valid
    1
    ;; uint16 _obj_id
    2
    ;; uint32 _query_id
    4
    ;; uint8 _fusionClass
    1
    ;; uint16 _obj_index
    2
    ;; uint64 _queryTime
    8
    ;; float32 _conf
    4
    ;; uint8 _tta_count
    1
    ;; float32[] _tta_scores
    (* 4    (length _tta_scores)) 4
    ;; string _class_str
    4 (length _class_str)
    ;; arbe_msgs/arbeFusionClasses _fusionClasses
    (send _fusionClasses :serialization-length)
    ;; uint16 _top
    2
    ;; uint16 _left
    2
    ;; uint16 _bottom
    2
    ;; uint16 _right
    2
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; bool _valid
       (if _valid (write-byte -1 s) (write-byte 0 s))
     ;; uint16 _obj_id
       (write-word _obj_id s)
     ;; uint32 _query_id
       (write-long _query_id s)
     ;; uint8 _fusionClass
       (write-byte _fusionClass s)
     ;; uint16 _obj_index
       (write-word _obj_index s)
     ;; uint64 _queryTime
#+(or :alpha :irix6 :x86_64)
       (progn (sys::poke _queryTime (send s :buffer) (send s :count) :long) (incf (stream-count s) 8))
#-(or :alpha :irix6 :x86_64)
       (cond ((and (class _queryTime) (= (length (_queryTime . bv)) 2)) ;; bignum
              (write-long (ash (elt (_queryTime . bv) 0) 0) s)
              (write-long (ash (elt (_queryTime . bv) 1) -1) s))
             ((and (class _queryTime) (= (length (_queryTime . bv)) 1)) ;; big1
              (write-long (elt (_queryTime . bv) 0) s)
              (write-long (if (>= _queryTime 0) 0 #xffffffff) s))
             (t                                         ;; integer
              (write-long _queryTime s)(write-long (if (>= _queryTime 0) 0 #xffffffff) s)))
     ;; float32 _conf
       (sys::poke _conf (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; uint8 _tta_count
       (write-byte _tta_count s)
     ;; float32[] _tta_scores
     (write-long (length _tta_scores) s)
     (dotimes (i (length _tta_scores))
       (sys::poke (elt _tta_scores i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; string _class_str
       (write-long (length _class_str) s) (princ _class_str s)
     ;; arbe_msgs/arbeFusionClasses _fusionClasses
       (send _fusionClasses :serialize s)
     ;; uint16 _top
       (write-word _top s)
     ;; uint16 _left
       (write-word _left s)
     ;; uint16 _bottom
       (write-word _bottom s)
     ;; uint16 _right
       (write-word _right s)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; bool _valid
     (setq _valid (not (= 0 (sys::peek buf ptr- :char)))) (incf ptr- 1)
   ;; uint16 _obj_id
     (setq _obj_id (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint32 _query_id
     (setq _query_id (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; uint8 _fusionClass
     (setq _fusionClass (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; uint16 _obj_index
     (setq _obj_index (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint64 _queryTime
     
#+(or :alpha :irix6 :x86_64)
      (setf _queryTime (prog1 (sys::peek buf ptr- :long) (incf ptr- 8)))
#-(or :alpha :irix6 :x86_64)
      (setf _queryTime (let ((b0 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4)))
                  (b1 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4))))
              (cond ((= b1 -1) b0)
                     ((and (= b1  0)
                           (<= lisp::most-negative-fixnum b0 lisp::most-positive-fixnum))
                      b0)
                    ((= b1  0) (make-instance bignum :size 1 :bv (integer-vector b0)))
                    (t (make-instance bignum :size 2 :bv (integer-vector b0 (ash b1 1)))))))
   ;; float32 _conf
     (setq _conf (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; uint8 _tta_count
     (setq _tta_count (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; float32[] _tta_scores
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _tta_scores (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _tta_scores i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; string _class_str
     (let (n) (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4) (setq _class_str (subseq buf ptr- (+ ptr- n))) (incf ptr- n))
   ;; arbe_msgs/arbeFusionClasses _fusionClasses
     (send _fusionClasses :deserialize buf ptr-) (incf ptr- (send _fusionClasses :serialization-length))
   ;; uint16 _top
     (setq _top (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _left
     (setq _left (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _bottom
     (setq _bottom (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _right
     (setq _right (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;;
   self)
  )

(setf (get arbe_msgs::arbeFusionResponse :md5sum-) "9d16e7674f130b5c070b9bc414f29ed5")
(setf (get arbe_msgs::arbeFusionResponse :datatype-) "arbe_msgs/arbeFusionResponse")
(setf (get arbe_msgs::arbeFusionResponse :definition-)
      "bool valid
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



(provide :arbe_msgs/arbeFusionResponse "9d16e7674f130b5c070b9bc414f29ed5")


