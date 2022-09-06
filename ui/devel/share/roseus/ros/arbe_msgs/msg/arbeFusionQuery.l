;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeFusionQuery)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeFusionQuery (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBEFUSIONQUERY")
  (make-package "ARBE_MSGS::ARBEFUSIONQUERY"))

(in-package "ROS")
;;//! \htmlinclude arbeFusionQuery.msg.html


(defclass arbe_msgs::arbeFusionQuery
  :super ros::object
  :slots (_obj_id _top _left _bottom _right _query_id _query_time _obj_index _tta_count _tta_scores ))

(defmethod arbe_msgs::arbeFusionQuery
  (:init
   (&key
    ((:obj_id __obj_id) 0)
    ((:top __top) 0)
    ((:left __left) 0)
    ((:bottom __bottom) 0)
    ((:right __right) 0)
    ((:query_id __query_id) 0)
    ((:query_time __query_time) 0)
    ((:obj_index __obj_index) 0)
    ((:tta_count __tta_count) 0)
    ((:tta_scores __tta_scores) (make-array 0 :initial-element 0.0 :element-type :float))
    )
   (send-super :init)
   (setq _obj_id (round __obj_id))
   (setq _top (round __top))
   (setq _left (round __left))
   (setq _bottom (round __bottom))
   (setq _right (round __right))
   (setq _query_id (round __query_id))
   (setq _query_time (round __query_time))
   (setq _obj_index (round __obj_index))
   (setq _tta_count (round __tta_count))
   (setq _tta_scores __tta_scores)
   self)
  (:obj_id
   (&optional __obj_id)
   (if __obj_id (setq _obj_id __obj_id)) _obj_id)
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
  (:query_id
   (&optional __query_id)
   (if __query_id (setq _query_id __query_id)) _query_id)
  (:query_time
   (&optional __query_time)
   (if __query_time (setq _query_time __query_time)) _query_time)
  (:obj_index
   (&optional __obj_index)
   (if __obj_index (setq _obj_index __obj_index)) _obj_index)
  (:tta_count
   (&optional __tta_count)
   (if __tta_count (setq _tta_count __tta_count)) _tta_count)
  (:tta_scores
   (&optional __tta_scores)
   (if __tta_scores (setq _tta_scores __tta_scores)) _tta_scores)
  (:serialization-length
   ()
   (+
    ;; uint16 _obj_id
    2
    ;; uint16 _top
    2
    ;; uint16 _left
    2
    ;; uint16 _bottom
    2
    ;; uint16 _right
    2
    ;; uint32 _query_id
    4
    ;; uint64 _query_time
    8
    ;; uint16 _obj_index
    2
    ;; uint8 _tta_count
    1
    ;; float32[] _tta_scores
    (* 4    (length _tta_scores)) 4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint16 _obj_id
       (write-word _obj_id s)
     ;; uint16 _top
       (write-word _top s)
     ;; uint16 _left
       (write-word _left s)
     ;; uint16 _bottom
       (write-word _bottom s)
     ;; uint16 _right
       (write-word _right s)
     ;; uint32 _query_id
       (write-long _query_id s)
     ;; uint64 _query_time
#+(or :alpha :irix6 :x86_64)
       (progn (sys::poke _query_time (send s :buffer) (send s :count) :long) (incf (stream-count s) 8))
#-(or :alpha :irix6 :x86_64)
       (cond ((and (class _query_time) (= (length (_query_time . bv)) 2)) ;; bignum
              (write-long (ash (elt (_query_time . bv) 0) 0) s)
              (write-long (ash (elt (_query_time . bv) 1) -1) s))
             ((and (class _query_time) (= (length (_query_time . bv)) 1)) ;; big1
              (write-long (elt (_query_time . bv) 0) s)
              (write-long (if (>= _query_time 0) 0 #xffffffff) s))
             (t                                         ;; integer
              (write-long _query_time s)(write-long (if (>= _query_time 0) 0 #xffffffff) s)))
     ;; uint16 _obj_index
       (write-word _obj_index s)
     ;; uint8 _tta_count
       (write-byte _tta_count s)
     ;; float32[] _tta_scores
     (write-long (length _tta_scores) s)
     (dotimes (i (length _tta_scores))
       (sys::poke (elt _tta_scores i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; uint16 _obj_id
     (setq _obj_id (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _top
     (setq _top (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _left
     (setq _left (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _bottom
     (setq _bottom (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _right
     (setq _right (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint32 _query_id
     (setq _query_id (sys::peek buf ptr- :integer)) (incf ptr- 4)
   ;; uint64 _query_time
     
#+(or :alpha :irix6 :x86_64)
      (setf _query_time (prog1 (sys::peek buf ptr- :long) (incf ptr- 8)))
#-(or :alpha :irix6 :x86_64)
      (setf _query_time (let ((b0 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4)))
                  (b1 (prog1 (sys::peek buf ptr- :integer) (incf ptr- 4))))
              (cond ((= b1 -1) b0)
                     ((and (= b1  0)
                           (<= lisp::most-negative-fixnum b0 lisp::most-positive-fixnum))
                      b0)
                    ((= b1  0) (make-instance bignum :size 1 :bv (integer-vector b0)))
                    (t (make-instance bignum :size 2 :bv (integer-vector b0 (ash b1 1)))))))
   ;; uint16 _obj_index
     (setq _obj_index (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint8 _tta_count
     (setq _tta_count (sys::peek buf ptr- :char)) (incf ptr- 1)
   ;; float32[] _tta_scores
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _tta_scores (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _tta_scores i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;;
   self)
  )

(setf (get arbe_msgs::arbeFusionQuery :md5sum-) "d6277f1c47cde1be60fd589e75618a8d")
(setf (get arbe_msgs::arbeFusionQuery :datatype-) "arbe_msgs/arbeFusionQuery")
(setf (get arbe_msgs::arbeFusionQuery :definition-)
      "uint16  	obj_id
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



(provide :arbe_msgs/arbeFusionQuery "d6277f1c47cde1be60fd589e75618a8d")


