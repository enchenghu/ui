;; Auto-generated. Do not edit!


(when (boundp 'arbe_msgs::arbeClassSingleQuery)
  (if (not (find-package "ARBE_MSGS"))
    (make-package "ARBE_MSGS"))
  (shadow 'arbeClassSingleQuery (find-package "ARBE_MSGS")))
(unless (find-package "ARBE_MSGS::ARBECLASSSINGLEQUERY")
  (make-package "ARBE_MSGS::ARBECLASSSINGLEQUERY"))

(in-package "ROS")
;;//! \htmlinclude arbeClassSingleQuery.msg.html


(defclass arbe_msgs::arbeClassSingleQuery
  :super ros::object
  :slots (_obj_id _obj_index _n_points _xyzvp _xyzvp_center _tta_scores _tta_count ))

(defmethod arbe_msgs::arbeClassSingleQuery
  (:init
   (&key
    ((:obj_id __obj_id) 0)
    ((:obj_index __obj_index) 0)
    ((:n_points __n_points) 0)
    ((:xyzvp __xyzvp) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:xyzvp_center __xyzvp_center) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:tta_scores __tta_scores) (make-array 0 :initial-element 0.0 :element-type :float))
    ((:tta_count __tta_count) 0)
    )
   (send-super :init)
   (setq _obj_id (round __obj_id))
   (setq _obj_index (round __obj_index))
   (setq _n_points (round __n_points))
   (setq _xyzvp __xyzvp)
   (setq _xyzvp_center __xyzvp_center)
   (setq _tta_scores __tta_scores)
   (setq _tta_count (round __tta_count))
   self)
  (:obj_id
   (&optional __obj_id)
   (if __obj_id (setq _obj_id __obj_id)) _obj_id)
  (:obj_index
   (&optional __obj_index)
   (if __obj_index (setq _obj_index __obj_index)) _obj_index)
  (:n_points
   (&optional __n_points)
   (if __n_points (setq _n_points __n_points)) _n_points)
  (:xyzvp
   (&optional __xyzvp)
   (if __xyzvp (setq _xyzvp __xyzvp)) _xyzvp)
  (:xyzvp_center
   (&optional __xyzvp_center)
   (if __xyzvp_center (setq _xyzvp_center __xyzvp_center)) _xyzvp_center)
  (:tta_scores
   (&optional __tta_scores)
   (if __tta_scores (setq _tta_scores __tta_scores)) _tta_scores)
  (:tta_count
   (&optional __tta_count)
   (if __tta_count (setq _tta_count __tta_count)) _tta_count)
  (:serialization-length
   ()
   (+
    ;; uint16 _obj_id
    2
    ;; uint16 _obj_index
    2
    ;; uint16 _n_points
    2
    ;; float32[] _xyzvp
    (* 4    (length _xyzvp)) 4
    ;; float32[] _xyzvp_center
    (* 4    (length _xyzvp_center)) 4
    ;; float32[] _tta_scores
    (* 4    (length _tta_scores)) 4
    ;; uint32 _tta_count
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; uint16 _obj_id
       (write-word _obj_id s)
     ;; uint16 _obj_index
       (write-word _obj_index s)
     ;; uint16 _n_points
       (write-word _n_points s)
     ;; float32[] _xyzvp
     (write-long (length _xyzvp) s)
     (dotimes (i (length _xyzvp))
       (sys::poke (elt _xyzvp i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
     ;; float32[] _xyzvp_center
     (write-long (length _xyzvp_center) s)
     (dotimes (i (length _xyzvp_center))
       (sys::poke (elt _xyzvp_center i) (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
       )
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
   ;; uint16 _obj_id
     (setq _obj_id (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _obj_index
     (setq _obj_index (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; uint16 _n_points
     (setq _n_points (sys::peek buf ptr- :short)) (incf ptr- 2)
   ;; float32[] _xyzvp
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _xyzvp (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _xyzvp i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
   ;; float32[] _xyzvp_center
   (let (n)
     (setq n (sys::peek buf ptr- :integer)) (incf ptr- 4)
     (setq _xyzvp_center (instantiate float-vector n))
     (dotimes (i n)
     (setf (elt _xyzvp_center i) (sys::peek buf ptr- :float)) (incf ptr- 4)
     ))
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

(setf (get arbe_msgs::arbeClassSingleQuery :md5sum-) "64f8fe3f54997e174c21d53624f859dd")
(setf (get arbe_msgs::arbeClassSingleQuery :datatype-) "arbe_msgs/arbeClassSingleQuery")
(setf (get arbe_msgs::arbeClassSingleQuery :definition-)
      "uint16 obj_id
uint16 obj_index
uint16 n_points
float32[] xyzvp
float32[] xyzvp_center
float32[] tta_scores
uint32 tta_count


")



(provide :arbe_msgs/arbeClassSingleQuery "64f8fe3f54997e174c21d53624f859dd")


